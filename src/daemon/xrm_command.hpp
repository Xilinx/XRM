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

#ifndef _XRM_COMMAND_HPP_
#define _XRM_COMMAND_HPP_

#include <map>
#include "xrm_system.hpp"
#include "xrm.h"

namespace pt = boost::property_tree;

namespace xrm {
class command {
   public:
    command(const std::string& name, xrm::system& sys) : m_name(name), m_system(&sys) {}

    virtual ~command() {}
    std::string& getName() { return m_name; }
    virtual void processCmd(pt::ptree& incmd, pt::ptree& outrsp) = 0;

   protected:
    std::string m_name;
    xrm::system* m_system;
};
} // namespace xrm

#endif // _XRM_COMMAND_HPP_
