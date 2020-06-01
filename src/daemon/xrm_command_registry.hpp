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

#ifndef _XRM_COMMAND_REGISTRY_HPP_
#define _XRM_COMMAND_REGISTRY_HPP_

#include <map>
#include <boost/property_tree/ptree.hpp>
#include "xrm_command.hpp"

namespace pt = boost::property_tree;

namespace xrm {
// Forward declare
class command;
class system;

class commandRegistry {
   public:
    commandRegistry() {}

    void dispatch(std::string& name, pt::ptree& incmd, pt::ptree& outrsp);
    void registerAll(system& sys);

   private:
    void registerCmd(xrm::command& cmd);
    std::map<std::string, xrm::command*> m_registry;
};
} // namespace xrm

#endif // _XRM_COMMAND_REGISTRY_HPP_
