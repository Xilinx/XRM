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

#ifndef _XRM_CONFIG_HPP_
#define _XRM_CONFIG_HPP_

#include <string>
#include <iosfwd>
#include <boost/property_tree/ptree_fwd.hpp>

#include "xrm_system.hpp"

#define XRM_DEFAULT_XRT_VERSION_FILE_FULL_PATH_NAME "/opt/xilinx/xrt/version.json"        // default full path name
#define XRM_DEFAULT_LIB_XRT_CORE_FILE_FULL_PATH_NAME "/opt/xilinx/xrt/lib/libxrt_core.so" // default full path name

namespace xrm {
namespace config {

/**
 * Configure file (ini) reader for XRM
 * Reads an xrm.ini file in the directory containing the
 * the host executable that is running.
 *
 * The format is of the form:
 *
 *  [XRM]
 *  verbosity = 3
 *  <any section>]
 *  <any key> = <any value>
 *
 */

uint32_t getVerbosity();
uint32_t getLimitConcurrentClient();
std::string getXrtVersionFileFullPathName();
std::string getLibXrtCoreFileFullPathName();

} // namespace config
} // namespace xrm

#endif // _XRM_CONFIG_HPP_
