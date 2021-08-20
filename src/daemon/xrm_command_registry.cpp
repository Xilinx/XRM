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

#include "xrm_command.hpp"
#include "xrm_command_registry.hpp"
#include "xrm_command_control.hpp"
#include "xrm_command_load.hpp"
#include "xrm_command_list.hpp"
#include "xrm_command_unload.hpp"
#include "xrm_command_resource.hpp"
#include "xrm_command_plugin.hpp"

void xrm::commandRegistry::dispatch(std::string& name, pt::ptree& incmd, pt::ptree& outrsp) {
    auto iter = m_registry.find(name);
    if (iter != m_registry.end()) {
        iter->second->processCmd(incmd, outrsp);
    } else {
        outrsp.put("response.status", "failed");
        outrsp.put("response.data.failed", "unsupported cmd name: " + name);
    }
}

void xrm::commandRegistry::registerAll(system& sys) {
    auto list = new xrm::listCommand(sys);
    registerCmd(*list);

    auto load = new xrm::loadCommand(sys);
    registerCmd(*load);

    auto loadOneDevice = new xrm::loadOneDeviceCommand(sys);
    registerCmd(*loadOneDevice);

    auto unload = new xrm::unloadCommand(sys);
    registerCmd(*unload);

    auto unloadOneDevice = new xrm::unloadOneDeviceCommand(sys);
    registerCmd(*unloadOneDevice);

    auto enableDevices = new xrm::enableDevicesCommand(sys);
    registerCmd(*enableDevices);

    auto disableDevices = new xrm::disableDevicesCommand(sys);
    registerCmd(*disableDevices);

    auto enableOneDevice = new xrm::enableOneDeviceCommand(sys);
    registerCmd(*enableOneDevice);

    auto disableOneDevice = new xrm::disableOneDeviceCommand(sys);
    registerCmd(*disableOneDevice);

    auto createContext = new xrm::createContextCommand(sys);
    registerCmd(*createContext);

    auto echoContext = new xrm::echoContextCommand(sys);
    registerCmd(*echoContext);

    auto destroyContext = new xrm::destroyContextCommand(sys);
    registerCmd(*destroyContext);

    auto isDaemonRunning = new xrm::isDaemonRunningCommand(sys);
    registerCmd(*isDaemonRunning);

    auto isCuExisting = new xrm::isCuExistingCommand(sys);
    registerCmd(*isCuExisting);

    auto isCuListExisting = new xrm::isCuListExistingCommand(sys);
    registerCmd(*isCuListExisting);

    auto isCuGroupExisting = new xrm::isCuGroupExistingCommand(sys);
    registerCmd(*isCuGroupExisting);

    auto cuAlloc = new xrm::cuAllocCommand(sys);
    registerCmd(*cuAlloc);

    auto cuAllocFromDev = new xrm::cuAllocFromDevCommand(sys);
    registerCmd(*cuAllocFromDev);

    auto cuAllocLeastUsedFromDev = new xrm::cuAllocLeastUsedFromDevCommand(sys);
    registerCmd(*cuAllocLeastUsedFromDev);

    auto cuRelease = new xrm::cuReleaseCommand(sys);
    registerCmd(*cuRelease);

    auto cuListAlloc = new xrm::cuListAllocCommand(sys);
    registerCmd(*cuListAlloc);

    auto cuListRelease = new xrm::cuListReleaseCommand(sys);
    registerCmd(*cuListRelease);

    auto udfCuGroupDeclare = new xrm::udfCuGroupDeclareCommand(sys);
    registerCmd(*udfCuGroupDeclare);

    auto udfCuGroupUndeclare = new xrm::udfCuGroupUndeclareCommand(sys);
    registerCmd(*udfCuGroupUndeclare);

    auto cuGroupAlloc = new xrm::cuGroupAllocCommand(sys);
    registerCmd(*cuGroupAlloc);

    auto cuGroupRelease = new xrm::cuGroupReleaseCommand(sys);
    registerCmd(*cuGroupRelease);

    auto cuPoolReserve = new xrm::cuPoolReserveCommand(sys);
    registerCmd(*cuPoolReserve);

    auto cuPoolRelinquish = new xrm::cuPoolRelinquishCommand(sys);
    registerCmd(*cuPoolRelinquish);

    auto cuGetMaxCapacity = new xrm::cuGetMaxCapacityCommand(sys);
    registerCmd(*cuGetMaxCapacity);

    auto cuCheckStatus = new xrm::cuCheckStatusCommand(sys);
    registerCmd(*cuCheckStatus);

    auto allocationQuery = new xrm::allocationQueryCommand(sys);
    registerCmd(*allocationQuery);

    auto reservationQuery = new xrm::reservationQueryCommand(sys);
    registerCmd(*reservationQuery);

    auto checkCuAvailableNum = new xrm::checkCuAvailableNumCommand(sys);
    registerCmd(*checkCuAvailableNum);

    auto checkCuListAvailableNum = new xrm::checkCuListAvailableNumCommand(sys);
    registerCmd(*checkCuListAvailableNum);

    auto checkCuGroupAvailableNum = new xrm::checkCuGroupAvailableNumCommand(sys);
    registerCmd(*checkCuGroupAvailableNum);

    auto checkCuPoolAvailableNum = new xrm::checkCuPoolAvailableNumCommand(sys);
    registerCmd(*checkCuPoolAvailableNum);

    auto loadXrmPlugins = new xrm::loadXrmPluginsCommand(sys);
    registerCmd(*loadXrmPlugins);

    auto unloadXrmPlugins = new xrm::unloadXrmPluginsCommand(sys);
    registerCmd(*unloadXrmPlugins);

    auto execXrmPluginFunc = new xrm::execXrmPluginFuncCommand(sys);
    registerCmd(*execXrmPluginFunc);

    auto cuAllocWithLoad = new xrm::cuAllocWithLoadCommand(sys);
    registerCmd(*cuAllocWithLoad);

    auto cuAllocLeastUsedWithLoad = new xrm::cuAllocLeastUsedWithLoadCommand(sys);
    registerCmd(*cuAllocLeastUsedWithLoad);

    auto loadAndAllCuAlloc = new xrm::loadAndAllCuAllocCommand(sys);
    registerCmd(*loadAndAllCuAlloc);

    auto cuAllocV2 = new xrm::cuAllocV2Command(sys);
    registerCmd(*cuAllocV2);

    auto cuReleaseV2 = new xrm::cuReleaseV2Command(sys);
    registerCmd(*cuReleaseV2);

    auto cuListAllocV2 = new xrm::cuListAllocV2Command(sys);
    registerCmd(*cuListAllocV2);

    auto cuListReleaseV2 = new xrm::cuListReleaseV2Command(sys);
    registerCmd(*cuListReleaseV2);

    auto udfCuGroupDeclareV2 = new xrm::udfCuGroupDeclareV2Command(sys);
    registerCmd(*udfCuGroupDeclareV2);

    auto udfCuGroupUndeclareV2 = new xrm::udfCuGroupUndeclareV2Command(sys);
    registerCmd(*udfCuGroupUndeclareV2);

    auto cuGroupAllocV2 = new xrm::cuGroupAllocV2Command(sys);
    registerCmd(*cuGroupAllocV2);

    auto cuGroupReleaseV2 = new xrm::cuGroupReleaseV2Command(sys);
    registerCmd(*cuGroupReleaseV2);

    auto cuPoolReserveV2 = new xrm::cuPoolReserveV2Command(sys);
    registerCmd(*cuPoolReserveV2);

    auto cuPoolRelinquishV2 = new xrm::cuPoolRelinquishV2Command(sys);
    registerCmd(*cuPoolRelinquishV2);

    auto allocationQueryV2 = new xrm::allocationQueryV2Command(sys);
    registerCmd(*allocationQueryV2);

    auto reservationQueryV2 = new xrm::reservationQueryV2Command(sys);
    registerCmd(*reservationQueryV2);

    auto checkCuAvailableNumV2 = new xrm::checkCuAvailableNumV2Command(sys);
    registerCmd(*checkCuAvailableNumV2);

    auto checkCuListAvailableNumV2 = new xrm::checkCuListAvailableNumV2Command(sys);
    registerCmd(*checkCuListAvailableNumV2);

    auto checkCuGroupAvailableNumV2 = new xrm::checkCuGroupAvailableNumV2Command(sys);
    registerCmd(*checkCuGroupAvailableNumV2);

    auto checkCuPoolAvailableNumV2 = new xrm::checkCuPoolAvailableNumV2Command(sys);
    registerCmd(*checkCuPoolAvailableNumV2);
}

void xrm::commandRegistry::registerCmd(xrm::command& cmd) {
    m_registry.insert(std::make_pair(cmd.getName(), &cmd));
}
