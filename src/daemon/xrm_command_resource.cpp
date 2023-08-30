/*
 * Copyright (C) 2019-2021, Xilinx Inc - All rights reserved
 * Xilinx Resource Management
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

#include "xrm_command_resource.hpp"

void xrm::createContextCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    auto context = incmd.get<std::string>("request.parameters.context");
    int32_t logLevel = m_system->getLogLevel();
    uint64_t clientId;
    m_system->enterLock();
    if (m_system->incNumConcurrentClient())
        clientId = m_system->getNewClientId();
    else
        clientId = 0; // reach the limit of concurrent client, fail to create new context
    m_system->exitLock();
    outrsp.put("response.status.value", logLevel);
    outrsp.put("response.data.clientId", clientId);
}

void xrm::echoContextCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    auto context = incmd.get<std::string>("request.parameters.context");
    outrsp.put("response.status.value", XRM_SUCCESS);
}

void xrm::destroyContextCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    auto context = incmd.get<std::string>("request.parameters.context");
    m_system->enterLock();
#if 0
    m_system->decNumConcurrentClient();
    /* the save() function is time cost operation, so not do it here */
    // m_system->save();
#endif
#if 1
    /*
     * This is to make sure resource will be automaticly recycled after context destroy.
     * But there maybe duplicate work here since resource may already be released/relinquished.
     */
    auto clientId = incmd.get<uint64_t>("request.parameters.clientId");
    if (clientId) m_system->recycleResource(clientId);
#endif
    m_system->exitLock();
    outrsp.put("response.status.value", XRM_SUCCESS);
}

void xrm::isDaemonRunningCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    auto echo = incmd.get<std::string>("request.parameters.echo");
    if (echo.c_str()[0] == '\0') {
        outrsp.put("response.status.value", XRM_ERROR);
    } else {
        outrsp.put("response.status.value", XRM_SUCCESS);
    }
}

void xrm::cuAllocCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    cuProperty cuProp;
    cuResource cuRes;
    std::string errmsg;

    auto kernelName = incmd.get<std::string>("request.parameters.kernelName");
    auto kernelAlias = incmd.get<std::string>("request.parameters.kernelAlias");
    auto devExcl = incmd.get<int32_t>("request.parameters.devExcl");
    auto requestLoadUnified = incmd.get<int32_t>("request.parameters.requestLoadUnified");
    auto requestLoadOriginal = incmd.get<int32_t>("request.parameters.requestLoadOriginal");
    auto clientId = incmd.get<uint64_t>("request.parameters.clientId");
    auto clientProcessId = incmd.get<pid_t>("request.parameters.clientProcessId");
    auto poolId = incmd.get<uint64_t>("request.parameters.poolId");
    strncpy(cuProp.kernelName, kernelName.c_str(), XRM_MAX_NAME_LEN - 1);
    strncpy(cuProp.kernelAlias, kernelAlias.c_str(), XRM_MAX_NAME_LEN - 1);
    strcpy(cuProp.cuName, "");
    if (devExcl == 0)
        cuProp.devExcl = false;
    else
        cuProp.devExcl = true;
    cuProp.requestLoadUnified = requestLoadUnified;
    cuProp.requestLoadOriginal = requestLoadOriginal;
    cuProp.clientId = clientId;
    cuProp.clientProcessId = clientProcessId;
    cuProp.poolId = poolId;

    bool update_id = true;
    m_system->enterLock();
    int32_t ret = m_system->resAllocCu(&cuProp, &cuRes, update_id);
    m_system->exitLock();
    outrsp.put("response.status.value", ret);
    if (ret == XRM_SUCCESS) {
        outrsp.put("response.data.xclbinFileName", cuRes.xclbinFileName);
        outrsp.put("response.data.uuidStr", cuRes.uuidStr);
        outrsp.put("response.data.kernelPluginFileName", cuRes.kernelPluginFileName);
        outrsp.put("response.data.kernelName", cuRes.kernelName);
        outrsp.put("response.data.instanceName", cuRes.instanceName);
        outrsp.put("response.data.cuName", cuRes.cuName);
        outrsp.put("response.data.kernelAlias", cuRes.kernelAlias);
        outrsp.put("response.data.deviceId", cuRes.deviceId);
        outrsp.put("response.data.cuId", cuRes.cuId);
        outrsp.put("response.data.channelId", cuRes.channelId);
        outrsp.put("response.data.cuType", (int32_t)cuRes.cuType);
        outrsp.put("response.data.baseAddr", cuRes.baseAddr);
        outrsp.put("response.data.membankId", cuRes.membankId);
        outrsp.put("response.data.membankType", cuRes.membankType);
        outrsp.put("response.data.membankSize", cuRes.membankSize);
        outrsp.put("response.data.membankBaseAddr", cuRes.membankBaseAddr);
        outrsp.put("response.data.allocServiceId", cuRes.allocServiceId);
        outrsp.put("response.data.channelLoadOriginal", cuRes.channelLoadOriginal);
        outrsp.put("response.data.poolId", cuRes.poolId);
    }
}

void xrm::cuAllocFromDevCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    cuProperty cuProp;
    cuResource cuRes;
    std::string errmsg;

    auto deviceId = incmd.get<std::int32_t>("request.parameters.deviceId");
    auto kernelName = incmd.get<std::string>("request.parameters.kernelName");
    auto kernelAlias = incmd.get<std::string>("request.parameters.kernelAlias");
    auto devExcl = incmd.get<int32_t>("request.parameters.devExcl");
    auto requestLoadUnified = incmd.get<int32_t>("request.parameters.requestLoadUnified");
    auto requestLoadOriginal = incmd.get<int32_t>("request.parameters.requestLoadOriginal");
    auto clientId = incmd.get<uint64_t>("request.parameters.clientId");
    auto clientProcessId = incmd.get<pid_t>("request.parameters.clientProcessId");
    auto poolId = incmd.get<uint64_t>("request.parameters.poolId");
    strncpy(cuProp.kernelName, kernelName.c_str(), XRM_MAX_NAME_LEN - 1);
    strncpy(cuProp.kernelAlias, kernelAlias.c_str(), XRM_MAX_NAME_LEN - 1);
    strcpy(cuProp.cuName, "");
    if (devExcl == 0)
        cuProp.devExcl = false;
    else
        cuProp.devExcl = true;
    cuProp.requestLoadUnified = requestLoadUnified;
    cuProp.requestLoadOriginal = requestLoadOriginal;
    cuProp.clientId = clientId;
    cuProp.clientProcessId = clientProcessId;
    cuProp.poolId = poolId;

    bool update_id = true;
    m_system->enterLock();
    int32_t ret = m_system->resAllocCuFromDev(deviceId, &cuProp, &cuRes, update_id);
    m_system->exitLock();
    outrsp.put("response.status.value", ret);
    if (ret == XRM_SUCCESS) {
        outrsp.put("response.data.xclbinFileName", cuRes.xclbinFileName);
        outrsp.put("response.data.uuidStr", cuRes.uuidStr);
        outrsp.put("response.data.kernelPluginFileName", cuRes.kernelPluginFileName);
        outrsp.put("response.data.kernelName", cuRes.kernelName);
        outrsp.put("response.data.instanceName", cuRes.instanceName);
        outrsp.put("response.data.cuName", cuRes.cuName);
        outrsp.put("response.data.kernelAlias", cuRes.kernelAlias);
        outrsp.put("response.data.deviceId", cuRes.deviceId);
        outrsp.put("response.data.cuId", cuRes.cuId);
        outrsp.put("response.data.channelId", cuRes.channelId);
        outrsp.put("response.data.cuType", (int32_t)cuRes.cuType);
        outrsp.put("response.data.baseAddr", cuRes.baseAddr);
        outrsp.put("response.data.membankId", cuRes.membankId);
        outrsp.put("response.data.membankType", cuRes.membankType);
        outrsp.put("response.data.membankSize", cuRes.membankSize);
        outrsp.put("response.data.membankBaseAddr", cuRes.membankBaseAddr);
        outrsp.put("response.data.allocServiceId", cuRes.allocServiceId);
        outrsp.put("response.data.channelLoadOriginal", cuRes.channelLoadOriginal);
        outrsp.put("response.data.poolId", cuRes.poolId);
    }
}

void xrm::cuAllocLeastUsedFromDevCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    cuProperty cuProp;
    cuResource cuRes;
    std::string errmsg;

    auto deviceId = incmd.get<std::int32_t>("request.parameters.deviceId");
    auto kernelName = incmd.get<std::string>("request.parameters.kernelName");
    auto kernelAlias = incmd.get<std::string>("request.parameters.kernelAlias");
    auto devExcl = incmd.get<int32_t>("request.parameters.devExcl");
    auto requestLoadUnified = incmd.get<int32_t>("request.parameters.requestLoadUnified");
    auto requestLoadOriginal = incmd.get<int32_t>("request.parameters.requestLoadOriginal");
    auto clientId = incmd.get<uint64_t>("request.parameters.clientId");
    auto clientProcessId = incmd.get<pid_t>("request.parameters.clientProcessId");
    auto poolId = incmd.get<uint64_t>("request.parameters.poolId");
    strncpy(cuProp.kernelName, kernelName.c_str(), XRM_MAX_NAME_LEN - 1);
    strncpy(cuProp.kernelAlias, kernelAlias.c_str(), XRM_MAX_NAME_LEN - 1);
    strcpy(cuProp.cuName, "");
    if (devExcl == 0)
        cuProp.devExcl = false;
    else
        cuProp.devExcl = true;
    cuProp.requestLoadUnified = requestLoadUnified;
    cuProp.requestLoadOriginal = requestLoadOriginal;
    cuProp.clientId = clientId;
    cuProp.clientProcessId = clientProcessId;
    cuProp.poolId = poolId;

    bool update_id = true;
    m_system->enterLock();
    int32_t ret = m_system->resAllocLeastUsedCuFromDev(deviceId, &cuProp, &cuRes, update_id);
    m_system->exitLock();
    outrsp.put("response.status.value", ret);
    if (ret == XRM_SUCCESS) {
        outrsp.put("response.data.xclbinFileName", cuRes.xclbinFileName);
        outrsp.put("response.data.uuidStr", cuRes.uuidStr);
        outrsp.put("response.data.kernelPluginFileName", cuRes.kernelPluginFileName);
        outrsp.put("response.data.kernelName", cuRes.kernelName);
        outrsp.put("response.data.instanceName", cuRes.instanceName);
        outrsp.put("response.data.cuName", cuRes.cuName);
        outrsp.put("response.data.kernelAlias", cuRes.kernelAlias);
        outrsp.put("response.data.deviceId", cuRes.deviceId);
        outrsp.put("response.data.cuId", cuRes.cuId);
        outrsp.put("response.data.channelId", cuRes.channelId);
        outrsp.put("response.data.cuType", (int32_t)cuRes.cuType);
        outrsp.put("response.data.baseAddr", cuRes.baseAddr);
        outrsp.put("response.data.membankId", cuRes.membankId);
        outrsp.put("response.data.membankType", cuRes.membankType);
        outrsp.put("response.data.membankSize", cuRes.membankSize);
        outrsp.put("response.data.membankBaseAddr", cuRes.membankBaseAddr);
        outrsp.put("response.data.allocServiceId", cuRes.allocServiceId);
        outrsp.put("response.data.channelLoadOriginal", cuRes.channelLoadOriginal);
        outrsp.put("response.data.poolId", cuRes.poolId);
    }
}

void xrm::cuListAllocCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    cuListProperty cuListProp;
    cuListResource cuListRes;
    std::string errmsg;
    int32_t i;

    memset(&cuListProp, 0, sizeof(cuListProperty));
    auto cuNum = incmd.get<int32_t>("request.parameters.cuNum");
    cuListProp.cuNum = cuNum;
    auto sameDevice = incmd.get<int32_t>("request.parameters.sameDevice");
    auto clientId = incmd.get<uint64_t>("request.parameters.clientId");
    auto clientProcessId = incmd.get<pid_t>("request.parameters.clientProcessId");
    if (sameDevice == 0)
        cuListProp.sameDevice = false;
    else
        cuListProp.sameDevice = true;
    for (i = 0; i < cuListProp.cuNum; i++) {
        auto kernelName = incmd.get<std::string>("request.parameters.kernelName" + std::to_string(i));
        auto kernelAlias = incmd.get<std::string>("request.parameters.kernelAlias" + std::to_string(i));
        auto devExcl = incmd.get<int32_t>("request.parameters.devExcl" + std::to_string(i));
        auto requestLoadUnified = incmd.get<int32_t>("request.parameters.requestLoadUnified" + std::to_string(i));
        auto requestLoadOriginal = incmd.get<int32_t>("request.parameters.requestLoadOriginal" + std::to_string(i));
        auto poolId = incmd.get<uint64_t>("request.parameters.poolId" + std::to_string(i));
        strncpy(cuListProp.cuProps[i].kernelName, kernelName.c_str(), XRM_MAX_NAME_LEN - 1);
        strncpy(cuListProp.cuProps[i].kernelAlias, kernelAlias.c_str(), XRM_MAX_NAME_LEN - 1);
        strcpy(cuListProp.cuProps[i].cuName, "");
        if (devExcl == 0)
            cuListProp.cuProps[i].devExcl = false;
        else
            cuListProp.cuProps[i].devExcl = true;
        cuListProp.cuProps[i].requestLoadUnified = requestLoadUnified;
        cuListProp.cuProps[i].requestLoadOriginal = requestLoadOriginal;
        cuListProp.cuProps[i].clientId = clientId;
        cuListProp.cuProps[i].clientProcessId = clientProcessId;
        cuListProp.cuProps[i].poolId = poolId;
    }

    memset(&cuListRes, 0, sizeof(cuListResource));
    m_system->enterLock();
    int32_t ret = m_system->resAllocCuList(&cuListProp, &cuListRes);
    m_system->exitLock();
    outrsp.put("response.status.value", ret);
    if (ret == XRM_SUCCESS) {
        outrsp.put("response.data.cuNum", cuListRes.cuNum);
        for (i = 0; i < cuListRes.cuNum; i++) {
            outrsp.put("response.data.xclbinFileName" + std::to_string(i), cuListRes.cuResources[i].xclbinFileName);
            outrsp.put("response.data.uuidStr" + std::to_string(i), cuListRes.cuResources[i].uuidStr);
            outrsp.put("response.data.kernelPluginFileName" + std::to_string(i),
                       cuListRes.cuResources[i].kernelPluginFileName);
            outrsp.put("response.data.kernelName" + std::to_string(i), cuListRes.cuResources[i].kernelName);
            outrsp.put("response.data.instanceName" + std::to_string(i), cuListRes.cuResources[i].instanceName);
            outrsp.put("response.data.cuName" + std::to_string(i), cuListRes.cuResources[i].cuName);
            outrsp.put("response.data.kernelAlias" + std::to_string(i), cuListRes.cuResources[i].kernelAlias);
            outrsp.put("response.data.deviceId" + std::to_string(i), cuListRes.cuResources[i].deviceId);
            outrsp.put("response.data.cuId" + std::to_string(i), cuListRes.cuResources[i].cuId);
            outrsp.put("response.data.channelId" + std::to_string(i), cuListRes.cuResources[i].channelId);
            outrsp.put("response.data.cuType" + std::to_string(i), (int32_t)cuListRes.cuResources[i].cuType);
            outrsp.put("response.data.baseAddr" + std::to_string(i), cuListRes.cuResources[i].baseAddr);
            outrsp.put("response.data.membankId" + std::to_string(i), cuListRes.cuResources[i].membankId);
            outrsp.put("response.data.membankType" + std::to_string(i), cuListRes.cuResources[i].membankType);
            outrsp.put("response.data.membankSize" + std::to_string(i), cuListRes.cuResources[i].membankSize);
            outrsp.put("response.data.membankBaseAddr" + std::to_string(i), cuListRes.cuResources[i].membankBaseAddr);
            outrsp.put("response.data.allocServiceId" + std::to_string(i), cuListRes.cuResources[i].allocServiceId);
            outrsp.put("response.data.channelLoadOriginal" + std::to_string(i),
                       cuListRes.cuResources[i].channelLoadOriginal);
            outrsp.put("response.data.poolId" + std::to_string(i), cuListRes.cuResources[i].poolId);
        }
    }
}

