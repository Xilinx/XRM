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

#include "xrm_command_load.hpp"

/*
 * The function is for load cmd line from xrmadm.
 */
void xrm::loadCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    auto requestId = incmd.get<int>("request.requestId");
    outrsp.put("response.name", "load");
    outrsp.put("response.requestId", requestId);

    boost::optional<pt::ptree&> loadTree = incmd.get_child_optional("request.parameters.");
    if (!loadTree.is_initialized()) {
        outrsp.put("response.status", "failed");
        outrsp.put("response.data.failed", "request parameters is not provided");
        return;
    }
    std::string errmsg;
    if (m_system->loadDevices((pt::ptree&)*loadTree, errmsg) == XRM_SUCCESS) {
        outrsp.put("response.status", "ok");
    } else {
        outrsp.put("response.status", "failed");
        outrsp.put("response.data.failed", errmsg);
    }
}

/*
 * The function is for load interface from xrm library API.
 */
void xrm::loadOneDeviceCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    int32_t ret;
    auto requestId = incmd.get<int>("request.requestId");
    outrsp.put("response.name", "loadOneDevice");
    outrsp.put("response.requestId", requestId);

    boost::optional<pt::ptree&> loadTree = incmd.get_child_optional("request.parameters.");
    if (!loadTree.is_initialized()) {
        outrsp.put("response.status.value", XRM_ERROR);
        outrsp.put("response.data.failed", "request parameters is not provided");
        return;
    }
    std::string errmsg;
    ret = m_system->loadOneDevice((pt::ptree&)*loadTree, errmsg);
    if (ret >= 0) {
        outrsp.put("response.status.value", XRM_SUCCESS);
        outrsp.put("response.data.deviceId", ret);
    } else {
        outrsp.put("response.status.value", XRM_ERROR);
        outrsp.put("response.data.failed", errmsg);
    }
}
