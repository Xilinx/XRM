/*
 * Copyright (C) 2019-2020, Xilinx Inc - All rights reserved
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

#include "xrm_plugin_example.hpp"

int32_t xrmPluginExamplePluginVersion(void) {
    syslog(LOG_NOTICE, "%s(): xrm plugin example version is %d\n", __func__, EXAMPLE_PLUGIN_VERSION);
    return (EXAMPLE_PLUGIN_VERSION);
}

int32_t xrmPluginExampleApiVersion(void) {
    syslog(LOG_NOTICE, "%s(): API version: %d\n", __func__, XRM_API_VERSION_1);
    return (XRM_API_VERSION_1);
}

int32_t xrmPluginExampleFunc_0(xrmPluginFuncParam* param) {
    syslog(LOG_NOTICE, "%s(): input is %s\n", __func__, param->input);
    memcpy(param->output, param->input, XRM_MAX_PLUGIN_FUNC_PARAM_LEN);

    return (XRM_SUCCESS);
}

int32_t xrmPluginExampleFunc_1(xrmPluginFuncParam* param) {
    syslog(LOG_NOTICE, "%s(): input is %s\n", __func__, param->input);
    memcpy(param->output, param->input, XRM_MAX_PLUGIN_FUNC_PARAM_LEN);

    return (XRM_SUCCESS);
}

int32_t xrmPluginExampleFunc_2(xrmPluginFuncParam* param) {
    syslog(LOG_NOTICE, "%s(): input is %s\n", __func__, param->input);
    memcpy(param->output, param->input, XRM_MAX_PLUGIN_FUNC_PARAM_LEN);

    return (XRM_SUCCESS);
}

int32_t xrmPluginExampleFunc_3(xrmPluginFuncParam* param) {
    syslog(LOG_NOTICE, "%s(): input is %s\n", __func__, param->input);
    memcpy(param->output, param->input, XRM_MAX_PLUGIN_FUNC_PARAM_LEN);

    return (XRM_SUCCESS);
}

int32_t xrmPluginExampleFunc_4(xrmPluginFuncParam* param) {
    syslog(LOG_NOTICE, "%s(): input is %s\n", __func__, param->input);
    memcpy(param->output, param->input, XRM_MAX_PLUGIN_FUNC_PARAM_LEN);

    return (XRM_SUCCESS);
}

int32_t xrmPluginExampleFunc_5(xrmPluginFuncParam* param) {
    syslog(LOG_NOTICE, "%s(): input is %s\n", __func__, param->input);
    memcpy(param->output, param->input, XRM_MAX_PLUGIN_FUNC_PARAM_LEN);

    return (XRM_SUCCESS);
}

int32_t xrmPluginExampleFunc_6(xrmPluginFuncParam* param) {
    syslog(LOG_NOTICE, "%s(): input is %s\n", __func__, param->input);
    memcpy(param->output, param->input, XRM_MAX_PLUGIN_FUNC_PARAM_LEN);

    return (XRM_SUCCESS);
}

int32_t xrmPluginExampleFunc_7(xrmPluginFuncParam* param) {
    syslog(LOG_NOTICE, "%s(): input is %s\n", __func__, param->input);
    memcpy(param->output, param->input, XRM_MAX_PLUGIN_FUNC_PARAM_LEN);

    return (XRM_SUCCESS);
}