void xrm::cuGroupAllocCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    cuGroupProperty cuGroupProp;
    cuGroupResource cuGroupRes;
    std::string errmsg;
    int32_t i;

    auto udfCuGroupName = incmd.get<std::string>("request.parameters.udfCuGroupName");
    auto clientId = incmd.get<uint64_t>("request.parameters.clientId");
    auto clientProcessId = incmd.get<pid_t>("request.parameters.clientProcessId");
    auto poolId = incmd.get<uint64_t>("request.parameters.poolId");
    cuGroupProp.udfCuGroupName = udfCuGroupName;
    cuGroupProp.clientId = clientId;
    cuGroupProp.clientProcessId = clientProcessId;
    cuGroupProp.poolId = poolId;

    memset(&cuGroupRes, 0, sizeof(cuGroupResource));
    m_system->enterLock();
    int32_t ret = m_system->resAllocCuGroup(&cuGroupProp, &cuGroupRes);
    m_system->exitLock();
    outrsp.put("response.status.value", ret);
    if (ret == XRM_SUCCESS) {
        outrsp.put("response.data.cuNum", cuGroupRes.cuNum);
        for (i = 0; i < cuGroupRes.cuNum; i++) {
            outrsp.put("response.data.xclbinFileName" + std::to_string(i), cuGroupRes.cuResources[i].xclbinFileName);
            outrsp.put("response.data.uuidStr" + std::to_string(i), cuGroupRes.cuResources[i].uuidStr);
            outrsp.put("response.data.kernelPluginFileName" + std::to_string(i),
                       cuGroupRes.cuResources[i].kernelPluginFileName);
            outrsp.put("response.data.kernelName" + std::to_string(i), cuGroupRes.cuResources[i].kernelName);
            outrsp.put("response.data.instanceName" + std::to_string(i), cuGroupRes.cuResources[i].instanceName);
            outrsp.put("response.data.cuName" + std::to_string(i), cuGroupRes.cuResources[i].cuName);
            outrsp.put("response.data.kernelAlias" + std::to_string(i), cuGroupRes.cuResources[i].kernelAlias);
            outrsp.put("response.data.deviceId" + std::to_string(i), cuGroupRes.cuResources[i].deviceId);
            outrsp.put("response.data.cuId" + std::to_string(i), cuGroupRes.cuResources[i].cuId);
            outrsp.put("response.data.channelId" + std::to_string(i), cuGroupRes.cuResources[i].channelId);
            outrsp.put("response.data.cuType" + std::to_string(i), (int32_t)cuGroupRes.cuResources[i].cuType);
            outrsp.put("response.data.baseAddr" + std::to_string(i), cuGroupRes.cuResources[i].baseAddr);
            outrsp.put("response.data.membankId" + std::to_string(i), cuGroupRes.cuResources[i].membankId);
            outrsp.put("response.data.membankType" + std::to_string(i), cuGroupRes.cuResources[i].membankType);
            outrsp.put("response.data.membankSize" + std::to_string(i), cuGroupRes.cuResources[i].membankSize);
            outrsp.put("response.data.membankBaseAddr" + std::to_string(i), cuGroupRes.cuResources[i].membankBaseAddr);
            outrsp.put("response.data.allocServiceId" + std::to_string(i), cuGroupRes.cuResources[i].allocServiceId);
            outrsp.put("response.data.channelLoadOriginal" + std::to_string(i),
                       cuGroupRes.cuResources[i].channelLoadOriginal);
            outrsp.put("response.data.poolId" + std::to_string(i), cuGroupRes.cuResources[i].poolId);
        }
    }
}

void xrm::cuReleaseCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    cuResource cuRes;
    std::string errmsg;

    auto deviceId = incmd.get<int32_t>("request.parameters.deviceId");
    auto cuId = incmd.get<int32_t>("request.parameters.cuId");
    auto channelId = incmd.get<int32_t>("request.parameters.channelId");
    auto cuType = incmd.get<int32_t>("request.parameters.cuType");
    auto allocServiceId = incmd.get<uint64_t>("request.parameters.allocServiceId");
    auto clientId = incmd.get<uint64_t>("request.parameters.clientId");
    auto channelLoadUnified = incmd.get<int32_t>("request.parameters.channelLoadUnified");
    auto channelLoadOriginal = incmd.get<int32_t>("request.parameters.channelLoadOriginal");
    auto poolId = incmd.get<uint64_t>("request.parameters.poolId");
    cuRes.deviceId = deviceId;
    cuRes.cuId = cuId;
    cuRes.channelId = channelId;
    cuRes.cuType = (xrmCuType)cuType;
    cuRes.allocServiceId = allocServiceId;
    cuRes.clientId = clientId;
    cuRes.channelLoadUnified = channelLoadUnified;
    cuRes.channelLoadOriginal = channelLoadOriginal;
    cuRes.poolId = poolId;

    m_system->enterLock();
    int32_t ret = m_system->resReleaseCu(&cuRes);
    m_system->exitLock();
    outrsp.put("response.status.value", ret);
}

void xrm::cuListReleaseCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    cuListResource cuListRes;
    std::string errmsg;
    int32_t i;

    memset(&cuListRes, 0, sizeof(cuListResource));
    auto cuNum = incmd.get<int32_t>("request.parameters.cuNum");
    auto clientId = incmd.get<uint64_t>("request.parameters.clientId");
    cuListRes.cuNum = cuNum;
    for (i = 0; i < cuListRes.cuNum; i++) {
        auto deviceId = incmd.get<int32_t>("request.parameters.deviceId" + std::to_string(i));
        auto cuId = incmd.get<int32_t>("request.parameters.cuId" + std::to_string(i));
        auto channelId = incmd.get<int32_t>("request.parameters.channelId" + std::to_string(i));
        auto cuType = incmd.get<int32_t>("request.parameters.cuType" + std::to_string(i));
        auto allocServiceId = incmd.get<uint64_t>("request.parameters.allocServiceId" + std::to_string(i));
        auto channelLoadUnified = incmd.get<int32_t>("request.parameters.channelLoadUnified" + std::to_string(i));
        auto channelLoadOriginal = incmd.get<int32_t>("request.parameters.channelLoadOriginal" + std::to_string(i));
        auto poolId = incmd.get<uint64_t>("request.parameters.poolId" + std::to_string(i));
        cuListRes.cuResources[i].deviceId = deviceId;
        cuListRes.cuResources[i].cuId = cuId;
        cuListRes.cuResources[i].channelId = channelId;
        cuListRes.cuResources[i].cuType = (xrmCuType)cuType;
        cuListRes.cuResources[i].allocServiceId = allocServiceId;
        cuListRes.cuResources[i].clientId = clientId;
        cuListRes.cuResources[i].channelLoadUnified = channelLoadUnified;
        cuListRes.cuResources[i].channelLoadOriginal = channelLoadOriginal;
        cuListRes.cuResources[i].poolId = poolId;
    }

    m_system->enterLock();
    int32_t ret = m_system->resReleaseCuList(&cuListRes);
    m_system->exitLock();
    outrsp.put("response.status.value", ret);
}

void xrm::cuGroupReleaseCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    cuGroupResource cuGroupRes;
    std::string errmsg;
    int32_t i;

    memset(&cuGroupRes, 0, sizeof(cuGroupResource));
    auto cuNum = incmd.get<int32_t>("request.parameters.cuNum");
    cuGroupRes.cuNum = cuNum;
    auto clientId = incmd.get<uint64_t>("request.parameters.clientId");
    for (i = 0; i < cuGroupRes.cuNum; i++) {
        auto deviceId = incmd.get<int32_t>("request.parameters.deviceId" + std::to_string(i));
        auto cuId = incmd.get<int32_t>("request.parameters.cuId" + std::to_string(i));
        auto channelId = incmd.get<int32_t>("request.parameters.channelId" + std::to_string(i));
        auto cuType = incmd.get<int32_t>("request.parameters.cuType" + std::to_string(i));
        auto allocServiceId = incmd.get<uint64_t>("request.parameters.allocServiceId" + std::to_string(i));
        auto channelLoadUnified = incmd.get<int32_t>("request.parameters.channelLoadUnified" + std::to_string(i));
        auto channelLoadOriginal = incmd.get<int32_t>("request.parameters.channelLoadOriginal" + std::to_string(i));
        auto poolId = incmd.get<uint64_t>("request.parameters.poolId" + std::to_string(i));
        cuGroupRes.cuResources[i].deviceId = deviceId;
        cuGroupRes.cuResources[i].cuId = cuId;
        cuGroupRes.cuResources[i].channelId = channelId;
        cuGroupRes.cuResources[i].cuType = (xrmCuType)cuType;
        cuGroupRes.cuResources[i].allocServiceId = allocServiceId;
        cuGroupRes.cuResources[i].clientId = clientId;
        cuGroupRes.cuResources[i].channelLoadUnified = channelLoadUnified;
        cuGroupRes.cuResources[i].channelLoadOriginal = channelLoadOriginal;
        cuGroupRes.cuResources[i].poolId = poolId;
    }

    m_system->enterLock();
    int32_t ret = m_system->resReleaseCuGroup(&cuGroupRes);
    m_system->exitLock();
    outrsp.put("response.status.value", ret);
}

void xrm::udfCuGroupDeclareCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    udfCuGroupInformation udfCuGroupInfo;
    std::string errmsg;
    int32_t i, cuListIdx;
    cuProperty* cuProp;
    cuListProperty* cuListProp;

    auto udfCuGroupName = incmd.get<std::string>("request.parameters.udfCuGroupName");
    auto optionUdfCuListNum = incmd.get<int32_t>("request.parameters.optionUdfCuListNum");
    udfCuGroupInfo.udfCuGroupName = udfCuGroupName;
    udfCuGroupInfo.optionUdfCuListNum = optionUdfCuListNum;

    for (cuListIdx = 0; cuListIdx < optionUdfCuListNum; cuListIdx++) {
        cuListProp = &udfCuGroupInfo.optionUdfCuListProps[cuListIdx];
        std::string udfCuListStr = "request.parameters.optionUdfCuListProps" + std::to_string(cuListIdx);
        auto sameDevice = incmd.get<int32_t>(udfCuListStr + ".sameDevice");
        if (sameDevice == 0)
            cuListProp->sameDevice = false;
        else
            cuListProp->sameDevice = true;
        auto cuNum = incmd.get<int32_t>(udfCuListStr + ".cuNum");
        cuListProp->cuNum = cuNum;
        for (i = 0; i < cuNum; i++) {
            cuProp = &cuListProp->cuProps[i];
            auto cuName = incmd.get<std::string>(udfCuListStr + ".cuName" + std::to_string(i));
            strncpy(cuProp->cuName, cuName.c_str(), XRM_MAX_NAME_LEN - 1);
            auto devExcl = incmd.get<int32_t>(udfCuListStr + ".devExcl" + std::to_string(i));
            if (devExcl == 0)
                cuProp->devExcl = false;
            else
                cuProp->devExcl = true;
            auto requestLoadUnified = incmd.get<int32_t>(udfCuListStr + ".requestLoadUnified" + std::to_string(i));
            auto requestLoadOriginal = incmd.get<int32_t>(udfCuListStr + ".requestLoadOriginal" + std::to_string(i));
            cuProp->requestLoadUnified = requestLoadUnified;
            cuProp->requestLoadOriginal = requestLoadOriginal;
        }
    }

    m_system->enterLock();
    int32_t ret = m_system->resUdfCuGroupDeclare(&udfCuGroupInfo);
    m_system->exitLock();
    outrsp.put("response.status.value", ret);
    if (ret != XRM_SUCCESS) {
        outrsp.put("response.data.failed", "failed to declare user defined cu group");
    }
}

void xrm::udfCuGroupUndeclareCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    udfCuGroupInformation udfCuGroupInfo;
    std::string errmsg;

    auto udfCuGroupName = incmd.get<std::string>("request.parameters.udfCuGroupName");
    udfCuGroupInfo.udfCuGroupName = udfCuGroupName;

    m_system->enterLock();
    int32_t ret = m_system->resUdfCuGroupUndeclare(&udfCuGroupInfo);
    m_system->exitLock();
    outrsp.put("response.status.value", ret);
    if (ret != XRM_SUCCESS) {
        outrsp.put("response.data.failed", "failed to undeclare user defined cu group");
    }
}

void xrm::cuGetMaxCapacityCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    cuProperty cuProp;
    std::string errmsg;

    auto kernelName = incmd.get<std::string>("request.parameters.kernelName");
    auto kernelAlias = incmd.get<std::string>("request.parameters.kernelAlias");
    strncpy(cuProp.kernelName, kernelName.c_str(), XRM_MAX_NAME_LEN - 1);
    strncpy(cuProp.kernelAlias, kernelAlias.c_str(), XRM_MAX_NAME_LEN - 1);

    m_system->enterLock();
    uint64_t maxCapacity = m_system->getCuMaxCapacity(&cuProp);
    m_system->exitLock();
    outrsp.put("response.status.value", maxCapacity);
}

