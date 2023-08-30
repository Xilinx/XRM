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

#include "xrm_command_plugin.hpp"

/*
 * The function is for load plugins cmd line from xrmadm.
 */
void xrm::loadXrmPluginsCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    auto requestId = incmd.get<int>("request.requestId");
    outrsp.put("response.name", "loadXrmPlugins");
    outrsp.put("response.requestId", requestId);

    boost::optional<pt::ptree&> loadPluginsTree = incmd.get_child_optional("request.parameters.");
    if (!loadPluginsTree.is_initialized()) {
        outrsp.put("response.status", "failed");
        outrsp.put("response.data.failed", "request parameters is not provided");
        return;
    }
    std::string errmsg;
    m_system->enterLock();
    if (m_system->loadXrmPlugins((pt::ptree&)*loadPluginsTree, errmsg) == XRM_SUCCESS) {
        outrsp.put("response.status", "ok");
    } else {
        outrsp.put("response.status", "failed");
        outrsp.put("response.data.failed", errmsg);
    }
    m_system->exitLock();
}

/*
 * The function is for unload plugins cmd line from xrmadm.
 */
void xrm::unloadXrmPluginsCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    auto requestId = incmd.get<int>("request.requestId");
    outrsp.put("response.name", "unloadXrmPlugins");
    outrsp.put("response.requestId", requestId);

    boost::optional<pt::ptree&> unloadPluginsTree = incmd.get_child_optional("request.parameters.");
    if (!unloadPluginsTree.is_initialized()) {
        outrsp.put("response.status", "failed");
        outrsp.put("response.data.failed", "request parameters is not provided");
        return;
    }
    std::string errmsg;
    m_system->enterLock();
    if (m_system->unloadXrmPlugins((pt::ptree&)*unloadPluginsTree, errmsg) == XRM_SUCCESS) {
        outrsp.put("response.status", "ok");
    } else {
        outrsp.put("response.status", "failed");
        outrsp.put("response.data.failed", errmsg);
    }
    m_system->exitLock();
}

/*
 * The function is for execute plugin function interface from xrm library API.
 */
void xrm::execXrmPluginFuncCommand::processCmd(pt::ptree& incmd, pt::ptree& outrsp) {
    int32_t ret;
    auto requestId = incmd.get<int>("request.requestId");
    outrsp.put("response.name", "execXrmPluginFunc");
    outrsp.put("response.requestId", requestId);

    auto xrmPluginName = incmd.get<std::string>("request.parameters.xrmPluginName");
    auto funcId = incmd.get<uint32_t>("request.parameters.funcId");
    auto input = incmd.get<std::string>("request.parameters.input");
    xrmPluginFuncParam* param = (xrmPluginFuncParam*)malloc(sizeof(xrmPluginFuncParam));
    memset(param, 0, sizeof(xrmPluginFuncParam));
    strncpy(param->input, input.c_str(), XRM_MAX_PLUGIN_FUNC_PARAM_LEN - 1);

    std::string errmsg;
    ret = m_system->execXrmPluginFunc(xrmPluginName.c_str(), funcId, param, errmsg);
    outrsp.put("response.status.value", ret);
    if (ret == XRM_SUCCESS) {
        outrsp.put("response.data.output", param->output);
    } else {
        outrsp.put("response.data.failed", errmsg);
    }
    free(param);
}
