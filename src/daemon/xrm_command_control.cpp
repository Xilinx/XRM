/*
 * Copyright (C) 2019-2021, Xilinx Inc - All rights reserved
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

#include "xrm_command_control.hpp"

/*
 * The function is for enable devices cmd line from xrmadm.
 */
void xrm::enableDevicesCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    auto requestId = incmd.get<int>("request.requestId");
    outrsp.put("response.name", "enableDevices");
    outrsp.put("response.requestId", requestId);

    boost::optional<pt::ptree&> enableDevicesTree = incmd.get_child_optional("request.parameters.device");
    if (!enableDevicesTree.is_initialized()) {
        outrsp.put("response.status", "failed");
        outrsp.put("response.data.failed", "request parameters device is not provided");
        return;
    }

    std::string errmsg;
    m_system->enterLock();
    for (auto it : (pt::ptree&)(*enableDevicesTree)) {
        uint32_t devId = it.second.get_value<uint32_t>();
        if (m_system->enableOneDevice(devId, errmsg) != XRM_SUCCESS) {
            outrsp.put("response.status", "failed");
            outrsp.put("response.data.failed", errmsg);
            m_system->exitLock();
            return;
        }
    }
    m_system->exitLock();
    outrsp.put("response.status", "ok");
    outrsp.put("response.data.ok", "enable devices completed");
}

/*
 * The function is for enableOneDevice interface from xrm library API.
 */
void xrm::enableOneDeviceCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    auto requestId = incmd.get<int>("request.requestId");
    outrsp.put("response.name", "enableOneDevice");
    outrsp.put("response.requestId", requestId);

    auto devId = incmd.get<int32_t>("request.parameters.deviceId");

    std::string errmsg;
    m_system->enterLock();
    int32_t ret = m_system->enableOneDevice(devId, errmsg);
    m_system->exitLock();
    if (ret != XRM_SUCCESS) {
        outrsp.put("response.status.value", ret);
        outrsp.put("response.data.failed", errmsg);
        return;
    }
    outrsp.put("response.status.value", XRM_SUCCESS);
    outrsp.put("response.data.ok", "enable one device completed");
}

/*
 * The function is for disable devices cmd line from xrmadm.
 */
void xrm::disableDevicesCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    auto requestId = incmd.get<int>("request.requestId");
    outrsp.put("response.name", "disableDevices");
    outrsp.put("response.requestId", requestId);

    boost::optional<pt::ptree&> disableDevicesTree = incmd.get_child_optional("request.parameters.device");
    if (!disableDevicesTree.is_initialized()) {
        outrsp.put("response.status", "failed");
        outrsp.put("response.data.failed", "request parameters device is not provided");
        return;
    }

    std::string errmsg;
    m_system->enterLock();
    for (auto it : (pt::ptree&)(*disableDevicesTree)) {
        uint32_t devId = it.second.get_value<uint32_t>();
        if (m_system->disableOneDevice(devId, errmsg) != XRM_SUCCESS) {
            outrsp.put("response.status", "failed");
            outrsp.put("response.data.failed", errmsg);
            m_system->exitLock();
            return;
        }
    }
    m_system->exitLock();
    outrsp.put("response.status", "ok");
    outrsp.put("response.data.ok", "disable devices completed");
}

/*
 * The function is for disableOneDevice interface from xrm library API.
 */
void xrm::disableOneDeviceCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    auto requestId = incmd.get<int>("request.requestId");
    outrsp.put("response.name", "disableOneDevice");
    outrsp.put("response.requestId", requestId);

    auto devId = incmd.get<int32_t>("request.parameters.deviceId");

    std::string errmsg;
    m_system->enterLock();
    int32_t ret = m_system->disableOneDevice(devId, errmsg);
    m_system->exitLock();
    if (ret != XRM_SUCCESS) {
        outrsp.put("response.status.value", ret);
        outrsp.put("response.data.failed", errmsg);
        return;
    }
    outrsp.put("response.status.value", XRM_SUCCESS);
    outrsp.put("response.data.ok", "disable one device completed");
}