void xrm::cuCheckStatusCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    cuResource cuRes;
    cuStatus cuStat;
    std::string errmsg;

    auto deviceId = incmd.get<int32_t>("request.parameters.deviceId");
    auto cuId = incmd.get<int32_t>("request.parameters.cuId");
    auto channelId = incmd.get<int32_t>("request.parameters.channelId");
    auto cuType = incmd.get<int32_t>("request.parameters.cuType");
    auto allocServiceId = incmd.get<uint64_t>("request.parameters.allocServiceId");
    cuRes.deviceId = deviceId;
    cuRes.cuId = cuId;
    cuRes.channelId = channelId;
    cuRes.cuType = (xrmCuType)cuType;
    cuRes.allocServiceId = allocServiceId;

    m_system->enterLock();
    int32_t ret = m_system->checkCuStat(&cuRes, &cuStat);
    m_system->exitLock();
    outrsp.put("response.status.value", ret);
    if (ret == XRM_SUCCESS) {
        if (cuStat.isBusy)
            outrsp.put("response.data.isBusy", 1);
        else
            outrsp.put("response.data.isBusy", 0);
        outrsp.put("response.data.usedLoadOriginal", cuStat.usedLoadOriginal);
    } else {
        outrsp.put("response.data.failed", "failed to check cu status");
    }
}

void xrm::allocationQueryCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    allocationQueryInfo allocQuery;
    cuListResource cuListRes;
    std::string errmsg;
    int32_t i;

    memset(&allocQuery, 0, sizeof(allocationQueryInfo));
    auto allocServiceId = incmd.get<uint64_t>("request.parameters.allocServiceId");
    auto kernelName = incmd.get<std::string>("request.parameters.kernelName");
    auto kernelAlias = incmd.get<std::string>("request.parameters.kernelAlias");

    memset(&cuListRes, 0, sizeof(cuListResource));
    allocQuery.allocServiceId = allocServiceId;
    strncpy(allocQuery.kernelName, kernelName.c_str(), XRM_MAX_NAME_LEN - 1);
    strncpy(allocQuery.kernelAlias, kernelAlias.c_str(), XRM_MAX_NAME_LEN - 1);
    m_system->enterLock();
    int32_t ret = m_system->resAllocationQuery(&allocQuery, &cuListRes);
    m_system->exitLock();
    outrsp.put("response.status.value", ret);
    if (ret == XRM_SUCCESS) {
        outrsp.put("response.data.cuNum", cuListRes.cuNum);
        for (i = 0; i < cuListRes.cuNum; i++) {
            outrsp.put("response.data.xclbinFileName" + std::to_string(i), cuListRes.cuResources[i].xclbinFileName);
            outrsp.put("response.data.uuidStr" + std::to_string(i), cuListRes.cuResources[i].uuidStr);
            outrsp.put("response.data.kernelPluginFileName" + std::to_string(i),
                       cuListRes.cuResources[i].kernelPluginFileName);
            outrsp.put("response.data.kernelName" + std::to_string(i), cuListRes.cuResources[i].kernelName);
            outrsp.put("response.data.instanceName" + std::to_string(i), cuListRes.cuResources[i].instanceName);
            outrsp.put("response.data.cuName" + std::to_string(i), cuListRes.cuResources[i].cuName);
            outrsp.put("response.data.kernelAlias" + std::to_string(i), cuListRes.cuResources[i].kernelAlias);
            outrsp.put("response.data.deviceId" + std::to_string(i), cuListRes.cuResources[i].deviceId);
            outrsp.put("response.data.cuId" + std::to_string(i), cuListRes.cuResources[i].cuId);
            outrsp.put("response.data.channelId" + std::to_string(i), cuListRes.cuResources[i].channelId);
            outrsp.put("response.data.cuType" + std::to_string(i), (int32_t)cuListRes.cuResources[i].cuType);
            outrsp.put("response.data.baseAddr" + std::to_string(i), (int32_t)cuListRes.cuResources[i].baseAddr);
            outrsp.put("response.data.membankId" + std::to_string(i), (int32_t)cuListRes.cuResources[i].membankId);
            outrsp.put("response.data.membankType" + std::to_string(i), (int32_t)cuListRes.cuResources[i].membankType);
            outrsp.put("response.data.membankSize" + std::to_string(i), (int32_t)cuListRes.cuResources[i].membankSize);
            outrsp.put("response.data.membankBaseAddr" + std::to_string(i),
                       (int32_t)cuListRes.cuResources[i].membankBaseAddr);
            outrsp.put("response.data.allocServiceId" + std::to_string(i), cuListRes.cuResources[i].allocServiceId);
            outrsp.put("response.data.channelLoadOriginal" + std::to_string(i),
                       cuListRes.cuResources[i].channelLoadOriginal);
            outrsp.put("response.data.poolId" + std::to_string(i), cuListRes.cuResources[i].poolId);
        }
    }
}

void xrm::isCuExistingCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    cuProperty cuProp;
    bool isCuExisting = false;

    auto kernelName = incmd.get<std::string>("request.parameters.kernelName");
    auto kernelAlias = incmd.get<std::string>("request.parameters.kernelAlias");
    strncpy(cuProp.kernelName, kernelName.c_str(), XRM_MAX_NAME_LEN - 1);
    strncpy(cuProp.kernelAlias, kernelAlias.c_str(), XRM_MAX_NAME_LEN - 1);
    strcpy(cuProp.cuName, "");

    m_system->enterLock();
    isCuExisting = m_system->resIsCuExisting(&cuProp);
    m_system->exitLock();
    if (isCuExisting) {
        outrsp.put("response.status.value", XRM_SUCCESS);
        outrsp.put("response.data.isCuExisting", 1);
    } else {
        outrsp.put("response.status.value", XRM_SUCCESS);
        outrsp.put("response.data.isCuExisting", 0);
    }
}

void xrm::isCuListExistingCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    cuListProperty cuListProp;
    std::string errmsg;
    int32_t i;
    bool isCuListExisting = false;

    memset(&cuListProp, 0, sizeof(cuListProperty));
    auto cuNum = incmd.get<int32_t>("request.parameters.cuNum");
    cuListProp.cuNum = cuNum;
    auto sameDevice = incmd.get<int32_t>("request.parameters.sameDevice");
    if (sameDevice == 0)
        cuListProp.sameDevice = false;
    else
        cuListProp.sameDevice = true;
    for (i = 0; i < cuListProp.cuNum; i++) {
        auto kernelName = incmd.get<std::string>("request.parameters.kernelName" + std::to_string(i));
        auto kernelAlias = incmd.get<std::string>("request.parameters.kernelAlias" + std::to_string(i));
        strncpy(cuListProp.cuProps[i].kernelName, kernelName.c_str(), XRM_MAX_NAME_LEN - 1);
        strncpy(cuListProp.cuProps[i].kernelAlias, kernelAlias.c_str(), XRM_MAX_NAME_LEN - 1);
        strcpy(cuListProp.cuProps[i].cuName, "");
    }

    m_system->enterLock();
    isCuListExisting = m_system->resIsCuListExisting(&cuListProp);
    m_system->exitLock();
    if (isCuListExisting) {
        outrsp.put("response.status.value", XRM_SUCCESS);
        outrsp.put("response.data.isCuListExisting", 1);
    } else {
        outrsp.put("response.status.value", XRM_SUCCESS);
        outrsp.put("response.data.isCuListExisting", 0);
    }
}

void xrm::isCuGroupExistingCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    cuGroupProperty cuGroupProp;
    bool isCuGroupExisting = false;

    auto udfCuGroupName = incmd.get<std::string>("request.parameters.udfCuGroupName");
    cuGroupProp.udfCuGroupName = udfCuGroupName;

    m_system->enterLock();
    isCuGroupExisting = m_system->resIsCuGroupExisting(&cuGroupProp);
    m_system->exitLock();
    if (isCuGroupExisting) {
        outrsp.put("response.status.value", XRM_SUCCESS);
        outrsp.put("response.data.isCuGroupExisting", 1);
    } else {
        outrsp.put("response.status.value", XRM_SUCCESS);
        outrsp.put("response.data.isCuGroupExisting", 0);
    }
}

void xrm::checkCuAvailableNumCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    cuProperty cuProp;
    std::vector<cuResource*> cuRes;
    std::string errmsg;
    cuResource* tmpCuRes;
    int32_t i, ret;
    int32_t availableCuNum = 0;

    auto kernelName = incmd.get<std::string>("request.parameters.kernelName");
    auto kernelAlias = incmd.get<std::string>("request.parameters.kernelAlias");
    auto devExcl = incmd.get<int32_t>("request.parameters.devExcl");
    auto requestLoadUnified = incmd.get<int32_t>("request.parameters.requestLoadUnified");
    auto requestLoadOriginal = incmd.get<int32_t>("request.parameters.requestLoadOriginal");
    auto clientId = incmd.get<uint64_t>("request.parameters.clientId");
    auto clientProcessId = incmd.get<pid_t>("request.parameters.clientProcessId");
    auto poolId = incmd.get<uint64_t>("request.parameters.poolId");
    strncpy(cuProp.kernelName, kernelName.c_str(), XRM_MAX_NAME_LEN - 1);
    strncpy(cuProp.kernelAlias, kernelAlias.c_str(), XRM_MAX_NAME_LEN - 1);
    strcpy(cuProp.cuName, "");
    if (devExcl == 0)
        cuProp.devExcl = false;
    else
        cuProp.devExcl = true;
    cuProp.requestLoadUnified = requestLoadUnified;
    cuProp.requestLoadOriginal = requestLoadOriginal;
    cuProp.clientId = clientId;
    cuProp.clientProcessId = clientProcessId;
    cuProp.poolId = poolId;

    bool update_id = true;
    m_system->enterLock();
    do {
        tmpCuRes = (cuResource*)malloc(sizeof(cuResource));
        memset(tmpCuRes, 0, sizeof(cuResource));
        ret = m_system->resAllocCu(&cuProp, tmpCuRes, update_id);
        if (ret == XRM_SUCCESS) {
            cuRes.push_back(tmpCuRes);
            availableCuNum++;
        } else {
            free(tmpCuRes);
            break;
        }
    } while (ret == XRM_SUCCESS);
    if (availableCuNum > 0) {
        /* get the available cu number */
        for (i = 0; i < availableCuNum; i++) {
            m_system->resReleaseCu(cuRes[i]);
            free(cuRes[i]);
        }
        outrsp.put("response.status.value", XRM_SUCCESS);
        outrsp.put("response.data.availableCuNum", availableCuNum);
    } else {
        /*
         * There is no available resource or input is invalid.
         * Available cu list num is 0.
         */
        if (ret == XRM_ERROR_INVALID) {
            /* The input is invalid */
            outrsp.put("response.status.value", XRM_ERROR_INVALID);
            outrsp.put("response.data.failed", "failed to check available cu number");
        } else {
            /* there is no available resource */
            outrsp.put("response.status.value", XRM_SUCCESS);
            outrsp.put("response.data.availableCuNum", availableCuNum);
        }
    }
    m_system->exitLock();
}

void xrm::checkCuListAvailableNumCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    cuListProperty cuListProp;
    std::vector<cuListResource*> cuListRes;
    cuListResource* tmpCuListRes;
    std::string errmsg;
    int32_t i, ret;
    int32_t availableListNum = 0;

    memset(&cuListProp, 0, sizeof(cuListProperty));
    auto cuNum = incmd.get<int32_t>("request.parameters.cuNum");
    cuListProp.cuNum = cuNum;
    auto sameDevice = incmd.get<int32_t>("request.parameters.sameDevice");
    auto clientId = incmd.get<uint64_t>("request.parameters.clientId");
    auto clientProcessId = incmd.get<pid_t>("request.parameters.clientProcessId");
    if (sameDevice == 0)
        cuListProp.sameDevice = false;
    else
        cuListProp.sameDevice = true;
    for (i = 0; i < cuListProp.cuNum; i++) {
        auto kernelName = incmd.get<std::string>("request.parameters.kernelName" + std::to_string(i));
        auto kernelAlias = incmd.get<std::string>("request.parameters.kernelAlias" + std::to_string(i));
        auto devExcl = incmd.get<int32_t>("request.parameters.devExcl" + std::to_string(i));
        auto requestLoadUnified = incmd.get<int32_t>("request.parameters.requestLoadUnified" + std::to_string(i));
        auto requestLoadOriginal = incmd.get<int32_t>("request.parameters.requestLoadOriginal" + std::to_string(i));
        auto poolId = incmd.get<uint64_t>("request.parameters.poolId" + std::to_string(i));
        strncpy(cuListProp.cuProps[i].kernelName, kernelName.c_str(), XRM_MAX_NAME_LEN - 1);
        strncpy(cuListProp.cuProps[i].kernelAlias, kernelAlias.c_str(), XRM_MAX_NAME_LEN - 1);
        strcpy(cuListProp.cuProps[i].cuName, "");
        if (devExcl == 0)
            cuListProp.cuProps[i].devExcl = false;
        else
            cuListProp.cuProps[i].devExcl = true;
        cuListProp.cuProps[i].requestLoadUnified = requestLoadUnified;
        cuListProp.cuProps[i].requestLoadOriginal = requestLoadOriginal;
        cuListProp.cuProps[i].clientId = clientId;
        cuListProp.cuProps[i].clientProcessId = clientProcessId;
        cuListProp.cuProps[i].poolId = poolId;
    }

    m_system->enterLock();
    do {
        tmpCuListRes = (cuListResource*)malloc(sizeof(cuListResource));
        memset(tmpCuListRes, 0, sizeof(cuListResource));
        ret = m_system->resAllocCuList(&cuListProp, tmpCuListRes);
        if (ret == XRM_SUCCESS) {
            cuListRes.push_back(tmpCuListRes);
            availableListNum++;
        } else {
            free(tmpCuListRes);
            break;
        }
    } while (ret == XRM_SUCCESS);
    if (availableListNum > 0) {
        /* get the available cu list number */
        for (i = 0; i < availableListNum; i++) {
            m_system->resReleaseCuList(cuListRes[i]);
            free(cuListRes[i]);
        }
        outrsp.put("response.status.value", XRM_SUCCESS);
        outrsp.put("response.data.availableListNum", availableListNum);
    } else {
        /*
         * There is no available resource or input is invalid.
         * Available cu list num is 0.
         */
        if (ret == XRM_ERROR_INVALID) {
            /* The input is invalid */
            outrsp.put("response.status.value", XRM_ERROR_INVALID);
            outrsp.put("response.data.failed", "failed to check available cu list number");
        } else {
            /* there is no available resource */
            outrsp.put("response.status.value", XRM_SUCCESS);
            outrsp.put("response.data.availableListNum", availableListNum);
        }
    }
    m_system->exitLock();
}

void xrm::checkCuGroupAvailableNumCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    cuGroupProperty cuGroupProp;
    std::vector<cuGroupResource*> cuGroupRes;
    cuGroupResource* tmpCuGroupRes;
    std::string errmsg;
    int32_t i, ret;
    int32_t availableGroupNum = 0;

    auto udfCuGroupName = incmd.get<std::string>("request.parameters.udfCuGroupName");
    auto clientId = incmd.get<uint64_t>("request.parameters.clientId");
    auto clientProcessId = incmd.get<pid_t>("request.parameters.clientProcessId");
    auto poolId = incmd.get<uint64_t>("request.parameters.poolId");
    cuGroupProp.udfCuGroupName = udfCuGroupName;
    cuGroupProp.clientId = clientId;
    cuGroupProp.clientProcessId = clientProcessId;
    cuGroupProp.poolId = poolId;

    m_system->enterLock();
    do {
        tmpCuGroupRes = (cuGroupResource*)malloc(sizeof(cuGroupResource));
        memset(tmpCuGroupRes, 0, sizeof(cuGroupResource));
        ret = m_system->resAllocCuGroup(&cuGroupProp, tmpCuGroupRes);
        if (ret == XRM_SUCCESS) {
            cuGroupRes.push_back(tmpCuGroupRes);
            availableGroupNum++;
        } else {
            free(tmpCuGroupRes);
            break;
        }
    } while (ret == XRM_SUCCESS);
    if (availableGroupNum > 0) {
        /* get the available cu group number */
        for (i = 0; i < availableGroupNum; i++) {
            m_system->resReleaseCuGroup(cuGroupRes[i]);
            free(cuGroupRes[i]);
        }
        outrsp.put("response.status.value", XRM_SUCCESS);
        outrsp.put("response.data.availableGroupNum", availableGroupNum);
    } else {
        /*
         * There is no available resource or input is invalid.
         * Available cu group num is 0.
         */
        if (ret == XRM_ERROR_INVALID) {
            /* The input is invalid */
            outrsp.put("response.status.value", XRM_ERROR_INVALID);
            outrsp.put("response.data.failed", "failed to check available cu group number");
        } else {
            /* there is no available resource */
            outrsp.put("response.status.value", XRM_SUCCESS);
            outrsp.put("response.data.availableGroupNum", availableGroupNum);
        }
    }
    m_system->exitLock();
}

void xrm::checkCuPoolAvailableNumCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    cuPoolProperty cuPoolProp;
    cuListProperty* cuListProp = NULL;
    std::vector<uint64_t> cuPoolId;
    std::string errmsg;
    int32_t i;
    uint64_t poolId;
    int32_t availablePoolNum = 0;

    memset(&cuPoolProp, 0, sizeof(cuPoolProperty));
    cuListProp = &(cuPoolProp.cuListProp);
    auto cuListNum = incmd.get<int32_t>("request.parameters.cuListNum");
    cuPoolProp.cuListNum = cuListNum;
    auto cuNum = incmd.get<int32_t>("request.parameters.cuNum");
    cuListProp->cuNum = cuNum;
    auto clientId = incmd.get<uint64_t>("request.parameters.clientId");
    auto clientProcessId = incmd.get<pid_t>("request.parameters.clientProcessId");
    auto sameDevice = incmd.get<int32_t>("request.parameters.sameDevice");
    if (sameDevice == 0)
        cuListProp->sameDevice = false;
    else
        cuListProp->sameDevice = true;
    for (i = 0; i < cuListProp->cuNum; i++) {
        auto kernelName = incmd.get<std::string>("request.parameters.kernelName" + std::to_string(i));
        auto kernelAlias = incmd.get<std::string>("request.parameters.kernelAlias" + std::to_string(i));
        auto devExcl = incmd.get<int32_t>("request.parameters.devExcl" + std::to_string(i));
        auto requestLoadUnified = incmd.get<int32_t>("request.parameters.requestLoadUnified" + std::to_string(i));
        auto requestLoadOriginal = incmd.get<int32_t>("request.parameters.requestLoadOriginal" + std::to_string(i));
        strncpy(cuListProp->cuProps[i].kernelName, kernelName.c_str(), XRM_MAX_NAME_LEN - 1);
        strncpy(cuListProp->cuProps[i].kernelAlias, kernelAlias.c_str(), XRM_MAX_NAME_LEN - 1);
        strcpy(cuListProp->cuProps[i].cuName, "");
        if (devExcl == 0)
            cuListProp->cuProps[i].devExcl = false;
        else
            cuListProp->cuProps[i].devExcl = true;
        cuListProp->cuProps[i].requestLoadUnified = requestLoadUnified;
        cuListProp->cuProps[i].requestLoadOriginal = requestLoadOriginal;
        cuListProp->cuProps[i].clientId = clientId;
        cuListProp->cuProps[i].clientProcessId = clientProcessId;
        cuListProp->cuProps[i].poolId = 0;
    }
    auto uuidStr = incmd.get<std::string>("request.parameters.xclbinUuidStr");
    m_system->hexstrToBin(uuidStr, 2 * sizeof(uuid_t), (unsigned char*)&cuPoolProp.xclbinUuid);
    auto xclbinNum = incmd.get<int32_t>("request.parameters.xclbinNum");
    cuPoolProp.xclbinNum = xclbinNum;

    m_system->enterLock();
    do {
        poolId = m_system->resReserveCuPool(&cuPoolProp);
        if (poolId != 0) {
            cuPoolId.push_back(poolId);
            availablePoolNum++;
        } else {
            break;
        }
    } while (poolId != 0);
    if (availablePoolNum > 0) {
        /* get the available cu pool id */
        for (i = 0; i < availablePoolNum; i++) m_system->resRelinquishCuPool(cuPoolId[i]);
        outrsp.put("response.status.value", XRM_SUCCESS);
        outrsp.put("response.data.availablePoolNum", availablePoolNum);
    } else {
        /*
         * Available cu pool num is 0.
         */
        outrsp.put("response.status.value", XRM_SUCCESS);
        outrsp.put("response.data.availablePoolNum", 0);
    }
    m_system->exitLock();
}

void xrm::cuPoolReserveCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    cuPoolProperty cuPoolProp;
    cuListProperty* cuListProp = NULL;
    std::string errmsg;
    int32_t i;

    memset(&cuPoolProp, 0, sizeof(cuPoolProperty));
    cuListProp = &(cuPoolProp.cuListProp);
    auto cuListNum = incmd.get<int32_t>("request.parameters.cuListNum");
    cuPoolProp.cuListNum = cuListNum;
    auto cuNum = incmd.get<int32_t>("request.parameters.cuList.cuNum");
    cuListProp->cuNum = cuNum;
    auto sameDevice = incmd.get<int32_t>("request.parameters.cuList.sameDevice");
    auto clientId = incmd.get<uint64_t>("request.parameters.clientId");
    auto clientProcessId = incmd.get<pid_t>("request.parameters.clientProcessId");
    if (sameDevice == 0)
        cuListProp->sameDevice = false;
    else
        cuListProp->sameDevice = true;
    for (i = 0; i < cuListProp->cuNum; i++) {
        auto kernelName = incmd.get<std::string>("request.parameters.cuList.kernelName" + std::to_string(i));
        auto kernelAlias = incmd.get<std::string>("request.parameters.cuList.kernelAlias" + std::to_string(i));
        auto devExcl = incmd.get<int32_t>("request.parameters.cuList.devExcl" + std::to_string(i));
        auto requestLoadUnified =
            incmd.get<int32_t>("request.parameters.cuList.requestLoadUnified" + std::to_string(i));
        auto requestLoadOriginal =
            incmd.get<int32_t>("request.parameters.cuList.requestLoadOriginal" + std::to_string(i));
        strncpy(cuListProp->cuProps[i].kernelName, kernelName.c_str(), XRM_MAX_NAME_LEN - 1);
        strncpy(cuListProp->cuProps[i].kernelAlias, kernelAlias.c_str(), XRM_MAX_NAME_LEN - 1);
        strcpy(cuListProp->cuProps[i].cuName, "");
        if (devExcl == 0)
            cuListProp->cuProps[i].devExcl = false;
        else
            cuListProp->cuProps[i].devExcl = true;
        cuListProp->cuProps[i].requestLoadUnified = requestLoadUnified;
        cuListProp->cuProps[i].requestLoadOriginal = requestLoadOriginal;
        cuListProp->cuProps[i].clientId = clientId;
        cuListProp->cuProps[i].clientProcessId = clientProcessId;
        cuListProp->cuProps[i].poolId = 0;
    }
    auto uuidStr = incmd.get<std::string>("request.parameters.xclbinUuidStr");
    m_system->hexstrToBin(uuidStr, 2 * sizeof(uuid_t), (unsigned char*)&cuPoolProp.xclbinUuid);
    auto xclbinNum = incmd.get<int32_t>("request.parameters.xclbinNum");
    cuPoolProp.xclbinNum = xclbinNum;
    cuPoolProp.clientId = clientId;
    cuPoolProp.clientProcessId = clientProcessId;

    m_system->enterLock();
    uint64_t poolId = m_system->resReserveCuPool(&cuPoolProp);
    m_system->exitLock();
    if (poolId > 0) {
        outrsp.put("response.status.value", XRM_SUCCESS);
        outrsp.put("response.data.poolId", poolId);
    } else {
        outrsp.put("response.status.value", XRM_ERROR);
    }
}

void xrm::cuPoolRelinquishCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    std::string errmsg;

    auto poolId = incmd.get<uint64_t>("request.parameters.poolId");
    m_system->enterLock();
    int32_t ret = m_system->resRelinquishCuPool(poolId);
    m_system->exitLock();
    outrsp.put("response.status.value", ret);
}

void xrm::reservationQueryCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    cuPoolResource cuPoolRes;
    std::string errmsg;
    int32_t i;

    auto poolId = incmd.get<uint64_t>("request.parameters.poolId");

    memset(&cuPoolRes, 0, sizeof(cuPoolResource));
    m_system->enterLock();
    int32_t ret = m_system->resReservationQuery(poolId, &cuPoolRes);
    m_system->exitLock();
    outrsp.put("response.status.value", ret);
    if (ret == XRM_SUCCESS) {
        if (cuPoolRes.cuNum > 48)
            outrsp.put("response.data.cuNum", 48);
        else
            outrsp.put("response.data.cuNum", cuPoolRes.cuNum);
        //        for (i = 0; i < cuPoolRes.cuNum; i++) {
        for (i = 0; (i < cuPoolRes.cuNum) && (i < 48); i++) {
            outrsp.put("response.data.xclbinFileName" + std::to_string(i), cuPoolRes.cuResources[i].xclbinFileName);
            outrsp.put("response.data.uuidStr" + std::to_string(i), cuPoolRes.cuResources[i].uuidStr);
            outrsp.put("response.data.kernelPluginFileName" + std::to_string(i),
                       cuPoolRes.cuResources[i].kernelPluginFileName);
            outrsp.put("response.data.kernelName" + std::to_string(i), cuPoolRes.cuResources[i].kernelName);
            outrsp.put("response.data.instanceName" + std::to_string(i), cuPoolRes.cuResources[i].instanceName);
            outrsp.put("response.data.cuName" + std::to_string(i), cuPoolRes.cuResources[i].cuName);
            outrsp.put("response.data.kernelAlias" + std::to_string(i), cuPoolRes.cuResources[i].kernelAlias);
            outrsp.put("response.data.deviceId" + std::to_string(i), cuPoolRes.cuResources[i].deviceId);
            outrsp.put("response.data.cuId" + std::to_string(i), cuPoolRes.cuResources[i].cuId);
            outrsp.put("response.data.cuType" + std::to_string(i), (int32_t)cuPoolRes.cuResources[i].cuType);
            outrsp.put("response.data.baseAddr" + std::to_string(i), cuPoolRes.cuResources[i].baseAddr);
            outrsp.put("response.data.membankId" + std::to_string(i), cuPoolRes.cuResources[i].membankId);
            outrsp.put("response.data.membankType" + std::to_string(i), cuPoolRes.cuResources[i].membankType);
            outrsp.put("response.data.membankSize" + std::to_string(i), cuPoolRes.cuResources[i].membankSize);
            outrsp.put("response.data.membankBaseAddr" + std::to_string(i), cuPoolRes.cuResources[i].membankBaseAddr);
            outrsp.put("response.data.poolId" + std::to_string(i), cuPoolRes.cuResources[i].poolId);
        }
    }
}

void xrm::cuAllocWithLoadCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    cuProperty cuProp;
    cuResource cuRes;
    std::string errmsg;

    auto kernelName = incmd.get<std::string>("request.parameters.kernelName");
    auto kernelAlias = incmd.get<std::string>("request.parameters.kernelAlias");
    auto devExcl = incmd.get<int32_t>("request.parameters.devExcl");
    auto requestLoadUnified = incmd.get<int32_t>("request.parameters.requestLoadUnified");
    auto requestLoadOriginal = incmd.get<int32_t>("request.parameters.requestLoadOriginal");
    auto clientId = incmd.get<uint64_t>("request.parameters.clientId");
    auto clientProcessId = incmd.get<pid_t>("request.parameters.clientProcessId");
    auto poolId = incmd.get<uint64_t>("request.parameters.poolId");
    auto xclbinFileName = incmd.get<std::string>("request.parameters.xclbinFileName");
    strncpy(cuProp.kernelName, kernelName.c_str(), XRM_MAX_NAME_LEN - 1);
    strncpy(cuProp.kernelAlias, kernelAlias.c_str(), XRM_MAX_NAME_LEN - 1);
    strcpy(cuProp.cuName, "");
    if (devExcl == 0)
        cuProp.devExcl = false;
    else
        cuProp.devExcl = true;
    cuProp.requestLoadUnified = requestLoadUnified;
    cuProp.requestLoadOriginal = requestLoadOriginal;
    cuProp.clientId = clientId;
    cuProp.clientProcessId = clientProcessId;
    cuProp.poolId = poolId;

    bool update_id = true;
    m_system->enterLock();
    int32_t ret = m_system->resAllocCuWithLoad(&cuProp, xclbinFileName, &cuRes, update_id);
    m_system->exitLock();
    outrsp.put("response.status.value", ret);
    if (ret == XRM_SUCCESS) {
        outrsp.put("response.data.xclbinFileName", cuRes.xclbinFileName);
        outrsp.put("response.data.uuidStr", cuRes.uuidStr);
        outrsp.put("response.data.kernelPluginFileName", cuRes.kernelPluginFileName);
        outrsp.put("response.data.kernelName", cuRes.kernelName);
        outrsp.put("response.data.instanceName", cuRes.instanceName);
        outrsp.put("response.data.cuName", cuRes.cuName);
        outrsp.put("response.data.kernelAlias", cuRes.kernelAlias);
        outrsp.put("response.data.deviceId", cuRes.deviceId);
        outrsp.put("response.data.cuId", cuRes.cuId);
        outrsp.put("response.data.channelId", cuRes.channelId);
        outrsp.put("response.data.cuType", (int32_t)cuRes.cuType);
        outrsp.put("response.data.baseAddr", cuRes.baseAddr);
        outrsp.put("response.data.membankId", cuRes.membankId);
        outrsp.put("response.data.membankType", cuRes.membankType);
        outrsp.put("response.data.membankSize", cuRes.membankSize);
        outrsp.put("response.data.membankBaseAddr", cuRes.membankBaseAddr);
        outrsp.put("response.data.allocServiceId", cuRes.allocServiceId);
        outrsp.put("response.data.channelLoadOriginal", cuRes.channelLoadOriginal);
        outrsp.put("response.data.poolId", cuRes.poolId);
    }
}

void xrm::cuAllocLeastUsedWithLoadCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    cuProperty cuProp;
    cuResource cuRes;
    std::string errmsg;

    auto kernelName = incmd.get<std::string>("request.parameters.kernelName");
    auto kernelAlias = incmd.get<std::string>("request.parameters.kernelAlias");
    auto devExcl = incmd.get<int32_t>("request.parameters.devExcl");
    auto requestLoadUnified = incmd.get<int32_t>("request.parameters.requestLoadUnified");
    auto requestLoadOriginal = incmd.get<int32_t>("request.parameters.requestLoadOriginal");
    auto clientId = incmd.get<uint64_t>("request.parameters.clientId");
    auto clientProcessId = incmd.get<pid_t>("request.parameters.clientProcessId");
    auto poolId = incmd.get<uint64_t>("request.parameters.poolId");
    auto xclbinFileName = incmd.get<std::string>("request.parameters.xclbinFileName");
    strncpy(cuProp.kernelName, kernelName.c_str(), XRM_MAX_NAME_LEN - 1);
    strncpy(cuProp.kernelAlias, kernelAlias.c_str(), XRM_MAX_NAME_LEN - 1);
    strcpy(cuProp.cuName, "");
    if (devExcl == 0)
        cuProp.devExcl = false;
    else
        cuProp.devExcl = true;
    cuProp.requestLoadUnified = requestLoadUnified;
    cuProp.requestLoadOriginal = requestLoadOriginal;
    cuProp.clientId = clientId;
    cuProp.clientProcessId = clientProcessId;
    cuProp.poolId = poolId;

    bool update_id = true;
    m_system->enterLock();
    int32_t ret = m_system->resAllocCuLeastUsedWithLoad(&cuProp, xclbinFileName, &cuRes, update_id);
    m_system->exitLock();
    outrsp.put("response.status.value", ret);
    if (ret == XRM_SUCCESS) {
        outrsp.put("response.data.xclbinFileName", cuRes.xclbinFileName);
        outrsp.put("response.data.uuidStr", cuRes.uuidStr);
        outrsp.put("response.data.kernelPluginFileName", cuRes.kernelPluginFileName);
        outrsp.put("response.data.kernelName", cuRes.kernelName);
        outrsp.put("response.data.instanceName", cuRes.instanceName);
        outrsp.put("response.data.cuName", cuRes.cuName);
        outrsp.put("response.data.kernelAlias", cuRes.kernelAlias);
        outrsp.put("response.data.deviceId", cuRes.deviceId);
        outrsp.put("response.data.cuId", cuRes.cuId);
        outrsp.put("response.data.channelId", cuRes.channelId);
        outrsp.put("response.data.cuType", (int32_t)cuRes.cuType);
        outrsp.put("response.data.baseAddr", cuRes.baseAddr);
        outrsp.put("response.data.membankId", cuRes.membankId);
        outrsp.put("response.data.membankType", cuRes.membankType);
        outrsp.put("response.data.membankSize", cuRes.membankSize);
        outrsp.put("response.data.membankBaseAddr", cuRes.membankBaseAddr);
        outrsp.put("response.data.allocServiceId", cuRes.allocServiceId);
        outrsp.put("response.data.channelLoadOriginal", cuRes.channelLoadOriginal);
        outrsp.put("response.data.poolId", cuRes.poolId);
    }
}

void xrm::loadAndAllCuAllocCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    cuListResource cuListRes;
    std::string errmsg;
    int32_t i;

    auto xclbinFileName = incmd.get<std::string>("request.parameters.xclbinFileName");
    auto clientId = incmd.get<uint64_t>("request.parameters.clientId");
    auto clientProcessId = incmd.get<pid_t>("request.parameters.clientProcessId");

    memset(&cuListRes, 0, sizeof(cuListResource));
    m_system->enterLock();
    int32_t ret = m_system->resLoadAndAllocAllCu(xclbinFileName, clientId, clientProcessId, &cuListRes);
    m_system->exitLock();
    outrsp.put("response.status.value", ret);
    if (ret == XRM_SUCCESS) {
        outrsp.put("response.data.cuNum", cuListRes.cuNum);
        for (i = 0; i < cuListRes.cuNum; i++) {
            outrsp.put("response.data.xclbinFileName" + std::to_string(i), cuListRes.cuResources[i].xclbinFileName);
            outrsp.put("response.data.uuidStr" + std::to_string(i), cuListRes.cuResources[i].uuidStr);
            outrsp.put("response.data.kernelPluginFileName" + std::to_string(i),
                       cuListRes.cuResources[i].kernelPluginFileName);
            outrsp.put("response.data.kernelName" + std::to_string(i), cuListRes.cuResources[i].kernelName);
            outrsp.put("response.data.instanceName" + std::to_string(i), cuListRes.cuResources[i].instanceName);
            outrsp.put("response.data.cuName" + std::to_string(i), cuListRes.cuResources[i].cuName);
            outrsp.put("response.data.kernelAlias" + std::to_string(i), cuListRes.cuResources[i].kernelAlias);
            outrsp.put("response.data.deviceId" + std::to_string(i), cuListRes.cuResources[i].deviceId);
            outrsp.put("response.data.cuId" + std::to_string(i), cuListRes.cuResources[i].cuId);
            outrsp.put("response.data.channelId" + std::to_string(i), cuListRes.cuResources[i].channelId);
            outrsp.put("response.data.cuType" + std::to_string(i), (int32_t)cuListRes.cuResources[i].cuType);
            outrsp.put("response.data.baseAddr" + std::to_string(i), cuListRes.cuResources[i].baseAddr);
            outrsp.put("response.data.membankId" + std::to_string(i), cuListRes.cuResources[i].membankId);
            outrsp.put("response.data.membankType" + std::to_string(i), cuListRes.cuResources[i].membankType);
            outrsp.put("response.data.membankSize" + std::to_string(i), cuListRes.cuResources[i].membankSize);
            outrsp.put("response.data.membankBaseAddr" + std::to_string(i), cuListRes.cuResources[i].membankBaseAddr);
            outrsp.put("response.data.allocServiceId" + std::to_string(i), cuListRes.cuResources[i].allocServiceId);
            outrsp.put("response.data.channelLoadOriginal" + std::to_string(i),
                       cuListRes.cuResources[i].channelLoadOriginal);
            outrsp.put("response.data.poolId" + std::to_string(i), cuListRes.cuResources[i].poolId);
        }
    }
}

void xrm::cuAllocV2Command::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    cuPropertyV2 cuProp;
    cuResource cuRes;
    std::string errmsg;

    auto kernelName = incmd.get<std::string>("request.parameters.kernelName");
    auto kernelAlias = incmd.get<std::string>("request.parameters.kernelAlias");
    auto devExcl = incmd.get<int32_t>("request.parameters.devExcl");
    auto deviceInfo = incmd.get<int64_t>("request.parameters.deviceInfo");
    auto memoryInfo = incmd.get<int64_t>("request.parameters.memoryInfo");
    auto policyInfo = incmd.get<int64_t>("request.parameters.policyInfo");
    auto requestLoadUnified = incmd.get<int32_t>("request.parameters.requestLoadUnified");
    auto requestLoadOriginal = incmd.get<int32_t>("request.parameters.requestLoadOriginal");
    auto clientId = incmd.get<uint64_t>("request.parameters.clientId");
    auto clientProcessId = incmd.get<pid_t>("request.parameters.clientProcessId");
    auto poolId = incmd.get<uint64_t>("request.parameters.poolId");
    strncpy(cuProp.kernelName, kernelName.c_str(), XRM_MAX_NAME_LEN - 1);
    strncpy(cuProp.kernelAlias, kernelAlias.c_str(), XRM_MAX_NAME_LEN - 1);
    strcpy(cuProp.cuName, "");
    if (devExcl == 0)
        cuProp.devExcl = false;
    else
        cuProp.devExcl = true;
    cuProp.deviceInfo = deviceInfo;
    cuProp.memoryInfo = memoryInfo;
    cuProp.policyInfo = policyInfo;
    cuProp.requestLoadUnified = requestLoadUnified;
    cuProp.requestLoadOriginal = requestLoadOriginal;
    cuProp.clientId = clientId;
    cuProp.clientProcessId = clientProcessId;
    cuProp.poolId = poolId;

    bool update_id = true;
    m_system->enterLock();
    int32_t ret = m_system->resAllocCuV2(&cuProp, &cuRes, update_id);
    m_system->exitLock();
    outrsp.put("response.status.value", ret);
    if (ret == XRM_SUCCESS) {
        outrsp.put("response.data.xclbinFileName", cuRes.xclbinFileName);
        outrsp.put("response.data.uuidStr", cuRes.uuidStr);
        outrsp.put("response.data.kernelPluginFileName", cuRes.kernelPluginFileName);
        outrsp.put("response.data.kernelName", cuRes.kernelName);
        outrsp.put("response.data.instanceName", cuRes.instanceName);
        outrsp.put("response.data.cuName", cuRes.cuName);
        outrsp.put("response.data.kernelAlias", cuRes.kernelAlias);
        outrsp.put("response.data.deviceId", cuRes.deviceId);
        outrsp.put("response.data.cuId", cuRes.cuId);
        outrsp.put("response.data.channelId", cuRes.channelId);
        outrsp.put("response.data.cuType", (int32_t)cuRes.cuType);
        outrsp.put("response.data.baseAddr", cuRes.baseAddr);
        outrsp.put("response.data.membankId", cuRes.membankId);
        outrsp.put("response.data.membankType", cuRes.membankType);
        outrsp.put("response.data.membankSize", cuRes.membankSize);
        outrsp.put("response.data.membankBaseAddr", cuRes.membankBaseAddr);
        outrsp.put("response.data.allocServiceId", cuRes.allocServiceId);
        outrsp.put("response.data.channelLoadOriginal", cuRes.channelLoadOriginal);
        outrsp.put("response.data.poolId", cuRes.poolId);
    }
}

void xrm::cuListAllocV2Command::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    cuListPropertyV2* cuListProp;
    cuListResourceV2* cuListRes;
    std::string errmsg;
    int32_t i;

    cuListProp = (cuListPropertyV2*)malloc(sizeof(cuListPropertyV2));
    memset(cuListProp, 0, sizeof(cuListPropertyV2));
    auto cuNum = incmd.get<int32_t>("request.parameters.cuNum");
    cuListProp->cuNum = cuNum;
    auto clientId = incmd.get<uint64_t>("request.parameters.clientId");
    auto clientProcessId = incmd.get<pid_t>("request.parameters.clientProcessId");
    for (i = 0; i < cuListProp->cuNum; i++) {
        auto kernelName = incmd.get<std::string>("request.parameters.kernelName" + std::to_string(i));
        auto kernelAlias = incmd.get<std::string>("request.parameters.kernelAlias" + std::to_string(i));
        auto devExcl = incmd.get<int32_t>("request.parameters.devExcl" + std::to_string(i));
        auto deviceInfo = incmd.get<int64_t>("request.parameters.deviceInfo" + std::to_string(i));
        auto memoryInfo = incmd.get<int64_t>("request.parameters.memoryInfo" + std::to_string(i));
        auto policyInfo = incmd.get<int64_t>("request.parameters.policyInfo" + std::to_string(i));
        auto requestLoadUnified = incmd.get<int32_t>("request.parameters.requestLoadUnified" + std::to_string(i));
        auto requestLoadOriginal = incmd.get<int32_t>("request.parameters.requestLoadOriginal" + std::to_string(i));
        auto poolId = incmd.get<uint64_t>("request.parameters.poolId" + std::to_string(i));
        strncpy(cuListProp->cuProps[i].kernelName, kernelName.c_str(), XRM_MAX_NAME_LEN - 1);
        strncpy(cuListProp->cuProps[i].kernelAlias, kernelAlias.c_str(), XRM_MAX_NAME_LEN - 1);
        strcpy(cuListProp->cuProps[i].cuName, "");
        if (devExcl == 0)
            cuListProp->cuProps[i].devExcl = false;
        else
            cuListProp->cuProps[i].devExcl = true;
        cuListProp->cuProps[i].deviceInfo = deviceInfo;
        cuListProp->cuProps[i].memoryInfo = memoryInfo;
        cuListProp->cuProps[i].policyInfo = policyInfo;
        cuListProp->cuProps[i].requestLoadUnified = requestLoadUnified;
        cuListProp->cuProps[i].requestLoadOriginal = requestLoadOriginal;
        cuListProp->cuProps[i].clientId = clientId;
        cuListProp->cuProps[i].clientProcessId = clientProcessId;
        cuListProp->cuProps[i].poolId = poolId;
    }

    cuListRes = (cuListResourceV2*)malloc(sizeof(cuListResourceV2));
    memset(cuListRes, 0, sizeof(cuListResourceV2));
    m_system->enterLock();
    int32_t ret = m_system->resAllocCuListV2(cuListProp, cuListRes);
    m_system->exitLock();
    outrsp.put("response.status.value", ret);
    if (ret == XRM_SUCCESS) {
        outrsp.put("response.data.cuNum", cuListRes->cuNum);
        for (i = 0; i < cuListRes->cuNum; i++) {
            outrsp.put("response.data.xclbinFileName" + std::to_string(i), cuListRes->cuResources[i].xclbinFileName);
            outrsp.put("response.data.uuidStr" + std::to_string(i), cuListRes->cuResources[i].uuidStr);
            outrsp.put("response.data.kernelPluginFileName" + std::to_string(i),
                       cuListRes->cuResources[i].kernelPluginFileName);
            outrsp.put("response.data.kernelName" + std::to_string(i), cuListRes->cuResources[i].kernelName);
            outrsp.put("response.data.instanceName" + std::to_string(i), cuListRes->cuResources[i].instanceName);
            outrsp.put("response.data.cuName" + std::to_string(i), cuListRes->cuResources[i].cuName);
            outrsp.put("response.data.kernelAlias" + std::to_string(i), cuListRes->cuResources[i].kernelAlias);
            outrsp.put("response.data.deviceId" + std::to_string(i), cuListRes->cuResources[i].deviceId);
            outrsp.put("response.data.cuId" + std::to_string(i), cuListRes->cuResources[i].cuId);
            outrsp.put("response.data.channelId" + std::to_string(i), cuListRes->cuResources[i].channelId);
            outrsp.put("response.data.cuType" + std::to_string(i), (int32_t)cuListRes->cuResources[i].cuType);
            outrsp.put("response.data.baseAddr" + std::to_string(i), cuListRes->cuResources[i].baseAddr);
            outrsp.put("response.data.membankId" + std::to_string(i), cuListRes->cuResources[i].membankId);
            outrsp.put("response.data.membankType" + std::to_string(i), cuListRes->cuResources[i].membankType);
            outrsp.put("response.data.membankSize" + std::to_string(i), cuListRes->cuResources[i].membankSize);
            outrsp.put("response.data.membankBaseAddr" + std::to_string(i), cuListRes->cuResources[i].membankBaseAddr);
            outrsp.put("response.data.allocServiceId" + std::to_string(i), cuListRes->cuResources[i].allocServiceId);
            outrsp.put("response.data.channelLoadOriginal" + std::to_string(i),
                       cuListRes->cuResources[i].channelLoadOriginal);
            outrsp.put("response.data.poolId" + std::to_string(i), cuListRes->cuResources[i].poolId);
        }
    }
    free(cuListProp);
    free(cuListRes);
}

