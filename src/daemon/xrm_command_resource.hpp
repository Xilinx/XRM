/*
 * Copyright (C) 2019-2021, Xilinx Inc - All rights reserved
 *
 * Copyright (C) 2023, Advanced Micro Devices, Inc. All rights reserved.
 *
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

#ifndef _XRM_COMMAND_RESOURCE_HPP_
#define _XRM_COMMAND_RESOURCE_HPP_

#include "xrm_command.hpp"

namespace xrm {
class createContextCommand : public command {
   public:
    createContextCommand(xrm::system& sys) : command("createContext", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class echoContextCommand : public command {
   public:
    echoContextCommand(xrm::system& sys) : command("echoContext", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class destroyContextCommand : public command {
   public:
    destroyContextCommand(xrm::system& sys) : command("destroyContext", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class isDaemonRunningCommand : public command {
   public:
    isDaemonRunningCommand(xrm::system& sys) : command("isDaemonRunning", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class cuAllocCommand : public command {
   public:
    cuAllocCommand(xrm::system& sys) : command("cuAlloc", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class cuAllocFromDevCommand : public command {
   public:
    cuAllocFromDevCommand(xrm::system& sys) : command("cuAllocFromDev", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class cuAllocLeastUsedFromDevCommand : public command {
   public:
    cuAllocLeastUsedFromDevCommand(xrm::system& sys) : command("cuAllocLeastUsedFromDev", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class cuListAllocCommand : public command {
   public:
    cuListAllocCommand(xrm::system& sys) : command("cuListAlloc", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class udfCuGroupDeclareCommand : public command {
   public:
    udfCuGroupDeclareCommand(xrm::system& sys) : command("udfCuGroupDeclare", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class udfCuGroupUndeclareCommand : public command {
   public:
    udfCuGroupUndeclareCommand(xrm::system& sys) : command("udfCuGroupUndeclare", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class cuGroupAllocCommand : public command {
   public:
    cuGroupAllocCommand(xrm::system& sys) : command("cuGroupAlloc", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class cuReleaseCommand : public command {
   public:
    cuReleaseCommand(xrm::system& sys) : command("cuRelease", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class cuListReleaseCommand : public command {
   public:
    cuListReleaseCommand(xrm::system& sys) : command("cuListRelease", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class cuGroupReleaseCommand : public command {
   public:
    cuGroupReleaseCommand(xrm::system& sys) : command("cuGroupRelease", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class cuGetMaxCapacityCommand : public command {
   public:
    cuGetMaxCapacityCommand(xrm::system& sys) : command("cuGetMaxCapacity", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class cuCheckStatusCommand : public command {
   public:
    cuCheckStatusCommand(xrm::system& sys) : command("cuCheckStatus", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class allocationQueryCommand : public command {
   public:
    allocationQueryCommand(xrm::system& sys) : command("allocationQuery", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class checkCuAvailableNumCommand : public command {
   public:
    checkCuAvailableNumCommand(xrm::system& sys) : command("checkCuAvailableNum", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class isCuExistingCommand : public command {
   public:
    isCuExistingCommand(xrm::system& sys) : command("isCuExisting", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class isCuListExistingCommand : public command {
   public:
    isCuListExistingCommand(xrm::system& sys) : command("isCuListExisting", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class isCuGroupExistingCommand : public command {
   public:
    isCuGroupExistingCommand(xrm::system& sys) : command("isCuGroupExisting", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class checkCuListAvailableNumCommand : public command {
   public:
    checkCuListAvailableNumCommand(xrm::system& sys) : command("checkCuListAvailableNum", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class checkCuGroupAvailableNumCommand : public command {
   public:
    checkCuGroupAvailableNumCommand(xrm::system& sys) : command("checkCuGroupAvailableNum", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class checkCuPoolAvailableNumCommand : public command {
   public:
    checkCuPoolAvailableNumCommand(xrm::system& sys) : command("checkCuPoolAvailableNum", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class cuPoolReserveCommand : public command {
   public:
    cuPoolReserveCommand(xrm::system& sys) : command("cuPoolReserve", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class cuPoolRelinquishCommand : public command {
   public:
    cuPoolRelinquishCommand(xrm::system& sys) : command("cuPoolRelinquish", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class reservationQueryCommand : public command {
   public:
    reservationQueryCommand(xrm::system& sys) : command("reservationQuery", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class cuAllocWithLoadCommand : public command {
   public:
    cuAllocWithLoadCommand(xrm::system& sys) : command("cuAllocWithLoad", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class cuAllocLeastUsedWithLoadCommand : public command {
   public:
    cuAllocLeastUsedWithLoadCommand(xrm::system& sys) : command("cuAllocLeastUsedWithLoad", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class loadAndAllCuAllocCommand : public command {
   public:
    loadAndAllCuAllocCommand(xrm::system& sys) : command("loadAndAllCuAlloc", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class cuAllocV2Command : public command {
   public:
    cuAllocV2Command(xrm::system& sys) : command("cuAllocV2", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class cuListAllocV2Command : public command {
   public:
    cuListAllocV2Command(xrm::system& sys) : command("cuListAllocV2", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class cuReleaseV2Command : public command {
   public:
    cuReleaseV2Command(xrm::system& sys) : command("cuReleaseV2", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class cuListReleaseV2Command : public command {
   public:
    cuListReleaseV2Command(xrm::system& sys) : command("cuListReleaseV2", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class udfCuGroupDeclareV2Command : public command {
   public:
    udfCuGroupDeclareV2Command(xrm::system& sys) : command("udfCuGroupDeclareV2", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class udfCuGroupUndeclareV2Command : public command {
   public:
    udfCuGroupUndeclareV2Command(xrm::system& sys) : command("udfCuGroupUndeclareV2", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class cuGroupAllocV2Command : public command {
   public:
    cuGroupAllocV2Command(xrm::system& sys) : command("cuGroupAllocV2", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class cuGroupReleaseV2Command : public command {
   public:
    cuGroupReleaseV2Command(xrm::system& sys) : command("cuGroupReleaseV2", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class allocationQueryV2Command : public command {
   public:
    allocationQueryV2Command(xrm::system& sys) : command("allocationQueryV2", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class checkCuAvailableNumV2Command : public command {
   public:
    checkCuAvailableNumV2Command(xrm::system& sys) : command("checkCuAvailableNumV2", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class checkCuListAvailableNumV2Command : public command {
   public:
    checkCuListAvailableNumV2Command(xrm::system& sys) : command("checkCuListAvailableNumV2", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class checkCuGroupAvailableNumV2Command : public command {
   public:
    checkCuGroupAvailableNumV2Command(xrm::system& sys) : command("checkCuGroupAvailableNumV2", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class checkCuPoolAvailableNumV2Command : public command {
   public:
    checkCuPoolAvailableNumV2Command(xrm::system& sys) : command("checkCuPoolAvailableNumV2", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class cuPoolReserveV2Command : public command {
   public:
    cuPoolReserveV2Command(xrm::system& sys) : command("cuPoolReserveV2", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class cuPoolRelinquishV2Command : public command {
   public:
    cuPoolRelinquishV2Command(xrm::system& sys) : command("cuPoolRelinquishV2", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class reservationQueryV2Command : public command {
   public:
    reservationQueryV2Command(xrm::system& sys) : command("reservationQueryV2", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

} // namespace xrm

#endif // _XRM_COMMAND_RESOURCE_HPP_
