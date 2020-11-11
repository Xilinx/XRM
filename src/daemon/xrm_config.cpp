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

#include "xrm_config.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <iostream>
#include <cstdlib>
#include <syslog.h>

#ifdef __GNUC__
#include <linux/limits.h>
#include <sys/stat.h>
#endif

namespace {

static bool isTrue(const std::string& str) {
    return str == "true";
}

static std::string getSelfPath() {
#ifdef __GNUC__
    char buf[PATH_MAX] = {0};
    auto len = ::readlink("/proc/self/exe", buf, PATH_MAX);
    return std::string(buf, (len > 0) ? len : 0);
#else
    return "";
#endif
}

/*
 * To check whether xrm.ini existing
 */
static std::string verifyIniPath(const boost::filesystem::path& dir) {
    auto file_path = dir / "xrm.ini";
    if (boost::filesystem::exists(file_path)) return file_path.string();

    return "";
}

static std::string getIniPath() {
    std::string fullPath = "";
    try {
        auto exePath = boost::filesystem::path(getSelfPath()).parent_path();
        fullPath = verifyIniPath(exePath);
        if (!fullPath.empty()) return fullPath;

        auto selfPath = boost::filesystem::current_path();
        fullPath = verifyIniPath(selfPath);
        if (!fullPath.empty()) return fullPath;

    } catch (const boost::filesystem::filesystem_error& e) {
        syslog(LOG_NOTICE, "Exception: %s", e.what());
    }
    return fullPath;
}

struct tree {
    boost::property_tree::ptree m_tree;

    void read(const std::string& path) {
        try {
            read_ini(path, m_tree);

        } catch (const std::exception& e) {
            syslog(LOG_NOTICE, "Exception: %s", e.what());
        }
    }

    tree() {
        auto iniPath = getIniPath();
        if (!iniPath.empty()) read(iniPath);

        return;
    }
};

static tree localTree;

} // namespace

namespace xrm {

const char* getEnvValue(const char* env) {
    return std::getenv(env);
}

bool getBoolValue(const char* key, bool defaultValue) {
    if (auto env = getEnvValue(key)) return isTrue(env);

    return localTree.m_tree.get<bool>(key, defaultValue);
}

uint32_t getUint32Value(const char* key, uint32_t defaultValue) {
    return localTree.m_tree.get<uint32_t>(key, defaultValue);
}

std::string getStringValue(const char* key, const std::string& defaultValue) {
    std::string val = localTree.m_tree.get<std::string>(key, defaultValue);
    /**
     * Although INI file entries are not supposed to have quotes around strings
     * but here to be cautious
     */
    if (!val.empty() && (val.front() == '"') && (val.back() == '"')) {
        val.erase(0, 1);
        val.erase(val.size() - 1);
    }
    return val;
}

namespace config {

uint32_t getVerbosity() {
    return (getUint32Value("XRM.verbosity", XRM_DEFAULT_LOG_LEVEL));
}

uint32_t getLimitConcurrentClient() {
    return (getUint32Value("XRM.limitConcurrentClient", XRM_DEFAULT_LIMIT_CONCURRENT_CLIENT));
}

std::string getLibXrtCoreFileFullPathName() {
    return (getStringValue("XRM.libXrtCoreFileFullPathName", XRM_DEFAULT_LIB_XRT_CORE_FILE_FULL_PATH_NAME));
}

std::string getXrtVersionFileFullPathName() {
    return (getStringValue("XRM.xrtVersionFileFullPathName", XRM_DEFAULT_XRT_VERSION_FILE_FULL_PATH_NAME));
}

} // namespace config

} // namespace xrm