void xrm::cuGroupAllocV2Command::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    cuGroupPropertyV2* cuGroupProp;
    cuGroupResourceV2* cuGroupRes;
    std::string errmsg;
    int32_t i;

    cuGroupProp = (cuGroupPropertyV2*)malloc(sizeof(cuGroupPropertyV2));
    auto udfCuGroupName = incmd.get<std::string>("request.parameters.udfCuGroupName");
    auto clientId = incmd.get<uint64_t>("request.parameters.clientId");
    auto clientProcessId = incmd.get<pid_t>("request.parameters.clientProcessId");
    auto poolId = incmd.get<uint64_t>("request.parameters.poolId");
    cuGroupProp->udfCuGroupName = udfCuGroupName;
    cuGroupProp->clientId = clientId;
    cuGroupProp->clientProcessId = clientProcessId;
    cuGroupProp->poolId = poolId;

    cuGroupRes = (cuGroupResourceV2*)malloc(sizeof(cuGroupResourceV2));
    memset(cuGroupRes, 0, sizeof(cuGroupResourceV2));
    m_system->enterLock();
    int32_t ret = m_system->resAllocCuGroupV2(cuGroupProp, cuGroupRes);
    m_system->exitLock();
    outrsp.put("response.status.value", ret);
    if (ret == XRM_SUCCESS) {
        outrsp.put("response.data.cuNum", cuGroupRes->cuNum);
        for (i = 0; i < cuGroupRes->cuNum; i++) {
            outrsp.put("response.data.xclbinFileName" + std::to_string(i), cuGroupRes->cuResources[i].xclbinFileName);
            outrsp.put("response.data.uuidStr" + std::to_string(i), cuGroupRes->cuResources[i].uuidStr);
            outrsp.put("response.data.kernelPluginFileName" + std::to_string(i),
                       cuGroupRes->cuResources[i].kernelPluginFileName);
            outrsp.put("response.data.kernelName" + std::to_string(i), cuGroupRes->cuResources[i].kernelName);
            outrsp.put("response.data.instanceName" + std::to_string(i), cuGroupRes->cuResources[i].instanceName);
            outrsp.put("response.data.cuName" + std::to_string(i), cuGroupRes->cuResources[i].cuName);
            outrsp.put("response.data.kernelAlias" + std::to_string(i), cuGroupRes->cuResources[i].kernelAlias);
            outrsp.put("response.data.deviceId" + std::to_string(i), cuGroupRes->cuResources[i].deviceId);
            outrsp.put("response.data.cuId" + std::to_string(i), cuGroupRes->cuResources[i].cuId);
            outrsp.put("response.data.channelId" + std::to_string(i), cuGroupRes->cuResources[i].channelId);
            outrsp.put("response.data.cuType" + std::to_string(i), (int32_t)cuGroupRes->cuResources[i].cuType);
            outrsp.put("response.data.baseAddr" + std::to_string(i), cuGroupRes->cuResources[i].baseAddr);
            outrsp.put("response.data.membankId" + std::to_string(i), cuGroupRes->cuResources[i].membankId);
            outrsp.put("response.data.membankType" + std::to_string(i), cuGroupRes->cuResources[i].membankType);
            outrsp.put("response.data.membankSize" + std::to_string(i), cuGroupRes->cuResources[i].membankSize);
            outrsp.put("response.data.membankBaseAddr" + std::to_string(i), cuGroupRes->cuResources[i].membankBaseAddr);
            outrsp.put("response.data.allocServiceId" + std::to_string(i), cuGroupRes->cuResources[i].allocServiceId);
            outrsp.put("response.data.channelLoadOriginal" + std::to_string(i),
                       cuGroupRes->cuResources[i].channelLoadOriginal);
            outrsp.put("response.data.poolId" + std::to_string(i), cuGroupRes->cuResources[i].poolId);
        }
    }
    free(cuGroupProp);
    free(cuGroupRes);
}

void xrm::cuReleaseV2Command::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    cuResource cuRes;
    std::string errmsg;

    auto deviceId = incmd.get<int32_t>("request.parameters.deviceId");
    auto cuId = incmd.get<int32_t>("request.parameters.cuId");
    auto channelId = incmd.get<int32_t>("request.parameters.channelId");
    auto cuType = incmd.get<int32_t>("request.parameters.cuType");
    auto allocServiceId = incmd.get<uint64_t>("request.parameters.allocServiceId");
    auto clientId = incmd.get<uint64_t>("request.parameters.clientId");
    auto channelLoadUnified = incmd.get<int32_t>("request.parameters.channelLoadUnified");
    auto channelLoadOriginal = incmd.get<int32_t>("request.parameters.channelLoadOriginal");
    auto poolId = incmd.get<uint64_t>("request.parameters.poolId");
    cuRes.deviceId = deviceId;
    cuRes.cuId = cuId;
    cuRes.channelId = channelId;
    cuRes.cuType = (xrmCuType)cuType;
    cuRes.allocServiceId = allocServiceId;
    cuRes.clientId = clientId;
    cuRes.channelLoadUnified = channelLoadUnified;
    cuRes.channelLoadOriginal = channelLoadOriginal;
    cuRes.poolId = poolId;

    m_system->enterLock();
    int32_t ret = m_system->resReleaseCuV2(&cuRes);
    m_system->exitLock();
    outrsp.put("response.status.value", ret);
}

void xrm::cuListReleaseV2Command::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    cuListResourceV2* cuListRes;
    std::string errmsg;
    int32_t i;

    cuListRes = (cuListResourceV2*)malloc(sizeof(cuListResourceV2));
    memset(cuListRes, 0, sizeof(cuListResourceV2));
    auto cuNum = incmd.get<int32_t>("request.parameters.cuNum");
    auto clientId = incmd.get<uint64_t>("request.parameters.clientId");
    cuListRes->cuNum = cuNum;
    for (i = 0; i < cuListRes->cuNum; i++) {
        auto deviceId = incmd.get<int32_t>("request.parameters.deviceId" + std::to_string(i));
        auto cuId = incmd.get<int32_t>("request.parameters.cuId" + std::to_string(i));
        auto channelId = incmd.get<int32_t>("request.parameters.channelId" + std::to_string(i));
        auto cuType = incmd.get<int32_t>("request.parameters.cuType" + std::to_string(i));
        auto allocServiceId = incmd.get<uint64_t>("request.parameters.allocServiceId" + std::to_string(i));
        auto channelLoadUnified = incmd.get<int32_t>("request.parameters.channelLoadUnified" + std::to_string(i));
        auto channelLoadOriginal = incmd.get<int32_t>("request.parameters.channelLoadOriginal" + std::to_string(i));
        auto poolId = incmd.get<uint64_t>("request.parameters.poolId" + std::to_string(i));
        cuListRes->cuResources[i].deviceId = deviceId;
        cuListRes->cuResources[i].cuId = cuId;
        cuListRes->cuResources[i].channelId = channelId;
        cuListRes->cuResources[i].cuType = (xrmCuType)cuType;
        cuListRes->cuResources[i].allocServiceId = allocServiceId;
        cuListRes->cuResources[i].clientId = clientId;
        cuListRes->cuResources[i].channelLoadUnified = channelLoadUnified;
        cuListRes->cuResources[i].channelLoadOriginal = channelLoadOriginal;
        cuListRes->cuResources[i].poolId = poolId;
    }

    m_system->enterLock();
    int32_t ret = m_system->resReleaseCuListV2(cuListRes);
    m_system->exitLock();
    free(cuListRes);
    outrsp.put("response.status.value", ret);
}

void xrm::cuGroupReleaseV2Command::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    cuGroupResourceV2* cuGroupRes;
    std::string errmsg;
    int32_t i;

    cuGroupRes = (cuGroupResourceV2*)malloc(sizeof(cuGroupResourceV2));
    memset(cuGroupRes, 0, sizeof(cuGroupResourceV2));
    auto cuNum = incmd.get<int32_t>("request.parameters.cuNum");
    cuGroupRes->cuNum = cuNum;
    auto clientId = incmd.get<uint64_t>("request.parameters.clientId");
    for (i = 0; i < cuGroupRes->cuNum; i++) {
        auto deviceId = incmd.get<int32_t>("request.parameters.deviceId" + std::to_string(i));
        auto cuId = incmd.get<int32_t>("request.parameters.cuId" + std::to_string(i));
        auto channelId = incmd.get<int32_t>("request.parameters.channelId" + std::to_string(i));
        auto cuType = incmd.get<int32_t>("request.parameters.cuType" + std::to_string(i));
        auto allocServiceId = incmd.get<uint64_t>("request.parameters.allocServiceId" + std::to_string(i));
        auto channelLoadUnified = incmd.get<int32_t>("request.parameters.channelLoadUnified" + std::to_string(i));
        auto channelLoadOriginal = incmd.get<int32_t>("request.parameters.channelLoadOriginal" + std::to_string(i));
        auto poolId = incmd.get<uint64_t>("request.parameters.poolId" + std::to_string(i));
        cuGroupRes->cuResources[i].deviceId = deviceId;
        cuGroupRes->cuResources[i].cuId = cuId;
        cuGroupRes->cuResources[i].channelId = channelId;
        cuGroupRes->cuResources[i].cuType = (xrmCuType)cuType;
        cuGroupRes->cuResources[i].allocServiceId = allocServiceId;
        cuGroupRes->cuResources[i].clientId = clientId;
        cuGroupRes->cuResources[i].channelLoadUnified = channelLoadUnified;
        cuGroupRes->cuResources[i].channelLoadOriginal = channelLoadOriginal;
        cuGroupRes->cuResources[i].poolId = poolId;
    }

    m_system->enterLock();
    int32_t ret = m_system->resReleaseCuGroupV2(cuGroupRes);
    m_system->exitLock();
    free(cuGroupRes);
    outrsp.put("response.status.value", ret);
}

void xrm::udfCuGroupDeclareV2Command::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    udfCuGroupInformationV2* udfCuGroupInfo;
    std::string errmsg;
    int32_t i, cuListIdx;
    cuPropertyV2* cuProp;
    cuListPropertyV2* cuListProp;

    udfCuGroupInfo = (udfCuGroupInformationV2*)malloc(sizeof(udfCuGroupInformationV2));
    auto udfCuGroupName = incmd.get<std::string>("request.parameters.udfCuGroupName");
    auto optionUdfCuListNum = incmd.get<int32_t>("request.parameters.optionUdfCuListNum");
    udfCuGroupInfo->udfCuGroupName = udfCuGroupName;
    udfCuGroupInfo->optionUdfCuListNum = optionUdfCuListNum;

    for (cuListIdx = 0; cuListIdx < optionUdfCuListNum; cuListIdx++) {
        cuListProp = &udfCuGroupInfo->optionUdfCuListProps[cuListIdx];
        std::string udfCuListStr = "request.parameters.optionUdfCuListProps" + std::to_string(cuListIdx);
        auto cuNum = incmd.get<int32_t>(udfCuListStr + ".cuNum");
        cuListProp->cuNum = cuNum;
        for (i = 0; i < cuNum; i++) {
            cuProp = &cuListProp->cuProps[i];
            auto cuName = incmd.get<std::string>(udfCuListStr + ".cuName" + std::to_string(i));
            strncpy(cuProp->cuName, cuName.c_str(), XRM_MAX_NAME_LEN - 1);
            auto devExcl = incmd.get<int32_t>(udfCuListStr + ".devExcl" + std::to_string(i));
            if (devExcl == 0)
                cuProp->devExcl = false;
            else
                cuProp->devExcl = true;
            auto deviceInfo = incmd.get<int64_t>(udfCuListStr + ".deviceInfo" + std::to_string(i));
            auto memoryInfo = incmd.get<int64_t>(udfCuListStr + ".memoryInfo" + std::to_string(i));
            auto requestLoadUnified = incmd.get<int32_t>(udfCuListStr + ".requestLoadUnified" + std::to_string(i));
            auto requestLoadOriginal = incmd.get<int32_t>(udfCuListStr + ".requestLoadOriginal" + std::to_string(i));
            cuProp->deviceInfo = deviceInfo;
            cuProp->memoryInfo = memoryInfo;
            cuProp->requestLoadUnified = requestLoadUnified;
            cuProp->requestLoadOriginal = requestLoadOriginal;
        }
    }

    m_system->enterLock();
    int32_t ret = m_system->resUdfCuGroupDeclareV2(udfCuGroupInfo);
    m_system->exitLock();
    free(udfCuGroupInfo);
    outrsp.put("response.status.value", ret);
    if (ret != XRM_SUCCESS) {
        outrsp.put("response.data.failed", "failed to declare user defined cu group");
    }
}

