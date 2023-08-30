/*
 * Copyright (C) 2019-2020, Xilinx Inc - All rights reserved
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

#include "xrm_command_unload.hpp"

/*
 * The function is for unload cmd line from xrmadm.
 */
void xrm::unloadCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    auto requestId = incmd.get<int>("request.requestId");
    outrsp.put("response.name", "unload");
    outrsp.put("response.requestId", requestId);

    boost::optional<pt::ptree&> unloadTree = incmd.get_child_optional("request.parameters.device");
    if (!unloadTree.is_initialized()) {
        outrsp.put("response.status", "failed");
        outrsp.put("response.data.failed", "request parameters device is not provided");
        return;
    }
    std::string errmsg;
    m_system->enterLock();
    int32_t ret;
    for (auto it : (pt::ptree&)(*unloadTree)) {
        auto devId = it.second.get_value<int32_t>();
        /*
         * devId: -2: unload all devices.
         *        0 - (m_numDevice -1): unload specified device.
         *        other: invalid device id.
         */
        if (devId == -2) {
            int32_t numDev = m_system->getDeviceNumber();
            for (int32_t i = 0; i < numDev; i++) {
                // to unload all devices
                ret = m_system->unloadOneDevice(i, errmsg);
                if (ret != XRM_SUCCESS) {
                    // one device unload fail, stop here
                    break;
                }
            }
            if (ret != XRM_SUCCESS) {
                outrsp.put("response.status", "failed");
                outrsp.put("response.data.failed", errmsg);
            } else {
                outrsp.put("response.status", "ok");
                outrsp.put("response.data.ok", "unload completed");
            }
            m_system->exitLock();
            return;
        } else {
            ret = m_system->unloadOneDevice(devId, errmsg);
            if (ret != XRM_SUCCESS) {
                outrsp.put("response.status", "failed");
                outrsp.put("response.data.failed", errmsg);
                m_system->exitLock();
                return;
            }
        }
    }
    m_system->exitLock();
    outrsp.put("response.status", "ok");
    outrsp.put("response.data.ok", "unload completed");
}

/*
 * The function is for unload interface from xrm library API.
 */
void xrm::unloadOneDeviceCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    auto requestId = incmd.get<int>("request.requestId");
    outrsp.put("response.name", "unloadOneDevice");
    outrsp.put("response.requestId", requestId);

    auto devId = incmd.get<int32_t>("request.parameters.deviceId");

    std::string errmsg;
    m_system->enterLock();
    int32_t ret = m_system->unloadOneDevice(devId, errmsg);
    m_system->exitLock();
    if (ret != XRM_SUCCESS) {
        outrsp.put("response.status.value", ret);
        outrsp.put("response.data.failed", errmsg);
        return;
    }
    outrsp.put("response.status.value", XRM_SUCCESS);
    outrsp.put("response.data.ok", "unload completed");
}
