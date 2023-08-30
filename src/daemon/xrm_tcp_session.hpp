/*
 * Copyright (C) 2019-2021, Xilinx Inc - All rights reserved
 *
 * Copyright (C) 2023, Advanced Micro Devices, Inc. All rights reserved.
 *
 * Xilinx Resouce Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You may
 * not use this file except in compliance with the License. A copy of the
 * License is located at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#ifndef _XRM_TCP_SESSION_HPP_
#define _XRM_TCP_SESSION_HPP_

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include "xrm_command.hpp"
#include "xrm_command_registry.hpp"
#include "xrm_system.hpp"

using boost::asio::ip::tcp;

namespace xrm {

class session : public std::enable_shared_from_this<session> {
   public:
    session(tcp::socket socket) : m_socket(std::move(socket)) {}

    void start() { doRead(); }

    void setSystem(xrm::system* sys) { m_system = sys; }

    void setRegistry(xrm::commandRegistry* registry) { m_registry = registry; }

    uint64_t getClientId() const { return m_clientId; }
    pid_t getClientProcessId() const { return m_clientProcessId; }

   private:
    void doRead();
    void handleCmd(std::size_t length);
    void doWrite(std::size_t length);

    enum { max_length = 131072 };

    tcp::socket m_socket;
    uint64_t m_clientId = 0;
    pid_t m_clientProcessId = 0;
    char m_indata[max_length];
    char m_outdata[max_length];
    boost::property_tree::ptree m_cmdtree;
    xrm::system* m_system;
    xrm::commandRegistry* m_registry;
};
} // namespace xrm

#endif // _XRM_TCP_SESSION_HPP_
