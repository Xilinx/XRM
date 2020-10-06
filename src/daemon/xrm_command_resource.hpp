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

class loadAndAllCuAllocCommand : public command {
   public:
    loadAndAllCuAllocCommand(xrm::system& sys) : command("loadAndAllCuAlloc", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

} // namespace xrm

#endif // _XRM_COMMAND_RESOURCE_HPP_
