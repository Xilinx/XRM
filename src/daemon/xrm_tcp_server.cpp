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

#include "xrm_tcp_session.hpp"
#include "xrm_tcp_server.hpp"

using boost::asio::ip::tcp;

void xrm::server::doAccept() {
    m_acceptor.async_accept(m_socket, [this](boost::system::error_code ec) {
        if (ec) {
            m_system->logMsg(XRM_LOG_ERROR, "%s: error %s = %d", __func__, ec.category().name(), ec.value());
        } else {
            auto thisSession = std::make_shared<xrm::session>(std::move(m_socket));
            thisSession->setSystem(m_system);
            thisSession->setRegistry(m_registry);
            thisSession->start();
        }

        doAccept();
    });
}