void xrm::udfCuGroupUndeclareV2Command::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    udfCuGroupInformationV2 udfCuGroupInfo;
    std::string errmsg;

    auto udfCuGroupName = incmd.get<std::string>("request.parameters.udfCuGroupName");
    udfCuGroupInfo.udfCuGroupName = udfCuGroupName;

    m_system->enterLock();
    int32_t ret = m_system->resUdfCuGroupUndeclareV2(&udfCuGroupInfo);
    m_system->exitLock();
    outrsp.put("response.status.value", ret);
    if (ret != XRM_SUCCESS) {
        outrsp.put("response.data.failed", "failed to undeclare user defined cu group");
    }
}

void xrm::allocationQueryV2Command::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    allocationQueryInfoV2 allocQuery;
    cuListResourceV2* cuListRes;
    std::string errmsg;
    int32_t i;

    memset(&allocQuery, 0, sizeof(allocationQueryInfoV2));
    auto allocServiceId = incmd.get<uint64_t>("request.parameters.allocServiceId");
    auto kernelName = incmd.get<std::string>("request.parameters.kernelName");
    auto kernelAlias = incmd.get<std::string>("request.parameters.kernelAlias");

    cuListRes = (cuListResourceV2*)malloc(sizeof(cuListResourceV2));
    memset(cuListRes, 0, sizeof(cuListResourceV2));
    allocQuery.allocServiceId = allocServiceId;
    strncpy(allocQuery.kernelName, kernelName.c_str(), XRM_MAX_NAME_LEN - 1);
    strncpy(allocQuery.kernelAlias, kernelAlias.c_str(), XRM_MAX_NAME_LEN - 1);
    m_system->enterLock();
    int32_t ret = m_system->resAllocationQueryV2(&allocQuery, cuListRes);
    m_system->exitLock();
    outrsp.put("response.status.value", ret);
    if (ret == XRM_SUCCESS) {
        outrsp.put("response.data.cuNum", cuListRes->cuNum);
        for (i = 0; i < cuListRes->cuNum; i++) {
            outrsp.put("response.data.xclbinFileName" + std::to_string(i), cuListRes->cuResources[i].xclbinFileName);
            outrsp.put("response.data.uuidStr" + std::to_string(i), cuListRes->cuResources[i].uuidStr);
            outrsp.put("response.data.kernelPluginFileName" + std::to_string(i),
                       cuListRes->cuResources[i].kernelPluginFileName);
            outrsp.put("response.data.kernelName" + std::to_string(i), cuListRes->cuResources[i].kernelName);
            outrsp.put("response.data.instanceName" + std::to_string(i), cuListRes->cuResources[i].instanceName);
            outrsp.put("response.data.cuName" + std::to_string(i), cuListRes->cuResources[i].cuName);
            outrsp.put("response.data.kernelAlias" + std::to_string(i), cuListRes->cuResources[i].kernelAlias);
            outrsp.put("response.data.deviceId" + std::to_string(i), cuListRes->cuResources[i].deviceId);
            outrsp.put("response.data.cuId" + std::to_string(i), cuListRes->cuResources[i].cuId);
            outrsp.put("response.data.channelId" + std::to_string(i), cuListRes->cuResources[i].channelId);
            outrsp.put("response.data.cuType" + std::to_string(i), (int32_t)cuListRes->cuResources[i].cuType);
            outrsp.put("response.data.baseAddr" + std::to_string(i), (int32_t)cuListRes->cuResources[i].baseAddr);
            outrsp.put("response.data.membankId" + std::to_string(i), (int32_t)cuListRes->cuResources[i].membankId);
            outrsp.put("response.data.membankType" + std::to_string(i), (int32_t)cuListRes->cuResources[i].membankType);
            outrsp.put("response.data.membankSize" + std::to_string(i), (int32_t)cuListRes->cuResources[i].membankSize);
            outrsp.put("response.data.membankBaseAddr" + std::to_string(i),
                       (int32_t)cuListRes->cuResources[i].membankBaseAddr);
            outrsp.put("response.data.allocServiceId" + std::to_string(i), cuListRes->cuResources[i].allocServiceId);
            outrsp.put("response.data.channelLoadOriginal" + std::to_string(i),
                       cuListRes->cuResources[i].channelLoadOriginal);
            outrsp.put("response.data.poolId" + std::to_string(i), cuListRes->cuResources[i].poolId);
        }
    }
    free(cuListRes);
}

void xrm::checkCuAvailableNumV2Command::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    cuPropertyV2 cuProp;
    std::vector<cuResource*> cuRes;
    std::string errmsg;
    cuResource* tmpCuRes;
    int32_t i, ret;
    int32_t availableCuNum = 0;

    auto kernelName = incmd.get<std::string>("request.parameters.kernelName");
    auto kernelAlias = incmd.get<std::string>("request.parameters.kernelAlias");
    auto devExcl = incmd.get<int32_t>("request.parameters.devExcl");
    auto deviceInfo = incmd.get<int64_t>("request.parameters.deviceInfo");
    auto memoryInfo = incmd.get<int64_t>("request.parameters.memoryInfo");
    auto policyInfo = incmd.get<int64_t>("request.parameters.policyInfo");
    auto requestLoadUnified = incmd.get<int32_t>("request.parameters.requestLoadUnified");
    auto requestLoadOriginal = incmd.get<int32_t>("request.parameters.requestLoadOriginal");
    auto clientId = incmd.get<uint64_t>("request.parameters.clientId");
    auto clientProcessId = incmd.get<pid_t>("request.parameters.clientProcessId");
    auto poolId = incmd.get<uint64_t>("request.parameters.poolId");
    strncpy(cuProp.kernelName, kernelName.c_str(), XRM_MAX_NAME_LEN - 1);
    strncpy(cuProp.kernelAlias, kernelAlias.c_str(), XRM_MAX_NAME_LEN - 1);
    strcpy(cuProp.cuName, "");
    if (devExcl == 0)
        cuProp.devExcl = false;
    else
        cuProp.devExcl = true;
    cuProp.deviceInfo = deviceInfo;
    cuProp.memoryInfo = memoryInfo;
    cuProp.policyInfo = policyInfo;
    cuProp.requestLoadUnified = requestLoadUnified;
    cuProp.requestLoadOriginal = requestLoadOriginal;
    cuProp.clientId = clientId;
    cuProp.clientProcessId = clientProcessId;
    cuProp.poolId = poolId;

    bool update_id = true;
    m_system->enterLock();
    do {
        tmpCuRes = (cuResource*)malloc(sizeof(cuResource));
        memset(tmpCuRes, 0, sizeof(cuResource));
        ret = m_system->resAllocCuV2(&cuProp, tmpCuRes, update_id);
        if (ret == XRM_SUCCESS) {
            cuRes.push_back(tmpCuRes);
            availableCuNum++;
        } else {
            free(tmpCuRes);
            break;
        }
    } while (ret == XRM_SUCCESS);
    if (availableCuNum > 0) {
        /* get the available cu number */
        for (i = 0; i < availableCuNum; i++) {
            m_system->resReleaseCuV2(cuRes[i]);
            free(cuRes[i]);
        }
        outrsp.put("response.status.value", XRM_SUCCESS);
        outrsp.put("response.data.availableCuNum", availableCuNum);
    } else {
        /*
         * There is no available resource or input is invalid.
         * Available cu list num is 0.
         */
        if (ret == XRM_ERROR_INVALID) {
            /* The input is invalid */
            outrsp.put("response.status.value", XRM_ERROR_INVALID);
            outrsp.put("response.data.failed", "failed to check available cu number");
        } else {
            /* there is no available resource */
            outrsp.put("response.status.value", XRM_SUCCESS);
            outrsp.put("response.data.availableCuNum", availableCuNum);
        }
    }
    m_system->exitLock();
}

void xrm::checkCuListAvailableNumV2Command::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    cuListPropertyV2* cuListProp;
    std::vector<cuListResourceV2*> cuListRes;
    cuListResourceV2* tmpCuListRes;
    std::string errmsg;
    int32_t i, ret;
    int32_t availableListNum = 0;

    cuListProp = (cuListPropertyV2*)malloc(sizeof(cuListPropertyV2));
    memset(cuListProp, 0, sizeof(cuListPropertyV2));
    auto cuNum = incmd.get<int32_t>("request.parameters.cuNum");
    cuListProp->cuNum = cuNum;
    auto clientId = incmd.get<uint64_t>("request.parameters.clientId");
    auto clientProcessId = incmd.get<pid_t>("request.parameters.clientProcessId");
    for (i = 0; i < cuListProp->cuNum; i++) {
        auto kernelName = incmd.get<std::string>("request.parameters.kernelName" + std::to_string(i));
        auto kernelAlias = incmd.get<std::string>("request.parameters.kernelAlias" + std::to_string(i));
        auto devExcl = incmd.get<int32_t>("request.parameters.devExcl" + std::to_string(i));
        auto deviceInfo = incmd.get<int64_t>("request.parameters.deviceInfo" + std::to_string(i));
        auto memoryInfo = incmd.get<int64_t>("request.parameters.memoryInfo" + std::to_string(i));
        auto policyInfo = incmd.get<int64_t>("request.parameters.policyInfo" + std::to_string(i));
        auto requestLoadUnified = incmd.get<int32_t>("request.parameters.requestLoadUnified" + std::to_string(i));
        auto requestLoadOriginal = incmd.get<int32_t>("request.parameters.requestLoadOriginal" + std::to_string(i));
        auto poolId = incmd.get<uint64_t>("request.parameters.poolId" + std::to_string(i));
        strncpy(cuListProp->cuProps[i].kernelName, kernelName.c_str(), XRM_MAX_NAME_LEN - 1);
        strncpy(cuListProp->cuProps[i].kernelAlias, kernelAlias.c_str(), XRM_MAX_NAME_LEN - 1);
        strcpy(cuListProp->cuProps[i].cuName, "");
        if (devExcl == 0)
            cuListProp->cuProps[i].devExcl = false;
        else
            cuListProp->cuProps[i].devExcl = true;
        cuListProp->cuProps[i].deviceInfo = deviceInfo;
        cuListProp->cuProps[i].memoryInfo = memoryInfo;
        cuListProp->cuProps[i].policyInfo = policyInfo;
        cuListProp->cuProps[i].requestLoadUnified = requestLoadUnified;
        cuListProp->cuProps[i].requestLoadOriginal = requestLoadOriginal;
        cuListProp->cuProps[i].clientId = clientId;
        cuListProp->cuProps[i].clientProcessId = clientProcessId;
        cuListProp->cuProps[i].poolId = poolId;
    }

    m_system->enterLock();
    do {
        tmpCuListRes = (cuListResourceV2*)malloc(sizeof(cuListResourceV2));
        memset(tmpCuListRes, 0, sizeof(cuListResourceV2));
        ret = m_system->resAllocCuListV2(cuListProp, tmpCuListRes);
        if (ret == XRM_SUCCESS) {
            cuListRes.push_back(tmpCuListRes);
            availableListNum++;
        } else {
            free(tmpCuListRes);
            break;
        }
    } while (ret == XRM_SUCCESS);
    if (availableListNum > 0) {
        /* get the available cu list number */
        for (i = 0; i < availableListNum; i++) {
            m_system->resReleaseCuListV2(cuListRes[i]);
            free(cuListRes[i]);
        }
        outrsp.put("response.status.value", XRM_SUCCESS);
        outrsp.put("response.data.availableListNum", availableListNum);
    } else {
        /*
         * There is no available resource or input is invalid.
         * Available cu list num is 0.
         */
        if (ret == XRM_ERROR_INVALID) {
            /* The input is invalid */
            outrsp.put("response.status.value", XRM_ERROR_INVALID);
            outrsp.put("response.data.failed", "failed to check available cu list number");
        } else {
            /* there is no available resource */
            outrsp.put("response.status.value", XRM_SUCCESS);
            outrsp.put("response.data.availableListNum", availableListNum);
        }
    }
    m_system->exitLock();
    free(cuListProp);
}

void xrm::checkCuGroupAvailableNumV2Command::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    cuGroupPropertyV2 cuGroupProp;
    std::vector<cuGroupResourceV2*> cuGroupRes;
    cuGroupResourceV2* tmpCuGroupRes;
    std::string errmsg;
    int32_t i, ret;
    int32_t availableGroupNum = 0;

    auto udfCuGroupName = incmd.get<std::string>("request.parameters.udfCuGroupName");
    auto clientId = incmd.get<uint64_t>("request.parameters.clientId");
    auto clientProcessId = incmd.get<pid_t>("request.parameters.clientProcessId");
    auto poolId = incmd.get<uint64_t>("request.parameters.poolId");
    cuGroupProp.udfCuGroupName = udfCuGroupName;
    cuGroupProp.clientId = clientId;
    cuGroupProp.clientProcessId = clientProcessId;
    cuGroupProp.poolId = poolId;

    m_system->enterLock();
    do {
        tmpCuGroupRes = (cuGroupResourceV2*)malloc(sizeof(cuGroupResourceV2));
        memset(tmpCuGroupRes, 0, sizeof(cuGroupResourceV2));
        ret = m_system->resAllocCuGroupV2(&cuGroupProp, tmpCuGroupRes);
        if (ret == XRM_SUCCESS) {
            cuGroupRes.push_back(tmpCuGroupRes);
            availableGroupNum++;
        } else {
            free(tmpCuGroupRes);
            break;
        }
    } while (ret == XRM_SUCCESS);
    if (availableGroupNum > 0) {
        /* get the available cu group number */
        for (i = 0; i < availableGroupNum; i++) {
            m_system->resReleaseCuGroupV2(cuGroupRes[i]);
            free(cuGroupRes[i]);
        }
        outrsp.put("response.status.value", XRM_SUCCESS);
        outrsp.put("response.data.availableGroupNum", availableGroupNum);
    } else {
        /*
         * There is no available resource or input is invalid.
         * Available cu group num is 0.
         */
        if (ret == XRM_ERROR_INVALID) {
            /* The input is invalid */
            outrsp.put("response.status.value", XRM_ERROR_INVALID);
            outrsp.put("response.data.failed", "failed to check available cu group number");
        } else {
            /* there is no available resource */
            outrsp.put("response.status.value", XRM_SUCCESS);
            outrsp.put("response.data.availableGroupNum", availableGroupNum);
        }
    }
    m_system->exitLock();
}

