/*
 * Copyright (C) 2019-2020, Xilinx Inc - All rights reserved
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

#ifndef _XRM_TCP_SERVER_HPP_
#define _XRM_TCP_SERVER_HPP_

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include "xrm_command.hpp"
#include "xrm_system.hpp"

using boost::asio::ip::tcp;

namespace xrm {
class server {
   public:
    server(boost::asio::io_service& ioService, short port)
        : m_acceptor(ioService, tcp::endpoint(tcp::v4(), port)), m_socket(ioService) {
        doAccept();
    }

    void setSystem(xrm::system* sys) { m_system = sys; }

    void setRegistry(xrm::commandRegistry* registry) { m_registry = registry; }

   private:
    void doAccept();

    tcp::acceptor m_acceptor;
    tcp::socket m_socket;
    xrm::system* m_system;
    xrm::commandRegistry* m_registry;
};
} // namespace xrm

#endif // _XRM_TCP_SERVER_HPP_
