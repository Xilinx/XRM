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

#include "xrm_command_resource.hpp"

void xrm::createContextCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    auto context = incmd.get<std::string>("request.parameters.context");
    int32_t logLevel = m_system->getLogLevel();
    uint64_t clientId;
    if (m_system->incNumConcurrentClient())
        clientId = m_system->getNewClientId();
    else
        clientId = 0; // reach the limit of concurrent client, fail to create new context
    outrsp.put("response.status.value", logLevel);
    outrsp.put("response.data.clientId", clientId);
}

void xrm::echoContextCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    auto context = incmd.get<std::string>("request.parameters.context");
    outrsp.put("response.status.value", XRM_SUCCESS);
}

void xrm::destroyContextCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    auto context = incmd.get<std::string>("request.parameters.context");
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
    auto requestLoad = incmd.get<int32_t>("request.parameters.requestLoad");
    auto clientId = incmd.get<uint64_t>("request.parameters.clientId");
    auto poolId = incmd.get<uint64_t>("request.parameters.poolId");
    strncpy(cuProp.kernelName, kernelName.c_str(), XRM_MAX_NAME_LEN - 1);
    strncpy(cuProp.kernelAlias, kernelAlias.c_str(), XRM_MAX_NAME_LEN - 1);
    strcpy(cuProp.cuName, "");
    if (devExcl == 0)
        cuProp.devExcl = false;
    else
        cuProp.devExcl = true;
    cuProp.requestLoad = requestLoad;
    cuProp.clientId = clientId;
    cuProp.poolId = poolId;

    bool update_id = true;
    int32_t ret = m_system->resAllocCu(&cuProp, &cuRes, update_id);
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
        outrsp.put("response.data.channelLoad", cuRes.channelLoad);
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
    auto requestLoad = incmd.get<int32_t>("request.parameters.requestLoad");
    auto clientId = incmd.get<uint64_t>("request.parameters.clientId");
    auto poolId = incmd.get<uint64_t>("request.parameters.poolId");
    strncpy(cuProp.kernelName, kernelName.c_str(), XRM_MAX_NAME_LEN - 1);
    strncpy(cuProp.kernelAlias, kernelAlias.c_str(), XRM_MAX_NAME_LEN - 1);
    strcpy(cuProp.cuName, "");
    if (devExcl == 0)
        cuProp.devExcl = false;
    else
        cuProp.devExcl = true;
    cuProp.requestLoad = requestLoad;
    cuProp.clientId = clientId;
    cuProp.poolId = poolId;

    bool update_id = true;
    int32_t ret = m_system->resAllocCuFromDev(deviceId, &cuProp, &cuRes, update_id);
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
        outrsp.put("response.data.channelLoad", cuRes.channelLoad);
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
    if (sameDevice == 0)
        cuListProp.sameDevice = false;
    else
        cuListProp.sameDevice = true;
    for (i = 0; i < cuListProp.cuNum; i++) {
        auto kernelName = incmd.get<std::string>("request.parameters.kernelName" + std::to_string(i));
        auto kernelAlias = incmd.get<std::string>("request.parameters.kernelAlias" + std::to_string(i));
        auto devExcl = incmd.get<int32_t>("request.parameters.devExcl" + std::to_string(i));
        auto requestLoad = incmd.get<int32_t>("request.parameters.requestLoad" + std::to_string(i));
        auto poolId = incmd.get<uint64_t>("request.parameters.poolId" + std::to_string(i));
        strncpy(cuListProp.cuProps[i].kernelName, kernelName.c_str(), XRM_MAX_NAME_LEN - 1);
        strncpy(cuListProp.cuProps[i].kernelAlias, kernelAlias.c_str(), XRM_MAX_NAME_LEN - 1);
        strcpy(cuListProp.cuProps[i].cuName, "");
        if (devExcl == 0)
            cuListProp.cuProps[i].devExcl = false;
        else
            cuListProp.cuProps[i].devExcl = true;
        cuListProp.cuProps[i].requestLoad = requestLoad;
        cuListProp.cuProps[i].clientId = clientId;
        cuListProp.cuProps[i].poolId = poolId;
    }

    memset(&cuListRes, 0, sizeof(cuListResource));
    int32_t ret = m_system->resAllocCuList(&cuListProp, &cuListRes);
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
            outrsp.put("response.data.channelLoad" + std::to_string(i), cuListRes.cuResources[i].channelLoad);
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
    auto poolId = incmd.get<uint64_t>("request.parameters.poolId");
    cuGroupProp.udfCuGroupName = udfCuGroupName;
    cuGroupProp.clientId = clientId;
    cuGroupProp.poolId = poolId;

    memset(&cuGroupRes, 0, sizeof(cuGroupResource));
    int32_t ret = m_system->resAllocCuGroup(&cuGroupProp, &cuGroupRes);
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
            outrsp.put("response.data.channelLoad" + std::to_string(i), cuGroupRes.cuResources[i].channelLoad);
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
    auto channelLoad = incmd.get<int32_t>("request.parameters.channelLoad");
    auto poolId = incmd.get<uint64_t>("request.parameters.poolId");
    cuRes.deviceId = deviceId;
    cuRes.cuId = cuId;
    cuRes.channelId = channelId;
    cuRes.cuType = (cuTypes)cuType;
    cuRes.allocServiceId = allocServiceId;
    cuRes.clientId = clientId;
    cuRes.channelLoad = channelLoad;
    cuRes.poolId = poolId;

    int32_t ret = m_system->resReleaseCu(&cuRes);
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
        auto channelLoad = incmd.get<int32_t>("request.parameters.channelLoad" + std::to_string(i));
        auto poolId = incmd.get<uint64_t>("request.parameters.poolId" + std::to_string(i));
        cuListRes.cuResources[i].deviceId = deviceId;
        cuListRes.cuResources[i].cuId = cuId;
        cuListRes.cuResources[i].channelId = channelId;
        cuListRes.cuResources[i].cuType = (cuTypes)cuType;
        cuListRes.cuResources[i].allocServiceId = allocServiceId;
        cuListRes.cuResources[i].clientId = clientId;
        cuListRes.cuResources[i].channelLoad = channelLoad;
        cuListRes.cuResources[i].poolId = poolId;
    }

    int32_t ret = m_system->resReleaseCuList(&cuListRes);
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
        auto channelLoad = incmd.get<int32_t>("request.parameters.channelLoad" + std::to_string(i));
        auto poolId = incmd.get<uint64_t>("request.parameters.poolId" + std::to_string(i));
        cuGroupRes.cuResources[i].deviceId = deviceId;
        cuGroupRes.cuResources[i].cuId = cuId;
        cuGroupRes.cuResources[i].channelId = channelId;
        cuGroupRes.cuResources[i].cuType = (cuTypes)cuType;
        cuGroupRes.cuResources[i].allocServiceId = allocServiceId;
        cuGroupRes.cuResources[i].clientId = clientId;
        cuGroupRes.cuResources[i].channelLoad = channelLoad;
        cuGroupRes.cuResources[i].poolId = poolId;
    }

    int32_t ret = m_system->resReleaseCuGroup(&cuGroupRes);
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
            auto requestLoad = incmd.get<int32_t>(udfCuListStr + ".requestLoad" + std::to_string(i));
            cuProp->requestLoad = requestLoad;
        }
    }

    int32_t ret = m_system->resUdfCuGroupDeclare(&udfCuGroupInfo);
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

    int32_t ret = m_system->resUdfCuGroupUndeclare(&udfCuGroupInfo);
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

    uint64_t maxCapacity = m_system->getCuMaxCapacity(&cuProp);
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
    cuRes.cuType = (cuTypes)cuType;
    cuRes.allocServiceId = allocServiceId;

    int32_t ret = m_system->checkCuStat(&cuRes, &cuStat);
    outrsp.put("response.status.value", ret);
    if (ret == XRM_SUCCESS) {
        if (cuStat.isBusy)
            outrsp.put("response.data.isBusy", 1);
        else
            outrsp.put("response.data.isBusy", 0);
        outrsp.put("response.data.usedLoad", cuStat.usedLoad);
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
    int32_t ret = m_system->resAllocationQuery(&allocQuery, &cuListRes);
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
            outrsp.put("response.data.channelLoad" + std::to_string(i), cuListRes.cuResources[i].channelLoad);
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

    isCuExisting = m_system->resIsCuExisting(&cuProp);
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

    isCuListExisting = m_system->resIsCuListExisting(&cuListProp);
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

    isCuGroupExisting = m_system->resIsCuGroupExisting(&cuGroupProp);
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
    cuResource* cuRes[XRM_MAX_AVAILABLE_CU_NUM];
    std::string errmsg;
    cuResource* tmpCuRes;
    int32_t i, ret;
    int32_t availableCuNum = 0;

    auto kernelName = incmd.get<std::string>("request.parameters.kernelName");
    auto kernelAlias = incmd.get<std::string>("request.parameters.kernelAlias");
    auto devExcl = incmd.get<int32_t>("request.parameters.devExcl");
    auto requestLoad = incmd.get<int32_t>("request.parameters.requestLoad");
    auto clientId = incmd.get<uint64_t>("request.parameters.clientId");
    auto poolId = incmd.get<uint64_t>("request.parameters.poolId");
    strncpy(cuProp.kernelName, kernelName.c_str(), XRM_MAX_NAME_LEN - 1);
    strncpy(cuProp.kernelAlias, kernelAlias.c_str(), XRM_MAX_NAME_LEN - 1);
    strcpy(cuProp.cuName, "");
    if (devExcl == 0)
        cuProp.devExcl = false;
    else
        cuProp.devExcl = true;
    cuProp.requestLoad = requestLoad;
    cuProp.clientId = clientId;
    cuProp.poolId = poolId;

    bool update_id = true;
    while (availableCuNum < XRM_MAX_AVAILABLE_CU_NUM) {
        tmpCuRes = (cuResource*)malloc(sizeof(cuResource));
        memset(tmpCuRes, 0, sizeof(cuResource));
        ret = m_system->resAllocCu(&cuProp, tmpCuRes, update_id);
        if (ret == XRM_SUCCESS) {
            cuRes[availableCuNum] = tmpCuRes;
            availableCuNum++;
        } else {
            free(tmpCuRes);
            break;
        }
    }
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
}