void xrm::checkCuPoolAvailableNumV2Command::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    cuPoolPropertyV2* cuPoolProp;
    cuListPropertyV2* cuListProp = NULL;
    deviceIdListPropertyV2* deviceIdListProp = NULL;
    std::vector<uint64_t> cuPoolId;
    std::string errmsg;
    int32_t i;
    uint64_t poolId;
    int32_t availablePoolNum = 0;
    cuPoolResInforV2* cuPoolResInfor = NULL;

    cuPoolProp = (cuPoolPropertyV2*)malloc(sizeof(cuPoolPropertyV2));
    memset(cuPoolProp, 0, sizeof(cuPoolPropertyV2));
    cuListProp = &(cuPoolProp->cuListProp);
    deviceIdListProp = &(cuPoolProp->deviceIdListProp);
    auto cuListNum = incmd.get<int32_t>("request.parameters.cuListNum");
    cuPoolProp->cuListNum = cuListNum;
    auto cuNum = incmd.get<int32_t>("request.parameters.cuList.cuNum");
    cuListProp->cuNum = cuNum;
    auto deviceNum = incmd.get<int32_t>("request.parameters.deviceIdList.deviceNum");
    deviceIdListProp->deviceNum = deviceNum;
    auto clientId = incmd.get<uint64_t>("request.parameters.clientId");
    auto clientProcessId = incmd.get<pid_t>("request.parameters.clientProcessId");
    for (i = 0; i < cuListProp->cuNum; i++) {
        auto kernelName = incmd.get<std::string>("request.parameters.kernelName" + std::to_string(i));
        auto kernelAlias = incmd.get<std::string>("request.parameters.kernelAlias" + std::to_string(i));
        auto devExcl = incmd.get<int32_t>("request.parameters.devExcl" + std::to_string(i));
        auto deviceInfo = incmd.get<int64_t>("request.parameters.deviceInfo" + std::to_string(i));
        auto memoryInfo = incmd.get<int64_t>("request.parameters.memoryInfo" + std::to_string(i));
        auto policyInfo = incmd.get<int64_t>("request.parameters.policyInfo" + std::to_string(i));
        auto requestLoadUnified = incmd.get<int32_t>("request.parameters.requestLoadUnified" + std::to_string(i));
        auto requestLoadOriginal = incmd.get<int32_t>("request.parameters.requestLoadOriginal" + std::to_string(i));
        strncpy(cuListProp->cuProps[i].kernelName, kernelName.c_str(), XRM_MAX_NAME_LEN - 1);
        strncpy(cuListProp->cuProps[i].kernelAlias, kernelAlias.c_str(), XRM_MAX_NAME_LEN - 1);
        strcpy(cuListProp->cuProps[i].cuName, "");
        if (devExcl == 0)
            cuListProp->cuProps[i].devExcl = false;
        else
            cuListProp->cuProps[i].devExcl = true;
        cuListProp->cuProps[i].deviceInfo = deviceInfo;
        cuListProp->cuProps[i].memoryInfo = memoryInfo;
        cuListProp->cuProps[i].policyInfo = policyInfo;
        cuListProp->cuProps[i].requestLoadUnified = requestLoadUnified;
        cuListProp->cuProps[i].requestLoadOriginal = requestLoadOriginal;
        cuListProp->cuProps[i].clientId = clientId;
        cuListProp->cuProps[i].clientProcessId = clientProcessId;
        cuListProp->cuProps[i].poolId = 0;
    }
    for (i = 0; i < deviceIdListProp->deviceNum; i++) {
        auto deviceId = incmd.get<uint64_t>("request.parameters.deviceIdList.deviceIds" + std::to_string(i));
        deviceIdListProp->deviceIds[i] = deviceId;
    }
    auto uuidStr = incmd.get<std::string>("request.parameters.xclbinUuidStr");
    m_system->hexstrToBin(uuidStr, 2 * sizeof(uuid_t), (unsigned char*)&cuPoolProp->xclbinUuid);
    auto xclbinNum = incmd.get<int32_t>("request.parameters.xclbinNum");
    cuPoolProp->xclbinNum = xclbinNum;

    cuPoolResInfor = (cuPoolResInforV2*)malloc(sizeof(cuPoolResInforV2));
    memset(cuPoolResInfor, 0, sizeof(cuPoolResInforV2));
    m_system->enterLock();
    do {
        poolId = m_system->resReserveCuPoolV2(cuPoolProp, cuPoolResInfor);
        if (poolId != 0) {
            cuPoolId.push_back(poolId);
            availablePoolNum++;
        } else {
            break;
        }
    } while (poolId != 0);
    if (availablePoolNum > 0) {
        /* get the available cu pool id */
        for (i = 0; i < availablePoolNum; i++) m_system->resRelinquishCuPoolV2(cuPoolId[i]);
        outrsp.put("response.status.value", XRM_SUCCESS);
        outrsp.put("response.data.availablePoolNum", availablePoolNum);
    } else {
        /*
         * Available cu pool num is 0.
         */
        outrsp.put("response.status.value", XRM_SUCCESS);
        outrsp.put("response.data.availablePoolNum", 0);
    }
    m_system->exitLock();
    free(cuPoolProp);
    free(cuPoolResInfor);
}

void xrm::cuPoolReserveV2Command::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    cuPoolPropertyV2* cuPoolProp;
    cuListPropertyV2* cuListProp = NULL;
    deviceIdListPropertyV2* deviceIdListProp = NULL;
    cuPoolResInforV2* cuPoolResInfor = NULL;
    cuListResInforV2* cuListResInfor = NULL;
    cuResInforV2* cuResInfor = NULL;

    std::string errmsg;
    int32_t i, j;

    cuPoolProp = (cuPoolPropertyV2*)malloc(sizeof(cuPoolPropertyV2));
    memset(cuPoolProp, 0, sizeof(cuPoolPropertyV2));
    cuListProp = &(cuPoolProp->cuListProp);
    deviceIdListProp = &(cuPoolProp->deviceIdListProp);
    auto cuListNum = incmd.get<int32_t>("request.parameters.cuListNum");
    cuPoolProp->cuListNum = cuListNum;
    auto cuNum = incmd.get<int32_t>("request.parameters.cuList.cuNum");
    cuListProp->cuNum = cuNum;
    auto deviceNum = incmd.get<int32_t>("request.parameters.deviceIdList.deviceNum");
    deviceIdListProp->deviceNum = deviceNum;
    auto clientId = incmd.get<uint64_t>("request.parameters.clientId");
    auto clientProcessId = incmd.get<pid_t>("request.parameters.clientProcessId");
    for (i = 0; i < cuListProp->cuNum; i++) {
        auto kernelName = incmd.get<std::string>("request.parameters.cuList.kernelName" + std::to_string(i));
        auto kernelAlias = incmd.get<std::string>("request.parameters.cuList.kernelAlias" + std::to_string(i));
        auto devExcl = incmd.get<int32_t>("request.parameters.cuList.devExcl" + std::to_string(i));
        auto deviceInfo = incmd.get<int64_t>("request.parameters.cuList.deviceInfo" + std::to_string(i));
        auto memoryInfo = incmd.get<int64_t>("request.parameters.cuList.memoryInfo" + std::to_string(i));
        auto policyInfo = incmd.get<int64_t>("request.parameters.cuList.policyInfo" + std::to_string(i));
        auto requestLoadUnified =
            incmd.get<int32_t>("request.parameters.cuList.requestLoadUnified" + std::to_string(i));
        auto requestLoadOriginal =
            incmd.get<int32_t>("request.parameters.cuList.requestLoadOriginal" + std::to_string(i));
        strncpy(cuListProp->cuProps[i].kernelName, kernelName.c_str(), XRM_MAX_NAME_LEN - 1);
        strncpy(cuListProp->cuProps[i].kernelAlias, kernelAlias.c_str(), XRM_MAX_NAME_LEN - 1);
        strcpy(cuListProp->cuProps[i].cuName, "");
        if (devExcl == 0)
            cuListProp->cuProps[i].devExcl = false;
        else
            cuListProp->cuProps[i].devExcl = true;
        cuListProp->cuProps[i].deviceInfo = deviceInfo;
        cuListProp->cuProps[i].memoryInfo = memoryInfo;
        cuListProp->cuProps[i].policyInfo = policyInfo;
        cuListProp->cuProps[i].requestLoadUnified = requestLoadUnified;
        cuListProp->cuProps[i].requestLoadOriginal = requestLoadOriginal;
        cuListProp->cuProps[i].clientId = clientId;
        cuListProp->cuProps[i].clientProcessId = clientProcessId;
        cuListProp->cuProps[i].poolId = 0;
    }
    for (i = 0; i < deviceIdListProp->deviceNum; i++) {
        auto deviceId = incmd.get<uint64_t>("request.parameters.deviceIdList.deviceIds" + std::to_string(i));
        deviceIdListProp->deviceIds[i] = deviceId;
    }
    auto uuidStr = incmd.get<std::string>("request.parameters.xclbinUuidStr");
    m_system->hexstrToBin(uuidStr, 2 * sizeof(uuid_t), (unsigned char*)&cuPoolProp->xclbinUuid);
    auto xclbinNum = incmd.get<int32_t>("request.parameters.xclbinNum");
    cuPoolProp->xclbinNum = xclbinNum;
    cuPoolProp->clientId = clientId;
    cuPoolProp->clientProcessId = clientProcessId;

    cuPoolResInfor = (cuPoolResInforV2*)malloc(sizeof(cuPoolResInforV2));
    memset(cuPoolResInfor, 0, sizeof(cuPoolResInforV2));
    m_system->enterLock();
    uint64_t poolId = m_system->resReserveCuPoolV2(cuPoolProp, cuPoolResInfor);
    m_system->exitLock();
    if (poolId > 0) {
        outrsp.put("response.status.value", XRM_SUCCESS);
        outrsp.put("response.data.poolId", poolId);

        outrsp.put("response.data.cuListNum", cuPoolResInfor->cuListNum);
        for (i = 0; i < cuPoolResInfor->cuListNum; i++) {
            cuListResInfor = &(cuPoolResInfor->cuListResInfor[i]);
            outrsp.put("response.data.cuList.cuNum" + std::to_string(i), cuListResInfor->cuNum);

            for (j = 0; j < cuListResInfor->cuNum; j++) {
                cuResInfor = &(cuListResInfor->cuResInfor[j]);
                outrsp.put("response.data.cuList.deviceId" + std::to_string(i) + std::to_string(j),
                           cuResInfor->deviceId);
            }
        }

        outrsp.put("response.data.xclbinNum", cuPoolResInfor->xclbinNum);
        for (i = 0; i < cuPoolResInfor->xclbinNum; i++) {
            cuResInfor = &(cuPoolResInfor->xclbinResInfor[i]);
            outrsp.put("response.data.xclbin.deviceId" + std::to_string(i), cuResInfor->deviceId);
        }
    } else {
        outrsp.put("response.status.value", XRM_ERROR);
    }
    free(cuPoolProp);
    free(cuPoolResInfor);
}

void xrm::cuPoolRelinquishV2Command::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    std::string errmsg;

    auto poolId = incmd.get<uint64_t>("request.parameters.poolId");
    m_system->enterLock();
    int32_t ret = m_system->resRelinquishCuPoolV2(poolId);
    m_system->exitLock();
    outrsp.put("response.status.value", ret);
}

void xrm::reservationQueryV2Command::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    reservationQueryInfoV2* reserveQueryInfo;
    cuPoolResourceV2* cuPoolRes;
    std::string errmsg;
    int32_t i;

    reserveQueryInfo = (reservationQueryInfoV2*)malloc(sizeof(reservationQueryInfoV2));
    memset(reserveQueryInfo, 0, sizeof(reservationQueryInfoV2));
    auto poolId = incmd.get<uint64_t>("request.parameters.poolId");
    auto kernelName = incmd.get<std::string>("request.parameters.kernelName");
    auto kernelAlias = incmd.get<std::string>("request.parameters.kernelAlias");
    reserveQueryInfo->poolId = poolId;
    strncpy(reserveQueryInfo->kernelName, kernelName.c_str(), XRM_MAX_NAME_LEN - 1);
    strncpy(reserveQueryInfo->kernelAlias, kernelAlias.c_str(), XRM_MAX_NAME_LEN - 1);

    cuPoolRes = (cuPoolResourceV2*)malloc(sizeof(cuPoolResourceV2));
    memset(cuPoolRes, 0, sizeof(cuPoolResourceV2));
    m_system->enterLock();
    int32_t ret = m_system->resReservationQueryV2(reserveQueryInfo, cuPoolRes);
    m_system->exitLock();
    outrsp.put("response.status.value", ret);
    if (ret == XRM_SUCCESS) {
        if (cuPoolRes->cuNum > 48)
            outrsp.put("response.data.cuNum", 48);
        else
            outrsp.put("response.data.cuNum", cuPoolRes->cuNum);
        //        for (i = 0; i < cuPoolRes->cuNum; i++) {
        for (i = 0; (i < cuPoolRes->cuNum) && (i < 48); i++) {
            outrsp.put("response.data.xclbinFileName" + std::to_string(i), cuPoolRes->cuResources[i].xclbinFileName);
            outrsp.put("response.data.uuidStr" + std::to_string(i), cuPoolRes->cuResources[i].uuidStr);
            outrsp.put("response.data.kernelPluginFileName" + std::to_string(i),
                       cuPoolRes->cuResources[i].kernelPluginFileName);
            outrsp.put("response.data.kernelName" + std::to_string(i), cuPoolRes->cuResources[i].kernelName);
            outrsp.put("response.data.instanceName" + std::to_string(i), cuPoolRes->cuResources[i].instanceName);
            outrsp.put("response.data.cuName" + std::to_string(i), cuPoolRes->cuResources[i].cuName);
            outrsp.put("response.data.kernelAlias" + std::to_string(i), cuPoolRes->cuResources[i].kernelAlias);
            outrsp.put("response.data.deviceId" + std::to_string(i), cuPoolRes->cuResources[i].deviceId);
            outrsp.put("response.data.cuId" + std::to_string(i), cuPoolRes->cuResources[i].cuId);
            outrsp.put("response.data.cuType" + std::to_string(i), (int32_t)cuPoolRes->cuResources[i].cuType);
            outrsp.put("response.data.baseAddr" + std::to_string(i), cuPoolRes->cuResources[i].baseAddr);
            outrsp.put("response.data.membankId" + std::to_string(i), cuPoolRes->cuResources[i].membankId);
            outrsp.put("response.data.membankType" + std::to_string(i), cuPoolRes->cuResources[i].membankType);
            outrsp.put("response.data.membankSize" + std::to_string(i), cuPoolRes->cuResources[i].membankSize);
            outrsp.put("response.data.membankBaseAddr" + std::to_string(i), cuPoolRes->cuResources[i].membankBaseAddr);
            outrsp.put("response.data.poolId" + std::to_string(i), cuPoolRes->cuResources[i].poolId);
        }
    }
    free(reserveQueryInfo);
    free(cuPoolRes);
}
