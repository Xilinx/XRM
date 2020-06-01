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

#ifndef _XRM_PLUGIN_EXAMPLE_HPP_
#define _XRM_PLUGIN_EXAMPLE_HPP_

#include <string.h>
#include <syslog.h>

#include "xrm.h"
#include "xrm_error.h"
#include "xrm_limits.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VERSION_1 1
#define EXAMPLE_PLUGIN_VERSION VERSION_1

int32_t xrmPluginExamplePluginVersion(void);
int32_t xrmPluginExampleApiVersion(void);
int32_t xrmPluginExampleFunc_0(xrmPluginFuncParam* param);
int32_t xrmPluginExampleFunc_1(xrmPluginFuncParam* param);
int32_t xrmPluginExampleFunc_2(xrmPluginFuncParam* param);
int32_t xrmPluginExampleFunc_3(xrmPluginFuncParam* param);
int32_t xrmPluginExampleFunc_4(xrmPluginFuncParam* param);
int32_t xrmPluginExampleFunc_5(xrmPluginFuncParam* param);
int32_t xrmPluginExampleFunc_6(xrmPluginFuncParam* param);
int32_t xrmPluginExampleFunc_7(xrmPluginFuncParam* param);

/**
 * Please be noted that veriable name "xrmPluginExample" should be unique for all available plugins. It will
 * be used as the identification for this plugin, and will be used as input for plugin loading.
 **/
xrmPluginData xrmPluginExample{xrmPluginExamplePluginVersion, xrmPluginExampleApiVersion, xrmPluginExampleFunc_0,
                               xrmPluginExampleFunc_1,        xrmPluginExampleFunc_2,     xrmPluginExampleFunc_3,
                               xrmPluginExampleFunc_4,        xrmPluginExampleFunc_5,     xrmPluginExampleFunc_6,
                               xrmPluginExampleFunc_7};

#ifdef __cplusplus
}
#endif

#endif // _XRM_PLUGIN_EXAMPLE_HPP_
