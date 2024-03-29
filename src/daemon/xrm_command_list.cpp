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

#include "xrm_command_list.hpp"

void xrm::listCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    m_system->enterLock();
    int32_t numDev = m_system->getDeviceNumber();
    int32_t numPlugin = m_system->getXrmPluginNumber();
    int32_t devId, cuId, pluginId;
    std::string strDevId;
    int32_t listStartDevId, listEndDevId, listNumDev;

    auto requestId = incmd.get<int>("request.requestId");
    strDevId = incmd.get<std::string>("request.device", "");
    if (strDevId.c_str()[0] != '\0') {
        listStartDevId = std::stoi(strDevId, nullptr, 0);
        if (listStartDevId < 0 || listStartDevId >= numDev) {
            std::string errmsg;
            errmsg = "device " + std::to_string(listStartDevId) + " not found";
            outrsp.put("response.name", "list");
            outrsp.put("response.requestId", requestId);
            outrsp.put("response.status", "failed");
            outrsp.put("response.data.failed", errmsg);
            m_system->exitLock();
            return;
        } else {
            listEndDevId = listStartDevId + 1;
            listNumDev = 1;
        }
    } else {
        listStartDevId = 0;
        listEndDevId = numDev; // this id will NOT be listed
        listNumDev = numDev;
    }
    /* if there are too many device then try to list less cu */
    int32_t listCuLimit;
    if (listNumDev > 10)
        listCuLimit = 4;
    else
        listCuLimit = 7;

    outrsp.put("response.name", "list");
    outrsp.put("response.requestId", requestId);
    outrsp.put("response.status", "ok");
    outrsp.put("response.data.deviceNumber", numDev);
    for (devId = listStartDevId; devId < listEndDevId; devId++) {
        std::string devNode = "response.data.device_" + std::to_string(devId);
        xrm::deviceData* devData = NULL;
        devData = (xrm::deviceData*)m_system->listDevice(devId);
        outrsp.add(devNode + ".dsaName    ", devData->dsaName);
        if (devData->isLoaded) {
            outrsp.add(devNode + ".xclbinName ", devData->xclbinName);
            outrsp.add(devNode + ".uuid       ", devData->xclbinInfo.uuidStr);
            outrsp.add(devNode + ".isExclusive", devData->isExcl);
            outrsp.add(devNode + ".cuNumber   ", devData->xclbinInfo.numCu);
            for (cuId = 0; cuId < devData->xclbinInfo.numCu; cuId++) {
                if ((listNumDev > 1) && (cuId > listCuLimit)) {
                    std::string cuNode = "response.data.device_" + std::to_string(devId) + ".cu_others";
                    outrsp.add(cuNode + ".other cu     ", "details are omitted due to large number of cu");
                    break;
                }
                xrm::cuData* cuData = &(devData->xclbinInfo.cuList[cuId]);
                std::string cuNode = "response.data.device_" + std::to_string(devId) + ".cu_" + std::to_string(cuId);
                outrsp.add(cuNode + ".cuId         ", cuData->cuId);
                switch (cuData->cuType) {
                    case XRM_CU_NULL:
                        outrsp.add(cuNode + ".cuType       ", "Empty Kernel");
                        break;
                    case XRM_CU_IPKERNEL:
                        outrsp.add(cuNode + ".cuType       ", "IP Kernel");
                        break;
                    case XRM_CU_IPPSKERNEL:
                        outrsp.add(cuNode + ".cuType       ", "IP PS Kernel");
                        break;
                    case XRM_CU_SOFTKERNEL:
                        outrsp.add(cuNode + ".cuType       ", "Soft Kernel");
                        break;
                    default:
                        outrsp.add(cuNode + ".cuType       ", "Unknown Kernel");
                        break;
                }
                outrsp.add(cuNode + ".kernelName   ", cuData->kernelName);
                outrsp.add(cuNode + ".kernelAlias  ", cuData->kernelAlias);
                outrsp.add(cuNode + ".instanceName ", cuData->instanceName);
                outrsp.add(cuNode + ".cuName       ", cuData->cuName);
                outrsp.add(cuNode + ".kernelPlugin ", cuData->kernelPluginFileName);
                if (cuData->maxCapacity)
                    outrsp.add(cuNode + ".maxCapacity  ", cuData->maxCapacity);
                else
                    outrsp.add(cuNode + ".maxCapacity  ", "");
                outrsp.add(cuNode + ".numChanInuse ", cuData->numChanInuse);
                outrsp.add(cuNode + ".usedLoad     ", std::to_string(cuData->totalUsedLoadUnified) + " of 1000000");
                outrsp.add(cuNode + ".reservedLoad ", std::to_string(cuData->totalReservedLoadUnified) + " of 1000000");
                outrsp.add(cuNode + ".resrvUsedLoad",
                           std::to_string(cuData->totalReservedUsedLoadUnified) + " of 1000000");
            }
        } else {
            if (devData->isDisabled)
                outrsp.add(devNode + ".xclbin     ", "device is disabled, xclbin can't be loaded");
            else
                outrsp.add(devNode + ".xclbin     ", "is NOT loaded");
        }
    }
    outrsp.put("response.data.xrmPluginNumber", numPlugin);
    for (pluginId = 0; pluginId < numPlugin; pluginId++) {
        std::string pluginNode = "response.data.xrmPlugin_" + std::to_string(pluginId);
        xrm::pluginInformation* pluginInfo = NULL;
        pluginInfo = (xrm::pluginInformation*)m_system->listXrmPlugin(pluginId);
        outrsp.add(pluginNode + ".xrmPluginName    ", pluginInfo->xrmPluginName);
        outrsp.add(pluginNode + ".xrmPluginFileName", pluginInfo->xrmPluginFileName);
    }
    m_system->exitLock();
}
