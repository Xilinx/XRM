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

#ifndef _XRM_COMMAND_PLUGIN_HPP_
#define _XRM_COMMAND_PLUGIN_HPP_

#include "xrm_command.hpp"

namespace xrm {
class loadXrmPluginsCommand : public command {
   public:
    loadXrmPluginsCommand(xrm::system& sys) : command("loadXrmPlugins", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class unloadXrmPluginsCommand : public command {
   public:
    unloadXrmPluginsCommand(xrm::system& sys) : command("unloadXrmPlugins", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};

class execXrmPluginFuncCommand : public command {
   public:
    execXrmPluginFuncCommand(xrm::system& sys) : command("execXrmPluginFunc", sys) {}

    void processCmd(pt::ptree& incmd, pt::ptree& outrsp);
};
} // namespace xrm

#endif // _XRM_COMMAND_PLUGIN_HPP_