void xrm::checkCuListAvailableNumCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    cuListProperty cuListProp;
    cuListResource* cuListRes[XRM_MAX_AVAILABLE_LIST_NUM];
    cuListResource* tmpCuListRes;
    std::string errmsg;
    int32_t i, ret;
    int32_t availableListNum = 0;

    memset(&cuListProp, 0, sizeof(cuListProperty));
    auto cuNum = incmd.get<int32_t>("request.parameters.cuNum");
    cuListProp.cuNum = cuNum;
    auto sameDevice = incmd.get<int32_t>("request.parameters.sameDevice");
    auto clientId = incmd.get<uint64_t>("request.parameters.clientId");
    if (sameDevice == 0)
        cuListProp.sameDevice = false;
    else
        cuListProp.sameDevice = true;
    for (i = 0; i < cuListProp.cuNum; i++) {
        auto kernelName = incmd.get<std::string>("request.parameters.kernelName" + std::to_string(i));
        auto kernelAlias = incmd.get<std::string>("request.parameters.kernelAlias" + std::to_string(i));
        auto devExcl = incmd.get<int32_t>("request.parameters.devExcl" + std::to_string(i));
        auto requestLoad = incmd.get<int32_t>("request.parameters.requestLoad" + std::to_string(i));
        auto poolId = incmd.get<uint64_t>("request.parameters.poolId" + std::to_string(i));
        strncpy(cuListProp.cuProps[i].kernelName, kernelName.c_str(), XRM_MAX_NAME_LEN - 1);
        strncpy(cuListProp.cuProps[i].kernelAlias, kernelAlias.c_str(), XRM_MAX_NAME_LEN - 1);
        strcpy(cuListProp.cuProps[i].cuName, "");
        if (devExcl == 0)
            cuListProp.cuProps[i].devExcl = false;
        else
            cuListProp.cuProps[i].devExcl = true;
        cuListProp.cuProps[i].requestLoad = requestLoad;
        cuListProp.cuProps[i].clientId = clientId;
        cuListProp.cuProps[i].poolId = poolId;
    }

    while (availableListNum < XRM_MAX_AVAILABLE_LIST_NUM) {
        tmpCuListRes = (cuListResource*)malloc(sizeof(cuListResource));
        memset(tmpCuListRes, 0, sizeof(cuListResource));
        ret = m_system->resAllocCuList(&cuListProp, tmpCuListRes);
        if (ret == XRM_SUCCESS) {
            cuListRes[availableListNum] = tmpCuListRes;
            availableListNum++;
        } else {
            free(tmpCuListRes);
            break;
        }
    }
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
}

void xrm::checkCuGroupAvailableNumCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    cuGroupProperty cuGroupProp;
    cuGroupResource* cuGroupRes[XRM_MAX_AVAILABLE_GROUP_NUM];
    cuGroupResource* tmpCuGroupRes;
    std::string errmsg;
    int32_t i, ret;
    int32_t availableGroupNum = 0;

    auto udfCuGroupName = incmd.get<std::string>("request.parameters.udfCuGroupName");
    auto clientId = incmd.get<uint64_t>("request.parameters.clientId");
    auto poolId = incmd.get<uint64_t>("request.parameters.poolId");
    cuGroupProp.udfCuGroupName = udfCuGroupName;
    cuGroupProp.clientId = clientId;
    cuGroupProp.poolId = poolId;

    while (availableGroupNum < XRM_MAX_AVAILABLE_GROUP_NUM) {
        tmpCuGroupRes = (cuGroupResource*)malloc(sizeof(cuGroupResource));
        memset(tmpCuGroupRes, 0, sizeof(cuGroupResource));
        ret = m_system->resAllocCuGroup(&cuGroupProp, tmpCuGroupRes);
        if (ret == XRM_SUCCESS) {
            cuGroupRes[availableGroupNum] = tmpCuGroupRes;
            availableGroupNum++;
        } else {
            free(tmpCuGroupRes);
            break;
        }
    }
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
}

void xrm::checkCuPoolAvailableNumCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    cuPoolProperty cuPoolProp;
    cuListProperty* cuListProp = NULL;
    uint64_t cuPoolId[XRM_MAX_AVAILABLE_POOL_NUM];
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
    auto sameDevice = incmd.get<int32_t>("request.parameters.sameDevice");
    if (sameDevice == 0)
        cuListProp->sameDevice = false;
    else
        cuListProp->sameDevice = true;
    for (i = 0; i < cuListProp->cuNum; i++) {
        auto kernelName = incmd.get<std::string>("request.parameters.kernelName" + std::to_string(i));
        auto kernelAlias = incmd.get<std::string>("request.parameters.kernelAlias" + std::to_string(i));
        auto devExcl = incmd.get<int32_t>("request.parameters.devExcl" + std::to_string(i));
        auto requestLoad = incmd.get<int32_t>("request.parameters.requestLoad" + std::to_string(i));
        strncpy(cuListProp->cuProps[i].kernelName, kernelName.c_str(), XRM_MAX_NAME_LEN - 1);
        strncpy(cuListProp->cuProps[i].kernelAlias, kernelAlias.c_str(), XRM_MAX_NAME_LEN - 1);
        strcpy(cuListProp->cuProps[i].cuName, "");
        if (devExcl == 0)
            cuListProp->cuProps[i].devExcl = false;
        else
            cuListProp->cuProps[i].devExcl = true;
        cuListProp->cuProps[i].requestLoad = requestLoad;
        cuListProp->cuProps[i].clientId = clientId;
        cuListProp->cuProps[i].poolId = 0;
    }
    auto uuidStr = incmd.get<std::string>("request.parameters.xclbinUuidStr");
    m_system->hexstrToBin(uuidStr, 2 * sizeof(uuid_t), (unsigned char*)&cuPoolProp.xclbinUuid);
    auto xclbinNum = incmd.get<int32_t>("request.parameters.xclbinNum");
    cuPoolProp.xclbinNum = xclbinNum;

    while (availablePoolNum < XRM_MAX_AVAILABLE_POOL_NUM) {
        poolId = m_system->resReserveCuPool(&cuPoolProp);
        if (poolId != 0) {
            cuPoolId[availablePoolNum] = poolId;
            availablePoolNum++;
        } else {
            break;
        }
    }
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
    if (sameDevice == 0)
        cuListProp->sameDevice = false;
    else
        cuListProp->sameDevice = true;
    for (i = 0; i < cuListProp->cuNum; i++) {
        auto kernelName = incmd.get<std::string>("request.parameters.cuList.kernelName" + std::to_string(i));
        auto kernelAlias = incmd.get<std::string>("request.parameters.cuList.kernelAlias" + std::to_string(i));
        auto devExcl = incmd.get<int32_t>("request.parameters.cuList.devExcl" + std::to_string(i));
        auto requestLoad = incmd.get<int32_t>("request.parameters.cuList.requestLoad" + std::to_string(i));
        strncpy(cuListProp->cuProps[i].kernelName, kernelName.c_str(), XRM_MAX_NAME_LEN - 1);
        strncpy(cuListProp->cuProps[i].kernelAlias, kernelAlias.c_str(), XRM_MAX_NAME_LEN - 1);
        strcpy(cuListProp->cuProps[i].cuName, "");
        if (devExcl == 0)
            cuListProp->cuProps[i].devExcl = false;
        else
            cuListProp->cuProps[i].devExcl = true;
        cuListProp->cuProps[i].requestLoad = requestLoad;
        cuListProp->cuProps[i].clientId = clientId;
        cuListProp->cuProps[i].poolId = 0;
    }
    auto uuidStr = incmd.get<std::string>("request.parameters.xclbinUuidStr");
    m_system->hexstrToBin(uuidStr, 2 * sizeof(uuid_t), (unsigned char*)&cuPoolProp.xclbinUuid);
    auto xclbinNum = incmd.get<int32_t>("request.parameters.xclbinNum");
    cuPoolProp.xclbinNum = xclbinNum;
    cuPoolProp.clientId = clientId;

    uint64_t poolId = m_system->resReserveCuPool(&cuPoolProp);
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
    int32_t ret = m_system->resRelinquishCuPool(poolId);
    outrsp.put("response.status.value", ret);
}

