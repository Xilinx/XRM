/*
 * Copyright (C) 2019-2021, Xilinx Inc - All rights reserved
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

#ifndef _XRM_COMMAND_CONTROL_HPP_
#define _XRM_COMMAND_CONTROL_HPP_

#include "xrm_command.hpp"

namespace xrm {
class enableDevicesCommand : public command {
   public:
    enableDevicesCommand(xrm::system& sys) : command("enableDevices", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class disableDevicesCommand : public command {
   public:
    disableDevicesCommand(xrm::system& sys) : command("disableDevices", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class enableOneDeviceCommand : public command {
   public:
    enableOneDeviceCommand(xrm::system& sys) : command("enableOneDevice", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class disableOneDeviceCommand : public command {
   public:
    disableOneDeviceCommand(xrm::system& sys) : command("disableOneDevice", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};
} // namespace xrm

#endif // _XRM_COMMAND_CONTROL_HPP_