void xrm::reservationQueryCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    cuPoolResource cuPoolRes;
    std::string errmsg;
    int32_t i;

    auto poolId = incmd.get<uint64_t>("request.parameters.poolId");

    memset(&cuPoolRes, 0, sizeof(cuPoolResource));
    int32_t ret = m_system->resReservationQuery(poolId, &cuPoolRes);
    outrsp.put("response.status.value", ret);
    if (ret == XRM_SUCCESS) {
        if (cuPoolRes.cuNum > 16)
            outrsp.put("response.data.cuNum", 16);
        else
            outrsp.put("response.data.cuNum", cuPoolRes.cuNum);
//        for (i = 0; i < cuPoolRes.cuNum; i++) {
        for (i = 0; (i < cuPoolRes.cuNum) && (i < 16); i++) {
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
    auto requestLoad = incmd.get<int32_t>("request.parameters.requestLoad");
    auto clientId = incmd.get<uint64_t>("request.parameters.clientId");
    auto poolId = incmd.get<uint64_t>("request.parameters.poolId");
    auto xclbinFileName = incmd.get<std::string>("request.parameters.xclbinFileName");
    strncpy(cuProp.kernelName, kernelName.c_str(), XRM_MAX_NAME_LEN - 1);
    strncpy(cuProp.kernelAlias, kernelAlias.c_str(), XRM_MAX_NAME_LEN - 1);
    strcpy(cuProp.cuName, "");
    if (devExcl == 0)
        cuProp.devExcl = false;
    else
        cuProp.devExcl = true;
    cuProp.requestLoad = requestLoad;
    cuProp.clientId = clientId;
    cuProp.poolId = poolId;

    bool update_id = true;
    int32_t ret = m_system->resAllocCuWithLoad(&cuProp, xclbinFileName, &cuRes, update_id);
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
        outrsp.put("response.data.channelLoad", cuRes.channelLoad);
        outrsp.put("response.data.poolId", cuRes.poolId);
    }
}

void xrm::loadAndAllCuAllocCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    cuListResource cuListRes;
    std::string errmsg;
    int32_t i;

    auto xclbinFileName = incmd.get<std::string>("request.parameters.xclbinFileName");
    auto clientId = incmd.get<uint64_t>("request.parameters.clientId");

    memset(&cuListRes, 0, sizeof(cuListResource));
    int32_t ret = m_system->resLoadAndAllocAllCu(xclbinFileName, clientId, &cuListRes);
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
            outrsp.put("response.data.channelLoad" + std::to_string(i), cuListRes.cuResources[i].channelLoad);
            outrsp.put("response.data.poolId" + std::to_string(i), cuListRes.cuResources[i].poolId);
        }
    }
}
