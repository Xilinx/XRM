/*
 * Copyright (C) 2019-2021, Xilinx Inc - All rights reserved
 * Xilinx Resource Management
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

#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/filesystem.hpp>
#include <pthread.h>

#include "xrm_system.hpp"
#include "xrm_config.hpp"

/*
 * All system / resource related operation should be protected by the system lock.
 *
 * The initSystem() lock enter/exit is called inside.
 *
 * All other operations will be called from command line / sigbus handler / socket close,
 * so the lock enter/exit is called from corresponding entry.
 */

/*
 * init operation should be protected by lock
 */
void xrm::system::initSystem() {
    enterLock();
    /* init the configuration */
    initConfig();
    /* init the version depended lib functions */
    initLibVersionDepFunctions();

    m_devicesInited = false;
#if 0
    /*
     * Considering the FPGA devices may not ready before XRM daemon started,
     * so to do the device init later.
     */
    /* init the devices */
    if (!m_devicesInited)
        initDevices();
#endif

    /* init the xrm plugins */
    initXrmPlugins();
    /* init the user defined cu groups */
    initUdfCuGroups();

    m_clientId = 0;
    m_numConcurrentClient = 0;
    m_allocServiceId = 0;
    m_reservePoolId = 0;
    exitLock();
}

/**
 * logMsg() - log system message for XRM
 *
 * @logLevel: requested log level
 * @format, ...: message to be logged
 * @return: void
 **/
void xrm::system::logMsg(xrmLogLevelType logLevel, const char* format, ...) {
    if (logLevel <= m_logLevel) {
        va_list tmpArgs;
        va_start(tmpArgs, format);
        int32_t len = std::vsnprintf(NULL, 0, format, tmpArgs);
        va_end(tmpArgs);
        if (len < 0) return;

        ++len; // To include null terminator
        std::vector<char> msg(len);
        va_list args;
        va_start(args, format);
        len = std::vsnprintf(msg.data(), len, format, args);
        va_end(args);
        syslog(logLevel, "%s", msg.data());
    }
}

int32_t xrm::system::getLogLevel() {
    return (m_logLevel);
}

/*
 * call while holding lock
 */
void xrm::system::initConfig() {
    m_logLevel = xrm::config::getVerbosity();
    if (m_logLevel > XRM_MAX_LOG_LEVEL) m_logLevel = XRM_DEFAULT_LOG_LEVEL;
    logMsg(XRM_LOG_NOTICE, "%s : logLevel = %d", __func__, m_logLevel);
    m_limitConcurrentClient = xrm::config::getLimitConcurrentClient();
    if (m_limitConcurrentClient > XRM_MAX_LIMIT_CONCURRENT_CLIENT)
        m_limitConcurrentClient = XRM_MAX_LIMIT_CONCURRENT_CLIENT;
    logMsg(XRM_LOG_NOTICE, "%s : limitConcurrentClient = %d", __func__, m_limitConcurrentClient);
    m_xrtVersionFileFullPathName = xrm::config::getXrtVersionFileFullPathName();
    logMsg(XRM_LOG_NOTICE, "%s : xrtVersionFileFullPathName = %s", __func__, m_xrtVersionFileFullPathName.c_str());
    m_libXrtCoreFileFullPathName = xrm::config::getLibXrtCoreFileFullPathName();
    logMsg(XRM_LOG_NOTICE, "%s : libXrtCoreFileFullPathName = %s", __func__, m_libXrtCoreFileFullPathName.c_str());
}

/*
 * open the dll library and get the functions
 */
void xrm::system::initLibVersionDepFunctions() {
    char* errorMsg;
    pt::ptree xrtVersionPtree;

    m_libVersionDepFuncs.libVersionDepFuncXclLockDevice = NULL;
    m_libVersionDepFuncs.libVersionDepFuncXclUnlockDevice = NULL;
    m_libVersionDepFuncs.libVersionDepFuncXclCuName2Index = NULL;
    m_libVersionDepFuncs.libVersionDepFuncXclIPName2Index25 = NULL;
    m_libVersionDepFuncs.libVersionDepFuncXclIPName2Index26 = NULL;

    /* load xrt library file */
    void* libXrtcoreHandle = dlopen(m_libXrtCoreFileFullPathName.c_str(), (RTLD_GLOBAL | RTLD_NOW));
    if (!libXrtcoreHandle) {
        errorMsg = dlerror();
        logMsg(XRM_LOG_ERROR, "%s: Failed to open %s\n Error msg: %s\n", __func__, m_libXrtCoreFileFullPathName.c_str(),
               errorMsg);
        return;
    }

    libVersionDepFuncXclLockDeviceType libVersionDepFuncXclLockDevice =
        (libVersionDepFuncXclLockDeviceType)dlsym(libXrtcoreHandle, "xclLockDevice");
    if ((errorMsg = dlerror()) != NULL) {
        logMsg(XRM_LOG_NOTICE, "%s: Function xclLockDevice not implemented\n Debug msg: %s\n", __func__, errorMsg);
        m_libVersionDepFuncs.libVersionDepFuncXclLockDevice = NULL;
    } else {
        m_libVersionDepFuncs.libVersionDepFuncXclLockDevice = libVersionDepFuncXclLockDevice;
    }

    libVersionDepFuncXclUnlockDeviceType libVersionDepFuncXclUnlockDevice =
        (libVersionDepFuncXclUnlockDeviceType)dlsym(libXrtcoreHandle, "xclUnlockDevice");
    if ((errorMsg = dlerror()) != NULL) {
        logMsg(XRM_LOG_NOTICE, "%s: Function xclUnlockDevice not implemented\n Debug msg: %s\n", __func__, errorMsg);
        m_libVersionDepFuncs.libVersionDepFuncXclUnlockDevice = NULL;
    } else {
        m_libVersionDepFuncs.libVersionDepFuncXclUnlockDevice = libVersionDepFuncXclUnlockDevice;
    }

    libVersionDepFuncXclCuName2IndexType libFuncXclCuName2Index =
        (libVersionDepFuncXclCuName2IndexType)dlsym(libXrtcoreHandle, "xclCuName2Index");
    if ((errorMsg = dlerror()) != NULL) {
        logMsg(XRM_LOG_NOTICE, "%s: Function xclCuName2Index not implemented\n Debug msg: %s\n", __func__, errorMsg);
        m_libVersionDepFuncs.libVersionDepFuncXclCuName2Index = NULL;
    } else {
        m_libVersionDepFuncs.libVersionDepFuncXclCuName2Index = libFuncXclCuName2Index;
    }

    /* load xrt version json file */
    try {
        pt::read_json(m_xrtVersionFileFullPathName.c_str(), xrtVersionPtree);
    } catch (const boost::property_tree::json_parser_error& e) {
        std::string eMsg = "Error: " + e.message();
        logMsg(XRM_LOG_ERROR, "%s: Failed to read %s, \n", __func__, m_xrtVersionFileFullPathName.c_str(), eMsg);
        return;
    }
    std::string xrtBuildVersion = xrtVersionPtree.get<std::string>("BUILD_VERSION", "");
    if (xrtBuildVersion.c_str()[0] == '\0') {
        logMsg(XRM_LOG_ERROR, "%s: BUILD_VERSION is not set in %s\n", __func__, m_xrtVersionFileFullPathName.c_str());
        return;
    }
    logMsg(XRM_LOG_NOTICE, "%s: XRT BUILD_VERSION: %s\n", __func__, xrtBuildVersion.c_str());
    if (Version(xrtBuildVersion.c_str()) < Version("2.6.0")) {
        // for XRT 2.5.xxx
        libVersionDepFuncXclIPName2IndexType25 libFuncXclIPName2Index25 =
            (libVersionDepFuncXclIPName2IndexType25)dlsym(libXrtcoreHandle, "xclIPName2Index");
        if ((errorMsg = dlerror()) != NULL) {
            logMsg(XRM_LOG_NOTICE, "%s: Function xclIPName2Index not implemented\n Debug msg: %s\n", __func__,
                   errorMsg);
            m_libVersionDepFuncs.libVersionDepFuncXclIPName2Index25 = NULL;
        } else {
            m_libVersionDepFuncs.libVersionDepFuncXclIPName2Index25 = libFuncXclIPName2Index25;
        }
    } else {
        // for XRT 2.6.xxx
        libVersionDepFuncXclIPName2IndexType26 libFuncXclIPName2Index26 =
            (libVersionDepFuncXclIPName2IndexType26)dlsym(libXrtcoreHandle, "xclIPName2Index");
        if ((errorMsg = dlerror()) != NULL) {
            logMsg(XRM_LOG_NOTICE, "%s: Function xclIPName2Index not implemented\n Debug msg: %s\n", __func__,
                   errorMsg);
            m_libVersionDepFuncs.libVersionDepFuncXclIPName2Index26 = NULL;
        } else {
            m_libVersionDepFuncs.libVersionDepFuncXclIPName2Index26 = libFuncXclIPName2Index26;
        }
    }
}

/*
 * call while holding lock
 */
void xrm::system::initDevices() {
    /* init the devices */
    if (m_devicesInited) return;

    int32_t numDevice = xclProbe();
    m_devicesInited = true;

    if (numDevice <= 0 || numDevice > XRM_MAX_XILINX_DEVICES) {
        logMsg(XRM_LOG_ERROR, "%s : Probed %d devices, out of range.", __func__, numDevice);
        return;
    }
    m_numDevice = numDevice;
    for (int32_t devId = 0; devId < numDevice; devId++) {
        flushDevData(devId);
        m_devList[devId].devId = devId;
        m_devList[devId].isDisabled = false;
        m_devList[devId].devLoadInfo.init(devId);
        openDevice(devId);
    }
}

/*
 * call while holding lock
 */
void xrm::system::initXrmPlugins() {
    /* init the plugins */
    m_numXrmPlugin = 0;
    for (uint32_t xrmPluginId = 0; xrmPluginId < XRM_MAX_PLUGIN_NUM; xrmPluginId++) {
        flushXrmPluginInfo(xrmPluginId);
    }
}

/*
 * call while holding lock
 */
void xrm::system::initUdfCuGroups() {
    /* init the user defined cu groups */
    m_numUdfCuGroup = 0;
    uint32_t udfCuGroupIdx;
    for (uint32_t udfCuGroupIdx = 0; udfCuGroupIdx < XRM_MAX_UDF_CU_GROUP_NUM; udfCuGroupIdx++) {
        flushUdfCuGroupInfo(udfCuGroupIdx);
    }
    m_numUdfCuGroupV2 = 0;
    for (udfCuGroupIdx = 0; udfCuGroupIdx < XRM_MAX_UDF_CU_GROUP_NUM_V2; udfCuGroupIdx++) {
        flushUdfCuGroupInfoV2(udfCuGroupIdx);
    }
}

/*
 * It's used to assign new client id for new connection during context creating.
 *
 * Need to be proected by lock.
 */
uint64_t xrm::system::getNewClientId() {
    uint64_t clientId;
    if (m_clientId == 0xFFFFFFFFFFFFFFFF)
        m_clientId = 1;
    else
        m_clientId++;
    clientId = m_clientId;
    return (clientId);
}

void* xrm::system::listDevice(int32_t devId) {
    /* if the device is not opened yet, then need to open it at first */
    if (m_devList[devId].deviceHandle == 0) openDevice(devId);
#if 1
    /* For test and debug */
    deviceDumpResource(devId);
#endif
    return ((void*)&m_devList[devId]);
}

int32_t xrm::system::getDeviceNumber() {
    /*
     * XRM daemon may start before FPGA device ready during boot,
     * so try to init devices at the first time using devices.
     */
    if (!m_devicesInited) initDevices();

    return (m_numDevice);
}

/*
 * call while holding lock
 */
int32_t xrm::system::openDevice(int32_t devId) {
    int32_t ret = 0;
    if (devId < 0 || devId >= m_numDevice) {
        logMsg(XRM_LOG_ERROR, "%s : device id %d out of range, device num = %d\n", __func__, devId, m_numDevice);
        ret = XRM_ERROR_INVALID;
        return (ret);
    }
    deviceData* dev = &m_devList[devId];

    dev->deviceHandle = xclOpen(devId, NULL, XCL_INFO);
    ret = xclGetDeviceInfo2(dev->deviceHandle, &dev->deviceInfo);
    if (ret != 0) {
        logMsg(XRM_LOG_ERROR, "%s : Could not get device %d info", __func__, devId);
        return (ret);
    }

    dev->dsaName = dev->deviceInfo.mName;
#if 0
    logMsg(LOG_NOTICE, "%s : device %d dsaName = %s", __func__, devId, dev->dsaName.c_str());
#endif
    return (ret);
}

bool static isFileExist(const std::string& fileName) {
    struct stat buf;
    return (stat(fileName.c_str(), &buf) == 0);
}

/*
 * call while holding lock
 */
int32_t xrm::system::isDeviceOffline(int32_t devId) {
    int32_t status = XRM_ERROR_INVALID;
    if (devId < 0 || devId >= m_numDevice) {
        logMsg(XRM_LOG_ERROR, "%s : device id %d out of range, device num = %d\n", __func__, devId, m_numDevice);
        return (status);
    }
    deviceData* dev = &m_devList[devId];
    if (m_devList[devId].deviceHandle == 0) {
        logMsg(XRM_LOG_ERROR, "%s : device %d is not opened\n", __func__, devId);
        return (status);
    }

    /* mPciSlot is "(mDev->domain<<16) + (mDev->bus<<8) + (mDev->dev<<3) + mDev->func" */
    int32_t pciSlot = dev->deviceInfo.mPciSlot;
    int32_t domain, bus, device, func;
    std::string domainStr, busStr, deviceStr, funcStr, offlineFileName;
    std::stringstream domainSs, busSs, deviceSs, funcSs;
    func = pciSlot & 0x7;
    funcSs << std::setw(1) << std::setfill('0') << std::hex << func;
    funcStr = funcSs.str();
    device = (pciSlot >> 3) & 0x1F;
    deviceSs << std::setw(2) << std::setfill('0') << std::hex << device;
    deviceStr = deviceSs.str();
    bus = (pciSlot >> 8) & 0xFF;
    busSs << std::setw(2) << std::setfill('0') << std::hex << bus;
    busStr = busSs.str();
    domain = (pciSlot >> 16) & 0xFFFF;
    domainSs << std::setw(4) << std::setfill('0') << std::hex << domain;
    domainStr = domainSs.str();

    offlineFileName =
        "/sys/bus/pci/devices/" + domainStr + ":" + busStr + ":" + deviceStr + "." + funcStr + "/dev_offline";
    if (!isFileExist(offlineFileName)) {
        logMsg(XRM_LOG_ERROR, "%s : %s is not existing\n", __func__, offlineFileName.c_str());
        return (status);
    }
    std::string offlineResultStr;
    std::ifstream offlineIfs(offlineFileName);
    if (offlineIfs.is_open()) {
        std::getline(offlineIfs, offlineResultStr);
        offlineIfs.close();
        status = std::stoi(offlineResultStr, nullptr, 0);
        logMsg(XRM_LOG_DEBUG, "%s : device [%d] offline status is %d\n", __func__, devId, status);
    }
    return (status);
}

/*
 * Should be called while holding lock
 *
 * To close the device, clean device handler
 */
int32_t xrm::system::closeDevice(int32_t devId) {
    int32_t ret = XRM_SUCCESS;
    if (m_devList[devId].deviceHandle) {
        logMsg(XRM_LOG_DEBUG, "%s : close device %d\n", __func__, devId);
        xclClose(m_devList[devId].deviceHandle);
        memset(&(m_devList[devId].deviceHandle), 0, sizeof(xclDeviceHandle));
        memset(&(m_devList[devId].deviceInfo), 0, sizeof(xclDeviceInfo2));
    } else {
        logMsg(XRM_LOG_ERROR, "%s : device handle is not valid\n", __func__);
        ret = XRM_ERROR;
    }
    return (ret);
}

/*
 * Should be called while holding lock
 *
 * To reset device:
 *    1) recycle all the resource used by all clients on this device.
 *    2) unload xclbin from this device.
 *    3) close this device.
 */
int32_t xrm::system::resetDevice(int32_t devId) {
    int32_t ret = XRM_SUCCESS;
    uint32_t i;
    if (m_devList[devId].deviceHandle) {
        logMsg(XRM_LOG_DEBUG, "%s : reset device %d\n", __func__, devId);

        /* clean all the clients which are using the device */
        std::vector<uint64_t> clientIdVect;
        std::vector<pid_t> clientProcessIdVect;
        if (m_devList[devId].isExcl) {
            if (m_devList[devId].clientProcs[0].clientId != 0) {
                clientIdVect.push_back(m_devList[devId].clientProcs[0].clientId);
                clientProcessIdVect.push_back(m_devList[devId].clientProcs[0].clientProcessId);
            }
        } else {
            for (i = 0; i < XRM_MAX_DEV_CLIENTS; i++) {
                if (m_devList[devId].clientProcs[i].clientId != 0) {
                    clientIdVect.push_back(m_devList[devId].clientProcs[i].clientId);
                    clientProcessIdVect.push_back(m_devList[devId].clientProcs[i].clientProcessId);
                }
            }
        }
        /* recycle all the resource used by the clients */
        for (i = 0; i < clientIdVect.size(); i++) {
            recycleResource(clientIdVect[i]);
        }
        /* send SIGBUS to kill the client processes */
        for (i = 0; i < clientProcessIdVect.size(); i++) {
            kill(clientProcessIdVect[i], SIGBUS);
        }

        /* unload xclbin from the device */
        std::string unloadErrmsg;
        unloadOneDevice(devId, unloadErrmsg);

        /* close the device, clean device handler */
        closeDevice(devId);
    } else {
        logMsg(XRM_LOG_ERROR, "%s : device handle is not valid\n", __func__);
        ret = XRM_ERROR;
    }
    return (ret);
}

/*
 * the load process need to be protected by lock
 * device id: 0 - (numDevice -1): the target device; -2: all devices on the system.
 */
int32_t xrm::system::loadDevices(pt::ptree& loadTree, std::string& errmsg) {
    int32_t ret = XRM_ERROR;
    std::string strDevId, xclbin;
    int32_t devId;
    std::string loadErrmsg;

    /*
     * XRM daemon may start before FPGA device ready during boot,
     * so try to init devices at the first time using devices.
     */
    if (!m_devicesInited) initDevices();

    for (auto it : loadTree) {
        auto kv = it.second;
        strDevId = kv.get<std::string>("device", "");
        if (strDevId.c_str()[0] == '\0') {
            errmsg += "request parameters device is not provided";
            ret = XRM_ERROR_INVALID;
            break;
        }
        devId = std::stoi(strDevId, nullptr, 0);
        if (devId < -2 || devId == -1 || devId >= m_numDevice) {
            errmsg += "device " + std::to_string(devId) + " not found";
            ret = XRM_ERROR_INVALID;
            break;
        }

        xclbin = kv.get<std::string>("xclbin", "");
        if (xclbin.c_str()[0] == '\0') {
            errmsg += "request parameters xclbin is not provided";
            ret = XRM_ERROR_INVALID;
            break;
        }

        if (devId == -2) {
            for (int32_t i = 0; i < m_numDevice; i++) {
                ret = deviceLoadXclbin(i, xclbin, loadErrmsg);
                if (ret != XRM_SUCCESS) {
                    errmsg += loadErrmsg;
                    break;
                }
            }
        } else {
            ret = deviceLoadXclbin(devId, xclbin, loadErrmsg);
            if (ret != XRM_SUCCESS) {
                errmsg += loadErrmsg;
                break;
            }
        }
    }
    return (ret);
}

/*
 * Load xclbin to any possible device
 *
 * success: return the device id loaded with new xclbin
 * fail: return the error code
 *
 * the operation should already within lock protection
 */
int32_t xrm::system::loadAnyDevice(int32_t* devId, std::string& xclbin, std::string& errmsg) {
    int32_t ret = XRM_ERROR;
    std::string loadErrmsg;

    /*
     * XRM daemon may start before FPGA device ready during boot,
     * so try to init devices at the first time using devices.
     */
    if (!m_devicesInited) initDevices();

    if (xclbin.c_str()[0] == '\0') {
        errmsg += "request parameters xclbin is not provided";
        ret = XRM_ERROR_INVALID;
        return (ret);
    }

    /* load xclbin to one device not loaded */
    for (int32_t i = 0; i < m_numDevice; i++) {
        if (!isDeviceLoaded(i)) {
            ret = deviceLoadXclbin(i, xclbin, loadErrmsg);
            if (ret == XRM_SUCCESS) {
                *devId = i;
                return (ret);
            }
        }
    }
    if (ret != XRM_SUCCESS) {
        /* load xclbin to one device not in use */
        for (int32_t i = 0; i < m_numDevice; i++) {
            if (!isDeviceBusy(i)) {
                if (unloadOneDevice(i, loadErrmsg) != XRM_SUCCESS) continue;
                ret = deviceLoadXclbin(i, xclbin, loadErrmsg);
                if (ret == XRM_SUCCESS) {
                    *devId = i;
                    return (ret);
                }
            }
        }
    }
    if (ret != XRM_SUCCESS) {
        errmsg += loadErrmsg;
    }
    return (ret);
}

/*
 * Load xclbin to one device
 *
 * success: return the device id loaded with new xclbin
 * fail: return the error code
 *
 * the load process need to be protected by lock
 */
int32_t xrm::system::loadOneDevice(pt::ptree& loadTree, std::string& errmsg) {
    int32_t ret = XRM_ERROR;
    std::string loadErrmsg;

    /*
     * XRM daemon may start before FPGA device ready during boot,
     * so try to init devices at the first time using devices.
     */
    if (!m_devicesInited) initDevices();

    auto xclbin = loadTree.get<std::string>("xclbinFileName");
    auto devId = loadTree.get<int32_t>("deviceId");
    logMsg(XRM_LOG_NOTICE, "%s(): devId is %d, xclbin file is %s\n", __func__, devId, xclbin.c_str());

    if (xclbin.c_str()[0] == '\0') {
        errmsg += "request parameters xclbin is not provided";
        ret = XRM_ERROR_INVALID;
        goto load_exit;
    }
    if (devId < -1 || devId >= m_numDevice) {
        errmsg += "device id is out of range";
        ret = XRM_ERROR_INVALID;
        goto load_exit;
    }

    if (devId != -1) {
        /* load xclbin to one specified device */
        ret = deviceLoadXclbin(devId, xclbin, loadErrmsg);
    } else {
        /* load xclbin to any possible device */
        ret = loadAnyDevice(&devId, xclbin, loadErrmsg);
    }
    if (ret != XRM_SUCCESS) {
        errmsg += loadErrmsg;
        goto load_exit;
    }

load_exit:
    if (ret == XRM_SUCCESS) /* return the device id if load is successful */
        return (devId);
    else
        /* return the error code if load fail */
        return (ret);
}

int32_t xrm::system::xclbinFileReadUuid(std::string& name, std::string& uuidStr, std::string& errmsg) {
    std::ifstream file(name.c_str(), std::ios::binary | std::ios::ate);
    if (!file.good()) {
        errmsg = "Failed to open xclbin file " + name;
        return (XRM_ERROR);
    }
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    char* memBuffer = (char*)malloc(size);
    if (memBuffer == NULL) {
        errmsg = "Failed to alloce buffer for xclbin file " + name;
        return (XRM_ERROR);
    }
    if (!file.read(memBuffer, size)) {
        errmsg = "Failed to read xclbin file " + name;
        free(memBuffer);
        return (XRM_ERROR);
    }

    axlf* xclbin = reinterpret_cast<axlf*>(memBuffer);
    uuid_t uuid;
    uuid_copy(uuid, xclbin->m_header.uuid);
    free(memBuffer);
    /* convert uuid to string */
    binToHexstr((unsigned char*)&uuid, sizeof(uuid_t), uuidStr);
    return (XRM_SUCCESS);
}

int32_t xrm::system::deviceLoadXclbin(int32_t devId, std::string& xclbin, std::string& errmsg) {
    if (devId < 0 || devId >= m_numDevice) {
        errmsg = "device id " + std::to_string(devId) + " is not found";
        errmsg += ", failed to load " + xclbin + " to device " + std::to_string(devId);
        return (XRM_ERROR_INVALID);
    }
    if (m_devList[devId].isDisabled) {
        errmsg = "device " + std::to_string(devId) + " is disabled";
        errmsg += ", failed to load " + xclbin + " to device " + std::to_string(devId);
        return (XRM_ERROR);
    }
    if (m_devList[devId].isLoaded) {
        std::string uuidStr;
        if (xclbinFileReadUuid(xclbin, uuidStr, errmsg) != XRM_SUCCESS) {
            errmsg += ", failed to load " + xclbin + " to device " + std::to_string(devId);
            return (XRM_ERROR);
        }
        if (uuidStr.compare(m_devList[devId].xclbinInfo.uuidStr) == 0) {
            /* target xclbin file has same uuid as the one already loaded to device */
            return (XRM_SUCCESS);
        } else {
            errmsg = "device " + std::to_string(devId) + " is already loaded with xclbin (" +
                     m_devList[devId].xclbinName + " uuid: " + m_devList[devId].xclbinInfo.uuidStr + ")";
            errmsg += ", failed to load " + xclbin + " (uuid: " + uuidStr + ") to device " + std::to_string(devId);
            return (XRM_ERROR);
        }
    }

    logMsg(XRM_LOG_NOTICE, "%s load xclbin file %s to device %d", __func__, xclbin.c_str(), devId);

    /* if the device is not opened yet, then need to open it at first */
    if (m_devList[devId].deviceHandle == 0) {
        openDevice(devId);
    }

    /* Read xclbin file */
    if (xclbinReadFile(devId, xclbin, errmsg) != XRM_SUCCESS) {
        errmsg += ", failed to load " + xclbin + " to device " + std::to_string(devId);
        deviceClearInfo(devId);
        return (XRM_ERROR);
    }

    /* Load xclbin to device */
    if (xclbinLoadToDevice(devId, errmsg) != XRM_SUCCESS) {
        errmsg += ", failed to load " + xclbin + " to device " + std::to_string(devId);
        deviceClearInfo(devId);
        return (XRM_ERROR);
    }

    /*
     * Get the information of xclbin on this device.
     * This function should be called after load because it will retrieve cuId which
     * depends on xclbin loading sucessful.
     */
    if (xclbinGetInfo(devId, errmsg) != XRM_SUCCESS) {
        errmsg += ", failed to load " + xclbin + " to device " + std::to_string(devId);
        deviceClearInfo(devId);
        return (XRM_ERROR);
    }

    /* Lock the loaded xclbin to the device so current downloaded xclbin will NOT be
     * replaced by other xclbin download process.
     */
    if (deviceLockXclbin(devId) != XRM_SUCCESS) {
        errmsg += "Unable to lock device, failed to load " + xclbin + " to device " + std::to_string(devId);
        deviceClearInfo(devId);
        return (XRM_ERROR);
    }

    /* Free the memory buffer */
    deviceFreeBuffer(devId);

    /* Update state */
    m_devList[devId].xclbinName = xclbin;
    m_devList[devId].isLoaded = true;

#if 0
    /* For testing only */
    deviceDumpResource(devId);
#endif

    return (XRM_SUCCESS);
}

void xrm::system::flushDevData(int32_t devId) {
    int i;

    if (devId >= 0 && devId < m_numDevice) {
        deviceData* dev = &m_devList[devId];
        xclbinInformation* xclbinInfo = &dev->xclbinInfo;
        cuData* cu;

        // fresh the dev struct
        dev->dsaName = "";
        dev->xclbinName = "";
        dev->devId = 0;

        dev->isLoaded = 0;
        dev->isExcl = 0;
        dev->memBuffer = NULL;
        memset(&dev->deviceHandle, 0, sizeof(xclDeviceHandle));
        memset(&dev->deviceInfo, 0, sizeof(xclDeviceInfo2));
        memset(dev->clientProcs, 0, sizeof(clientData) * XRM_MAX_DEV_CLIENTS);

        // fresh the xclbin info
        memset(&xclbinInfo->uuid, 0, sizeof(uuid_t));
        xclbinInfo->uuidStr = "";
        xclbinInfo->numCu = 0;
        xclbinInfo->numHardwareCu = 0;
        xclbinInfo->numSoftwareCu = 0;
        xclbinInfo->numMemBank = 0;
        xclbinInfo->numConnect = 0;
        memset(xclbinInfo->memTopologyList, 0, sizeof(memTopologyData) * XRM_MAX_DDR_MAP);
        memset(xclbinInfo->connectList, 0, sizeof(connectData) * XRM_MAX_CONNECTION_ENTRIES);

        // fresh the cuList
        for (i = 0; i < XRM_MAX_XILINX_KERNELS; i++) {
            cu = &xclbinInfo->cuList[i];
            cu->cuType = CU_NULL;
            cu->ipLayoutIndex = 0;
            cu->kernelName = "";
            cu->kernelAlias = "";
            cu->cuName = "";
            cu->instanceName = "";
            cu->kernelPluginFileName = "";
            cu->maxCapacity = 0;
            cu->baseAddr = 0;
            cu->membankId = 0;
            cu->membankType = 0;
            cu->membankSize = 0;
            cu->membankBaseAddr = 0;
            memset(cu->channels, 0, sizeof(channelData) * XRM_MAX_KERNEL_CHANNELS);
            cu->numChanInuse = 0;
            memset(cu->clients, 0, sizeof(uint64_t) * XRM_MAX_KERNEL_CHANNELS);
            cu->numClient = 0;
            cu->totalUsedLoadUnified = 0; // will set dev load when numCu is set
            cu->totalReservedLoadUnified = 0;
            cu->totalReservedUsedLoadUnified = 0;
            memset(cu->reserves, 0, sizeof(reserveData) * XRM_MAX_KERNEL_RESERVES);
            cu->numReserve = 0;
            cu->deviceId = devId;
        }
    }
}

bool xrm::system::deviceFreeBuffer(int32_t devId) {
    if (devId >= 0 && devId < m_numDevice) {
        deviceData* dev = &m_devList[devId];

        if (dev->memBuffer) {
            free(dev->memBuffer);
            dev->memBuffer = NULL;
        }
        return (true);
    } else {
        return (false);
    }
}

bool xrm::system::deviceClearInfo(int32_t devId) {
    std::string dsaName;
    xclDeviceHandle deviceHandle;
    xclDeviceInfo2 deviceInfo;

    if (devId >= 0 && devId < m_numDevice) {
        deviceData* dev = &m_devList[devId];

        // device is busy
        if (isDeviceBusy(devId)) {
            return (false);
        }

        if (dev->memBuffer) {
            free(dev->memBuffer);
            dev->memBuffer = NULL;
        }
        /* save the required info */
        dsaName = dev->dsaName;
        deviceHandle = dev->deviceHandle;
        deviceInfo = dev->deviceInfo;

        flushDevData(devId);

        /* restore the info */
        dev->devId = devId;
        dev->dsaName = dsaName;
        dev->deviceHandle = deviceHandle;
        dev->deviceInfo = deviceInfo;
        return (true);
    }
    return (false);
}

int32_t xrm::system::xclbinReadFile(int32_t devId, std::string& name, std::string& errmsg) {
    if (devId < 0 || devId >= m_numDevice) {
        errmsg = "device id " + std::to_string(devId) + " is not found";
        return (XRM_ERROR_INVALID);
    }
    std::ifstream file(name.c_str(), std::ios::binary | std::ios::ate);
    if (!file.good()) {
        errmsg = "Failed to open xclbin file " + name;
        return (XRM_ERROR);
    }
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    m_devList[devId].memBuffer = (char*)malloc(size);
    if (m_devList[devId].memBuffer == NULL) {
        errmsg = "Failed to alloce buffer for xclbin file " + name;
        return (XRM_ERROR);
    }
    if (!file.read(m_devList[devId].memBuffer, size)) {
        errmsg = "Failed to read xclbin file " + name;
        free(m_devList[devId].memBuffer);
        return (XRM_ERROR);
    }

    return (XRM_SUCCESS);
}

int32_t xrm::system::xclbinGetInfo(int32_t devId, std::string& errmsg) {
    if (devId < 0 || devId >= m_numDevice) {
        errmsg = "device id " + std::to_string(devId) + " is not found";
        return (XRM_ERROR_INVALID);
    }

    if (xclbinGetLayout(devId, errmsg) != XRM_SUCCESS) return (XRM_ERROR);
    if (xclbinGetUuid(devId, errmsg) != XRM_SUCCESS) return (XRM_ERROR);
    if (xclbinGetMemTopology(devId, errmsg) != XRM_SUCCESS) return (XRM_ERROR);
    if (xclbinGetConnectivity(devId, errmsg) != XRM_SUCCESS) return (XRM_ERROR);
    if (xclbinGetKeyvalues(devId, errmsg) != XRM_SUCCESS) return (XRM_ERROR);
    xclbinInformation* xclbinInfo = &m_devList[devId].xclbinInfo;
    for (int32_t cuIdx = 0; cuIdx < xclbinInfo->numCu; cuIdx++) {
        for (int32_t connectIdx = 0; connectIdx < xclbinInfo->numConnect; connectIdx++) {
            connectData* conn = &xclbinInfo->connectList[connectIdx];
            cuData* cu = &xclbinInfo->cuList[cuIdx];
            if (conn->ipLayoutIndex == cu->ipLayoutIndex) {
                memTopologyData* topology = &xclbinInfo->memTopologyList[conn->memDataIndex];
                cu->membankId = conn->memDataIndex;
                cu->membankType = topology->memType;
                cu->membankSize = topology->memSize;
                cu->membankBaseAddr = topology->memBaseAddress;
            }
        }
    }

    return (XRM_SUCCESS);
}

int32_t xrm::system::xclbinGetLayout(int32_t devId, std::string& errmsg) {
    if (devId < 0 || devId >= m_numDevice) {
        errmsg = "device id " + std::to_string(devId) + " is not found";
        return (XRM_ERROR_INVALID);
    }

    bool hardwareKernelExisting = false;
    deviceData* dev = &m_devList[devId];
    char* buffer = m_devList[devId].memBuffer;
    xclbinInformation* xclbinInfo = &dev->xclbinInfo;
    axlf* xclbin = reinterpret_cast<axlf*>(buffer);
    xclbinInfo->numCu = 0;
    xclbinInfo->numHardwareCu = 0;
    xclbinInfo->numSoftwareCu = 0;
    std::string cuName;
    std::string instanceName;

    const axlf_section_header* ipHdr = xclbin::get_axlf_section(xclbin, IP_LAYOUT);
    if (ipHdr) {
        /* Handle the IP Hardware Kernel */
        char* data = &buffer[ipHdr->m_sectionOffset];
        const ip_layout* ipl = reinterpret_cast<ip_layout*>(data);
        for (int32_t i = 0; i < ipl->m_count; i++) {
            if (ipl->m_ip_data[i].m_type != IP_KERNEL) continue;
            xclbinInfo->numCu++;
            xclbinInfo->numHardwareCu++;
            if (xclbinInfo->numCu > XRM_MAX_XILINX_KERNELS) {
                errmsg = "cu number in xclbin is out range of " + std::to_string(XRM_MAX_XILINX_KERNELS);
                return (XRM_ERROR);
            }
            /* For hardware kernel, here should use the XRT interface to retrive cuId. */
            cuName = std::string((char*)ipl->m_ip_data[i].m_name);
            logMsg(XRM_LOG_NOTICE, "%s : cuName is %s\n", __func__, cuName.c_str());

            int32_t cuId = wrapIPName2Index(dev->deviceHandle, cuName.c_str());
            cuData* cu;
            if (cuId < 0) {
                if (cuId == XRM_ERROR_INVALID) {
                    // the function in XRT lib is NOT implemented, handle the cuId by XRM itself
                    cu = &xclbinInfo->cuList[xclbinInfo->numCu - 1];
                    cu->cuId = xclbinInfo->numCu - 1;
                } else {
                    // the function in XRT lib is implemented, return result is error
                    logMsg(XRM_LOG_ERROR, "%s : failed to get cu id of %s, ret = %d\n", __func__, cuName.c_str(), cuId);
                    return (XRM_ERROR);
                }
            } else {
                // the function in XRT lib is implemented, return result is good
                cu = &xclbinInfo->cuList[cuId];
                logMsg(XRM_LOG_NOTICE, "%s : cu id of %s is %d\n", __func__, cuName.c_str(), cuId);
                cu->cuId = cuId;
            }
            cu->ipLayoutIndex = xclbinInfo->numCu - 1; // the static index will be index of m_ip_data in xclbin file

            cu->cuType = CU_IPKERNEL;
            // cuName (m_name in m_ip_data) is "kerenlName:instanceName"
            cu->cuName = cuName;
            std::vector<std::string> results;
            // retrieve the kernelName
            boost::split(results, cu->cuName, [](char c) { return c == ':'; });
            cu->kernelName = results[0];
            instanceName = cuName.erase(0, cu->kernelName.length() + 1);
            cu->instanceName = instanceName;
            cu->baseAddr = ipl->m_ip_data[i].m_base_address;
            cuInitChannels(cu);
            cu->numClient = 0;
            cu->totalUsedLoadUnified = 0; // update dev load at end of loop
            cu->totalReservedLoadUnified = 0;
            cu->totalReservedUsedLoadUnified = 0;
            memset(cu->reserves, 0, sizeof(reserveData) * XRM_MAX_KERNEL_RESERVES);
            cu->numReserve = 0;
            cu->deviceId = devId;
            hardwareKernelExisting = true;
        } // end of hardware kernel handling
        /*
         * After all hardware kernel handled, it's assuming that all the previous cuId
         * is in range from 0 - current (xclbinInfo->numCu - 1).
         */

        /* Handle the Soft Kernel */
        for (const axlf_section_header* softKernelHdr = xclbin::get_axlf_section(xclbin, SOFT_KERNEL);
             softKernelHdr != NULL; softKernelHdr = xclbin::get_axlf_section_next(xclbin, softKernelHdr, SOFT_KERNEL)) {
            char* data = &buffer[softKernelHdr->m_sectionOffset];
            const soft_kernel* sk = reinterpret_cast<soft_kernel*>(data);
            if ((xclbinInfo->numCu + sk->m_num_instances) > XRM_MAX_XILINX_KERNELS) {
                errmsg = "soft kernel number in xclbin is out range of " + std::to_string(XRM_MAX_XILINX_KERNELS);
                return (XRM_ERROR);
            }

            for (uint32_t i = 0; i < sk->m_num_instances; i++) {
                xclbinInfo->numCu++;
                xclbinInfo->numSoftwareCu++;
                cuData* cu = &xclbinInfo->cuList[xclbinInfo->numCu - 1];
                cu->cuId = xclbinInfo->numCu - 1;
                cu->ipLayoutIndex = xclbinInfo->numCu - 1; // the static index will be index of m_ip_data in xclbin file
                cu->cuType = CU_SOFTKERNEL;
                /*
                 * mpo_name in soft_kernel is just "kernelName", but not "kernelName:instanceName"
                 * since instanceName of soft kernel is not descripted in xclbin, so cuName is same as kernelName.
                 */
                cu->kernelName = std::string((char*)&buffer[softKernelHdr->m_sectionOffset + sk->mpo_name]);
                cu->cuName = cu->kernelName;
                logMsg(XRM_LOG_NOTICE, "%s soft kernel name: %s\n", __func__, cu->kernelName.c_str());
                cuInitChannels(cu);
                cu->numClient = 0;
                cu->totalUsedLoadUnified = 0;
                cu->totalReservedLoadUnified = 0;
                cu->totalReservedUsedLoadUnified = 0;
                memset(cu->reserves, 0, sizeof(reserveData) * XRM_MAX_KERNEL_RESERVES);
                cu->numReserve = 0;
                cu->deviceId = devId;
            }
        } // end of soft kernel handling
    }
    updateDeviceLoad(devId, 0, 0);
    /* the xclbin should contain at least one hardware kernel */
    if (hardwareKernelExisting) {
        return (XRM_SUCCESS);
    } else {
        errmsg = "No hardware kernel existing in xclbin";
        return (XRM_ERROR);
    }
}

void xrm::system::cuInitChannels(cuData* cu) {
    int32_t i;

    if (cu == NULL) {
        logMsg(XRM_LOG_ERROR, "%s : init channels, the cu is NULL\n", __func__);
        return;
    }

    for (i = 0; i < XRM_MAX_KERNEL_CHANNELS; i++) {
        cu->channels[i].channelId = i;
        cu->channels[i].allocServiceId = 0;
        cu->channels[i].poolId = 0;
        cu->channels[i].clientId = 0;
        cu->channels[i].clientProcessId = 0;
        cu->channels[i].channelLoadUnified = 0;
        cu->channels[i].channelLoadOriginal = 0;
    }
    cu->numChanInuse = 0;
}

/*
 * ret: first free channel id,
 *    : -1, no free channel
 */
int32_t xrm::system::cuFindFreeChannelId(cuData* cu) {
    int32_t i;
    int32_t ret = -1;

    if (cu == NULL) return (ret);

    if (cu->numChanInuse >= XRM_MAX_KERNEL_CHANNELS) return (ret);

    for (i = 0; i < XRM_MAX_KERNEL_CHANNELS; i++) {
        /* channelLoadUnified is 0, clientId are also 0 */
        if (cu->channels[i].channelLoadUnified == 0) {
            ret = i;
            break;
        }
    }
    return (ret);
}

/*
 * To convert bin array to hex string.
 */
void xrm::system::binToHexstr(unsigned char* in, int32_t insz, std::string& outStr) {
    unsigned char* pin = in;
    const char* hex = "0123456789abcdef";
    for (; pin < (in + insz); pin++) {
        outStr = outStr + hex[(*pin >> 4) & 0xF];
        outStr = outStr + hex[*pin & 0xF];
    }
}

/*
 * To conver hex string like "0123456789abcdef" to bin 0123456789abcdef
 */
void xrm::system::hexstrToBin(std::string& inStr, int32_t insz, unsigned char* out) {
    char in[insz];
    unsigned char* pout = out;
    unsigned char hex_0, hex_1;
    strncpy(in, inStr.c_str(), insz);
    for (int32_t i = 0; i < insz; i += 2, pout++) {
        if (in[i] < 'a')
            hex_0 = (in[i] - '0');
        else
            hex_0 = (in[i] - 'a') + 10;
        if (in[i + 1] < 'a')
            hex_1 = (in[i + 1] - '0');
        else
            hex_1 = (in[i + 1] - 'a') + 10;
        *pout = ((hex_0 << 4) & 0xF0) | (hex_1 & 0xF);
    }
}

int32_t xrm::system::xclbinGetUuid(int32_t devId, std::string& errmsg) {
    if (devId < 0 || devId >= m_numDevice) {
        errmsg = "device id " + std::to_string(devId) + " is not found";
        return (XRM_ERROR_INVALID);
    }

    axlf* xclbin = reinterpret_cast<axlf*>(m_devList[devId].memBuffer);
    uuid_copy(m_devList[devId].xclbinInfo.uuid, xclbin->m_header.uuid);
    /* convert uuid to string */
    binToHexstr((unsigned char*)&m_devList[devId].xclbinInfo.uuid, sizeof(uuid_t), m_devList[devId].xclbinInfo.uuidStr);
    return (XRM_SUCCESS);
}

int32_t xrm::system::xclbinGetKeyvalues(int32_t devId, std::string& errmsg) {
    if (devId < 0 || devId >= m_numDevice) {
        errmsg = "device id " + std::to_string(devId) + " is not found";
        return (XRM_ERROR_INVALID);
    }

    deviceData* dev = &m_devList[devId];
    char* buffer = m_devList[devId].memBuffer;
    axlf* xclbin = reinterpret_cast<axlf*>(buffer);
    const axlf_section_header* kvHdr = xclbin::get_axlf_section(xclbin, KEYVALUE_METADATA);
    if (kvHdr) {
        auto size = kvHdr->m_sectionSize;
        char* data = &buffer[kvHdr->m_sectionOffset];
        if (size == 0 || data == NULL) {
            logMsg(XRM_LOG_NOTICE, "%s : There is no Keyvalues metadata in xclbin file\n", __func__);
            return (XRM_SUCCESS);
        }
        std::string kvData(data, size);

        // Fix-up the metadata string
        boost::erase_all(kvData, "\\");
        boost::replace_all(kvData, "\"{", "{");
        boost::replace_all(kvData, "}\"", "}");

        std::stringstream kvStr;
        kvStr << kvData;

        pt::ptree kvTree;
        try {
            pt::read_json(kvStr, kvTree);
        } catch (const boost::property_tree::json_parser_error& e) {
            errmsg = "Keyvalues metadata Json format is not right: " + e.message();
            return (XRM_ERROR_INVALID);
        }

        /*
         * The kernel name is unique in system, so xrm can identify the kernel with the name.
         */
        auto keyValues = kvTree.get_child("key_values");
        for (const auto& kviter : keyValues) {
            std::string kernelName = kviter.second.get<std::string>("key", "");
            if (kernelName.c_str()[0] == '\0') continue;
            logMsg(XRM_LOG_NOTICE, "%s : kernel name in metadata is %s\n", __func__, kernelName.c_str());
            auto value = kviter.second.get_child("value");
            std::string alias = value.get("alias", "");
            logMsg(XRM_LOG_NOTICE, "%s : alias in metadata is %s\n", __func__, alias.c_str());
            std::string kernelPluginFileName = value.get("plugin", "");
            logMsg(XRM_LOG_NOTICE, "%s : kernelPluginFileName in metadata is %s\n", __func__,
                   kernelPluginFileName.c_str());
            std::string maxCapacity = value.get("maxCapacity", "");
            logMsg(XRM_LOG_NOTICE, "%s : maxCapacity in metadata is %s\n", __func__, maxCapacity.c_str());
            for (int32_t i = 0; i < dev->xclbinInfo.numCu; i++) {
                cuData* cu = &dev->xclbinInfo.cuList[i];
                logMsg(XRM_LOG_NOTICE, "%s : kernel name in IP Layout is %s\n", __func__, cu->kernelName.c_str());
                if (kernelName.compare(cu->kernelName) == 0) {
                    logMsg(XRM_LOG_NOTICE, "%s : matching the kernel name\n", __func__);
                    if (alias.c_str()[0] != '\0') cu->kernelAlias = alias;
                    if (kernelPluginFileName.c_str()[0] != '\0') cu->kernelPluginFileName = kernelPluginFileName;
                    if (maxCapacity.c_str()[0] != '\0') cu->maxCapacity = std::strtoull(maxCapacity.c_str(), NULL, 0);
                }
            }
        }
        logMsg(XRM_LOG_NOTICE, "%s : successed handling key value\n", __func__);
        return (XRM_SUCCESS);
    } else {
        logMsg(XRM_LOG_NOTICE, "%s : There is no metadata session in the xclbin file.\n", __func__);
        return (XRM_SUCCESS);
    }
}

int32_t xrm::system::xclbinGetMemTopology(int32_t devId, std::string& errmsg) {
    if (devId < 0 || devId >= m_numDevice) {
        errmsg = "device id " + std::to_string(devId) + " is not found";
        return (XRM_ERROR_INVALID);
    }

    deviceData* dev = &m_devList[devId];
    char* buffer = m_devList[devId].memBuffer;
    axlf* xclbin = reinterpret_cast<axlf*>(buffer);

    const axlf_section_header* ipHdr = xclbin::get_axlf_section(xclbin, MEM_TOPOLOGY);
    if (ipHdr) {
        char* data = &buffer[ipHdr->m_sectionOffset];
        const mem_topology* memTopo = reinterpret_cast<mem_topology*>(data);
        dev->xclbinInfo.numMemBank = memTopo->m_count;
        logMsg(XRM_LOG_NOTICE, "MEM TOPOLOGY - %d banks\n", dev->xclbinInfo.numMemBank);
        for (int i = 0; i < memTopo->m_count; i++) {
            memTopologyData* topology = &dev->xclbinInfo.memTopologyList[i];
            topology->memType = memTopo->m_mem_data[i].m_type;
            topology->memUsed = memTopo->m_mem_data[i].m_used;
            topology->memSize = memTopo->m_mem_data[i].m_size;
            topology->memBaseAddress = memTopo->m_mem_data[i].m_base_address;
            memcpy(topology->memTag, memTopo->m_mem_data[i].m_tag, XRM_MAX_DDR_MAP * sizeof(unsigned char));
            logMsg(XRM_LOG_NOTICE, "index=%d, tag=%s, type = %d, used = %d, size = %lx, base = %lx\n", i,
                   topology->memTag, topology->memType, topology->memUsed, topology->memSize, topology->memBaseAddress);
        }
    } else {
        logMsg(XRM_LOG_NOTICE, "Could not find MEM_TOPOLOGY in xclbin");
    }

    return (XRM_SUCCESS);
}

int32_t xrm::system::xclbinGetConnectivity(int32_t devId, std::string& errmsg) {
    if (devId < 0 || devId >= m_numDevice) {
        errmsg = "device id " + std::to_string(devId) + " is not found";
        return (XRM_ERROR_INVALID);
    }

    deviceData* dev = &m_devList[devId];
    char* buffer = m_devList[devId].memBuffer;
    axlf* xclbin = reinterpret_cast<axlf*>(buffer);

    const axlf_section_header* ipHdr = xclbin::get_axlf_section(xclbin, CONNECTIVITY);
    if (ipHdr) {
        char* data = &buffer[ipHdr->m_sectionOffset];
        const connectivity* axlfConn = reinterpret_cast<connectivity*>(data);
        dev->xclbinInfo.numConnect = axlfConn->m_count;
        logMsg(XRM_LOG_NOTICE, "CONNECTIVITY - %d connections\n", dev->xclbinInfo.numConnect);
        for (int i = 0; i < axlfConn->m_count; i++) {
            connectData* conn = &dev->xclbinInfo.connectList[i];
            conn->argIndex = axlfConn->m_connection[i].arg_index;
            conn->ipLayoutIndex = axlfConn->m_connection[i].m_ip_layout_index;
            conn->memDataIndex = axlfConn->m_connection[i].mem_data_index;
            logMsg(XRM_LOG_NOTICE, "index = %d, arg_idx = %d, ip_idx = %d, mem_idx = %d\n", i, conn->argIndex,
                   conn->ipLayoutIndex, conn->memDataIndex);
        }
    } else {
        logMsg(XRM_LOG_NOTICE, "Could not find CONNECTIVITY in xclbin");
    }

    return (XRM_SUCCESS);
}

int32_t xrm::system::xclbinLoadToDevice(int32_t devId, std::string& errmsg) {
    if (devId < 0 || devId >= m_numDevice) {
        errmsg = "device id " + std::to_string(devId) + " is not found";
        return (XRM_ERROR_INVALID);
    }

    logMsg(XRM_LOG_NOTICE, "%s load xclbin to device %d", __func__, devId);

    int32_t rc;
    deviceData* dev = &m_devList[devId];
    char* buffer = m_devList[devId].memBuffer;
    xclDeviceHandle deviceHandle = dev->deviceHandle;
    rc = wrapLockDevice(deviceHandle);
    if (rc != 0) {
        errmsg = "Failed to lock device " + std::to_string(devId);
        return (rc);
    }
    rc = xclLoadXclBin(deviceHandle, (const xclBin*)buffer);
    if (rc != 0) errmsg = "xclLoadXclBin failed, rc = " + std::to_string(rc);
    wrapUnlockDevice(deviceHandle);
    return (rc);
}

int32_t xrm::system::deviceLockXclbin(int32_t devId) {
    int32_t ret = XRM_SUCCESS;
    if (devId < 0 || devId >= m_numDevice) {
        return (XRM_ERROR_INVALID);
    }

    deviceData* dev = &m_devList[devId];
    xclbinInformation* xclbinInfo = &m_devList[devId].xclbinInfo;

    /* To use xclOpenContext() with ipIndex as -1 to ocupie the resouce so that xclLoadXclBin()
     * can NOT load new xclbin to replace current loaded xclbin.
     */
    ret = xclOpenContext(dev->deviceHandle, xclbinInfo->uuid, -1, true);
    if (ret != 0) {
        logMsg(XRM_LOG_ERROR, "%s Failed to lock down xclbin to device %d", __func__, devId);
        return (ret);
    }
    return (XRM_SUCCESS);
}

int32_t xrm::system::deviceUnlockXclbin(int32_t devId) {
    int32_t ret = XRM_SUCCESS;
    if (devId < 0 || devId >= m_numDevice) {
        return (XRM_ERROR_INVALID);
    }

    deviceData* dev = &m_devList[devId];
    if (!dev->isLoaded) return (XRM_SUCCESS);

    xclbinInformation* xclbinInfo = &m_devList[devId].xclbinInfo;

    /* To use xclCloseContext() with ipIndex as -1 to release the resouce so that xclLoadXclBin()
     * can load new xclbin to replace current loaded xclbin.
     */
    ret = xclCloseContext(dev->deviceHandle, xclbinInfo->uuid, -1);
    if (ret != 0) {
        logMsg(XRM_LOG_ERROR, "%s Failed to unlock xclbin from device %d", __func__, devId);
        return (ret);
    }
    return (XRM_SUCCESS);
}

void xrm::system::deviceDumpResource(int32_t devId) {
    int32_t i, j;
    if (devId < 0 || devId >= m_numDevice) return;

    deviceData* dev = &m_devList[devId];
    xclbinInformation* xclbinInfo = &m_devList[devId].xclbinInfo;

    logMsg(XRM_LOG_NOTICE, "%s(): deviceId : %d", __func__, devId);
    logMsg(XRM_LOG_NOTICE, "%s(): isDisabled : %d", __func__, dev->isDisabled);
    logMsg(XRM_LOG_NOTICE, "%s(): isLoaded : %d", __func__, dev->isLoaded);
    if (!dev->isLoaded) return;
    logMsg(XRM_LOG_NOTICE, "%s(): uuid : %s", __func__, xclbinInfo->uuidStr.c_str());
#if 0
    for (i = 0; i < XRM_MAX_DEV_CLIENTS; i++)
    {
        logMsg(XRM_LOG_NOTICE, "%s() dev[%d] clientProcs[%d] : clientId: %lu, ref: %d",
               __func__, devId, i, dev->clientProcs[i].clientId,
               dev->clientProcs[i].ref);
    }
#endif
    logMsg(XRM_LOG_NOTICE, "%s(): numCu : %d", __func__, xclbinInfo->numCu);
    logMsg(XRM_LOG_NOTICE, "%s(): numHardwareCu : %d", __func__, xclbinInfo->numHardwareCu);
    logMsg(XRM_LOG_NOTICE, "%s(): numSoftwareCu : %d", __func__, xclbinInfo->numSoftwareCu);
    for (i = 0; i < xclbinInfo->numCu; i++) {
        cuData* cu = &xclbinInfo->cuList[i];
        logMsg(XRM_LOG_NOTICE, "%s() cu : cuId :%d", __func__, cu->cuId);
        logMsg(XRM_LOG_NOTICE, "%s() cu : ipLayoutIndex :%d", __func__, cu->ipLayoutIndex);
        logMsg(XRM_LOG_NOTICE, "%s() cu : cuType :%d", __func__, cu->cuType);
        logMsg(XRM_LOG_NOTICE, "%s() cu : kernelName :%s", __func__, cu->kernelName.c_str());
        logMsg(XRM_LOG_NOTICE, "%s() cu : kernelAlias :%s", __func__, cu->kernelAlias.c_str());
        logMsg(XRM_LOG_NOTICE, "%s() cu : cuName :%s", __func__, cu->cuName.c_str());
        logMsg(XRM_LOG_NOTICE, "%s() cu : instanceName :%s", __func__, cu->instanceName.c_str());
        logMsg(XRM_LOG_NOTICE, "%s() cu : baseAddr :%lu", __func__, cu->baseAddr);
        logMsg(XRM_LOG_NOTICE, "%s() cu : membankId :%d", __func__, cu->membankId);
        logMsg(XRM_LOG_NOTICE, "%s() cu : membankType :%d", __func__, cu->membankId);
        logMsg(XRM_LOG_NOTICE, "%s() cu : membankSize :%lu", __func__, cu->membankSize);
        logMsg(XRM_LOG_NOTICE, "%s() cu : membankBaseAddr :%lu", __func__, cu->membankBaseAddr);
        logMsg(XRM_LOG_NOTICE, "%s() cu : kernelPluginFileName :%s", __func__, cu->kernelPluginFileName.c_str());
        logMsg(XRM_LOG_NOTICE, "%s() cu : maxCapacity :%lu", __func__, cu->maxCapacity);
        logMsg(XRM_LOG_NOTICE, "%s() cu : numChanInuse :%d", __func__, cu->numChanInuse);
        logMsg(XRM_LOG_NOTICE, "%s() cu : numReserve :%d", __func__, cu->numReserve);
        logMsg(XRM_LOG_NOTICE, "%s() cu : totalUsedLoadUnified :%d", __func__, cu->totalUsedLoadUnified);
        logMsg(XRM_LOG_NOTICE, "%s() cu : totalReservedLoadUnified :%d", __func__, cu->totalReservedLoadUnified);
        for (j = 0; j < XRM_MAX_KERNEL_CHANNELS; j++) {
            channelData* chan = &cu->channels[j];
            logMsg(XRM_LOG_NOTICE, "%s() cu : clients[%d] : %d", __func__, j, cu->clients[j]);
            logMsg(XRM_LOG_NOTICE, "%s() cu : channels[%d] channelId: %d", __func__, j, chan->channelId);
            logMsg(XRM_LOG_NOTICE, "%s() cu : channels[%d] allocServiceId: %lu", __func__, j, chan->allocServiceId);
            logMsg(XRM_LOG_NOTICE, "%s() cu : channels[%d] poolId: %lu", __func__, j, chan->poolId);
            logMsg(XRM_LOG_NOTICE, "%s() cu : channels[%d] clientId: %lu", __func__, j, chan->clientId);
            logMsg(XRM_LOG_NOTICE, "%s() cu : channels[%d] clientProcessId: %lu", __func__, j, chan->clientProcessId);
            logMsg(XRM_LOG_NOTICE, "%s() cu : channels[%d] channelLoadUnified: %d", __func__, j,
                   chan->channelLoadUnified);
            logMsg(XRM_LOG_NOTICE, "%s() cu : channels[%d] channelLoadOriginal: %d", __func__, j,
                   chan->channelLoadOriginal);
        }
        cu->totalReservedUsedLoadUnified = 0;
        for (j = 0; j < XRM_MAX_KERNEL_RESERVES; j++) {
            reserveData* reserve = &cu->reserves[j];
            logMsg(XRM_LOG_NOTICE, "%s() cu : reserves[%d] reservePoolId: %lu", __func__, j, reserve->reservePoolId);
            logMsg(XRM_LOG_NOTICE, "%s() cu : reserves[%d] reserveLoadUnified: %d", __func__, j,
                   reserve->reserveLoadUnified);
            logMsg(XRM_LOG_NOTICE, "%s() cu : reserves[%d] reserveUsedLoadUnified: %d", __func__, j,
                   reserve->reserveUsedLoadUnified);
            cu->totalReservedUsedLoadUnified += reserve->reserveUsedLoadUnified;
            logMsg(XRM_LOG_NOTICE, "%s() cu : reserves[%d] clientIsActive: %d", __func__, j, reserve->clientIsActive);
            logMsg(XRM_LOG_NOTICE, "%s() cu : reserves[%d] clientId: %lu", __func__, j, reserve->clientId);
            logMsg(XRM_LOG_NOTICE, "%s() cu : reserves[%d] clientProcessId: %lu", __func__, j,
                   reserve->clientProcessId);
        }
        logMsg(XRM_LOG_NOTICE, "%s() cu : totalReservedUsedLoadUnified :%d", __func__,
               cu->totalReservedUsedLoadUnified);
    }
}

/*
 * To get the cu max capacity based on kernel name or alias or both
 *
 * return:
 *        0: cu not specified or not found
 *        > 0: max capacity of the cu
 */
uint64_t xrm::system::getCuMaxCapacity(cuProperty* cuProp) {
    if (cuProp == NULL) return (0);

    uint64_t maxCapacity = 0;
    uint64_t maxCapacityWithAlias = 0;
    std::string name = cuProp->kernelName;
    std::string alias = cuProp->kernelAlias;
    deviceData* dev;
    cuData* cu;
    int32_t devId, cuId;
    bool found;

    if ((name[0] == '\0') && (alias[0] == '\0')) {
        logMsg(XRM_LOG_ERROR, "%s : neither name nor alias are presented", __func__);
        return (0);
    }

    if (name[0] != '\0') {
        /* kernel name is presented */
        found = false;
        for (devId = 0; devId < m_numDevice; devId++) {
            dev = &m_devList[devId];
            if (dev->isLoaded) {
                for (cuId = 0; cuId < dev->xclbinInfo.numCu; cuId++) {
                    cu = &dev->xclbinInfo.cuList[cuId];
                    if (name.compare(cu->kernelName.c_str()) == 0) {
                        found = true;
                        maxCapacity = cu->maxCapacity;
                        break;
                    }
                }
            }
            if (found) break;
        }
        if (found) {
            logMsg(XRM_LOG_NOTICE, "%s : maxCapacity with name is %lu", __func__, maxCapacity);
        } else {
            logMsg(XRM_LOG_NOTICE, "%s : cu name%s not found", __func__, name.c_str());
        }

        /* kernel alias is also presented */
        if (alias[0] != '\0') {
            found = false;
            for (devId = 0; devId < m_numDevice; devId++) {
                dev = &m_devList[devId];
                if (dev->isLoaded) {
                    for (cuId = 0; cuId < dev->xclbinInfo.numCu; cuId++) {
                        cu = &dev->xclbinInfo.cuList[cuId];
                        if (alias.compare(cu->kernelAlias.c_str()) == 0) {
                            found = true;
                            maxCapacityWithAlias = cu->maxCapacity;
                            break;
                        }
                    }
                }
                if (found) break;
            }
            if (found) {
                logMsg(XRM_LOG_NOTICE, "%s : maxCapacity with alias is %lu", __func__, maxCapacityWithAlias);
            } else {
                logMsg(XRM_LOG_NOTICE, "%s : cu alias%s not found", __func__, alias.c_str());
            }

            if (maxCapacity != maxCapacityWithAlias) {
                logMsg(XRM_LOG_NOTICE, "%s : kernel name and alias mismatch", __func__);
                maxCapacity = 0;
            }
        }
    } else // alias[0] != '\0'
    {
        /* kernel alias is presented */
        found = false;
        for (devId = 0; devId < m_numDevice; devId++) {
            dev = &m_devList[devId];
            if (dev->isLoaded) {
                for (cuId = 0; cuId < dev->xclbinInfo.numCu; cuId++) {
                    cu = &dev->xclbinInfo.cuList[cuId];
                    if (alias.compare(cu->kernelAlias.c_str()) == 0) {
                        found = true;
                        maxCapacityWithAlias = cu->maxCapacity;
                        break;
                    }
                }
            }
            if (found) break;
        }
        if (found) {
            logMsg(XRM_LOG_NOTICE, "%s : maxCapacity with alias is %lu", __func__, maxCapacityWithAlias);
        } else {
            logMsg(XRM_LOG_NOTICE, "%s : cu alias%s not found", __func__, alias.c_str());
        }
        maxCapacity = maxCapacityWithAlias;
    }

    return (maxCapacity);
}

void xrm::system::initLock() {
    pthread_mutex_init(&m_lock, NULL);
}

/*
 * Considering the xrmadm cli and xrm lib are using asio to do the IPC with xrmd deamon,
 * So the daemon is running as a single process there is no need to use lock in xrmd
 * daemon to protect the resource pool data.
 */
void xrm::system::enterLock() {
#if 1
    pthread_mutex_lock(&m_lock);
#endif
}

void xrm::system::exitLock() {
#if 1
    pthread_mutex_unlock(&m_lock);
#endif
}

inline void xrm::system::save() {
    std::string fname = "/dev/shm/xrm.data";
    /* create and open the character archive for output */
    std::ofstream ofs(fname.c_str());
    /* write class instance to archive */
    boost::archive::text_oarchive ar(ofs);
    ar&* this;
}

/*
 * call while holding lock
 */
bool xrm::system::restore() {
    bool rc = false;

    std::string fname = "/dev/shm/xrm.data";
    /* create and open the archive for input */
    std::ifstream ifs(fname.c_str());
    if (ifs.is_open()) {
        logMsg(XRM_LOG_NOTICE, "Database found, reloading daemon");
        /* read class from archive */
        boost::archive::text_iarchive ar(ifs);
        ar&* this;
        initConfig();
        initLibVersionDepFunctions();
        for (int32_t devId = 0; devId < m_numDevice; devId++) {
            if (!m_devList[devId].isDisabled) openDevice(devId);
        }
        rc = true;
    } else
        logMsg(XRM_LOG_NOTICE, "No database found, starting from fresh state");

    return (rc);
}

/* input parameters valid check need to be done before the calling */
bool xrm::system::isDeviceLoaded(int32_t devId) {
    if (devId >= 0 && devId < m_numDevice) {
        deviceData* dev = &m_devList[devId];
        return (dev->isLoaded);
    } else {
        return (false);
    }
}

/* input parameters valid check need to be done before the calling */
bool xrm::system::isDeviceLoadedXclbinWithUuid(int32_t devId, uuid_t uuid) {
    if (devId >= 0 && devId < m_numDevice) {
        deviceData* dev = &m_devList[devId];
        bool sameUuid = true;
        size_t size = sizeof(uuid_t);
        unsigned char* uuidOfDev = dev->xclbinInfo.uuid;

        for (size_t i = 0; i < size; i++) {
            if ((uint8_t)(uuidOfDev[i]) != (uint8_t)(uuid[i])) {
                sameUuid = false;
                break;
            }
        }
        return (sameUuid);
    } else {
        return (false);
    }
}

/* input parameters valid check need to be done before the calling */
bool xrm::system::isDeviceBusy(int32_t devId) {
    if (devId >= 0 && devId < m_numDevice) {
        deviceData* dev = &m_devList[devId];

        for (int32_t cuId = 0; cuId < dev->xclbinInfo.numCu; cuId++) {
            if (dev->xclbinInfo.cuList[cuId].totalUsedLoadUnified) return (true);
        }
        return (false);
    } else {
        return (false);
    }
}

/*
 * the unload process should be protected by system lock
 */
int32_t xrm::system::unloadOneDevice(const int32_t& devId, std::string& errmsg) {
    /*
     * XRM daemon may start before FPGA device ready during boot,
     * so try to init devices at the first time using devices.
     */
    if (!m_devicesInited) initDevices();

    if (devId >= 0 && devId < m_numDevice) {
        deviceData* dev = &m_devList[devId];

        if (!dev->isLoaded) {
            /*
             * if device is not loaded with xclbin, then the device will
             * still keep unload state.
             */
            deviceClearInfo(devId);
            return (XRM_SUCCESS);
        }

        // device is busy
        if (isDeviceBusy(devId)) {
            errmsg = "Device  " + std::to_string(devId) + " is busy";
            return (XRM_ERROR_DEVICE_IS_BUSY);
        }

        // unlock loaded xclbin from device
        if (deviceUnlockXclbin(devId)) {
            errmsg = "Fail to unlock xclbin from device  " + std::to_string(devId);
            return (XRM_ERROR_DEVICE_IS_LOCKED);
        }

        deviceClearInfo(devId);
        return (XRM_SUCCESS);
    }
    errmsg = "Invalid device id [" + std::to_string(devId) + "] passed in";
    return (XRM_ERROR_INVALID);
}

/*
 * the device enable process should be protected by system lock
 */
int32_t xrm::system::enableOneDevice(const int32_t& devId, std::string& errmsg) {
    /*
     * XRM daemon may start before FPGA device ready during boot,
     * so try to init devices at the first time using devices.
     */
    if (!m_devicesInited) initDevices();

    if (devId >= 0 && devId < m_numDevice) {
        deviceData* dev = &m_devList[devId];

        dev->isDisabled = false;
        return (XRM_SUCCESS);
    }
    errmsg = "Invalid device id [" + std::to_string(devId) + "] passed in";
    return (XRM_ERROR_INVALID);
}

/*
 * the device disable process should be protected by system lock
 */
int32_t xrm::system::disableOneDevice(const int32_t& devId, std::string& errmsg) {
    /*
     * XRM daemon may start before FPGA device ready during boot,
     * so try to init devices at the first time using devices.
     */
    if (!m_devicesInited) initDevices();

    if (devId >= 0 && devId < m_numDevice) {
        deviceData* dev = &m_devList[devId];

        if (dev->isDisabled) {
            return (XRM_SUCCESS);
        }
        if (!dev->isLoaded) {
            dev->isDisabled = true;
            return (XRM_SUCCESS);
        }

        // device is busy
        if (isDeviceBusy(devId)) {
            errmsg = "Device  " + std::to_string(devId) + " is busy";
            return (XRM_ERROR_DEVICE_IS_BUSY);
        }

        // unlock loaded xclbin from device
        if (deviceUnlockXclbin(devId)) {
            errmsg = "Fail to unlock xclbin from device  " + std::to_string(devId);
            return (XRM_ERROR_DEVICE_IS_LOCKED);
        }

        deviceClearInfo(devId);
        dev->isDisabled = true;
        return (XRM_SUCCESS);
    }
    errmsg = "Invalid device id [" + std::to_string(devId) + "] passed in";
    return (XRM_ERROR_INVALID);
}

/*
 * Check whether the cu (compute unit) existing on specified device
 *
 * true: cu is existing on device
 * false: cu is NOT existing on device
 *
 * Lock: should enter lock before calling the function
 */
bool xrm::system::resIsCuExistingOnDev(int32_t devId, cuProperty* cuProp) {
    deviceData* dev;
    cuData* cu;
    bool kernelNameEqual, kernelAliasEqual, cuNameEqual;
    int32_t cuId;
    bool cuFound = false;

    if ((cuProp->kernelName[0] == '\0') && (cuProp->kernelAlias[0] == '\0') && (cuProp->cuName[0] == '\0')) {
        logMsg(XRM_LOG_ERROR, "None of kernel name, kernel alias and cu name are presented\n");
        return (cuFound);
    }

    dev = &m_devList[devId];
    /*
     * Now check whether matching cu is on allocated device
     * if not, free device, increment dev count, re-loop
     */
    for (cuId = 0; cuId < XRM_MAX_XILINX_KERNELS && cuId < dev->xclbinInfo.numCu && !cuFound; cuId++) {
        cu = &dev->xclbinInfo.cuList[cuId];

        /*
         * compare, 0: equal
         *
         * kernel name is presented, compare it; otherwise no need to compare
         */
        kernelNameEqual = true;
        if (cuProp->kernelName[0] != '\0') {
            if (cu->kernelName.compare(cuProp->kernelName)) kernelNameEqual = false;
        }
        /* kernel alias is presented, compare it; otherwise no need to compare */
        kernelAliasEqual = true;
        if (cuProp->kernelAlias[0] != '\0') {
            if (cu->kernelAlias.compare(cuProp->kernelAlias)) kernelAliasEqual = false;
        }
        /* cu name is presented, compare it; otherwise no need to compare */
        cuNameEqual = true;
        if (cuProp->cuName[0] != '\0') {
            if (cu->cuName.compare(cuProp->cuName)) cuNameEqual = false;
        }
        if (!(kernelNameEqual && kernelAliasEqual && cuNameEqual)) continue;
        cuFound = true;
    }
    return (cuFound);
}

/*
 * Check whether the cu (compute unit) existing in system
 *
 * true: cu is existing in system
 * false: cu is NOT existing in system
 *
 * Lock: should enter lock during the checking so that the result is accurate.
 */
bool xrm::system::resIsCuExisting(cuProperty* cuProp) {
    deviceData* dev;
    int32_t devId;
    bool cuFound = false;

    if ((cuProp->kernelName[0] == '\0') && (cuProp->kernelAlias[0] == '\0') && (cuProp->cuName[0] == '\0')) {
        logMsg(XRM_LOG_ERROR, "None of kernel name, kernel alias and cu name are presented\n");
        return (cuFound);
    }

    for (devId = 0; !cuFound && (devId < m_numDevice); devId++) {
        dev = &m_devList[devId];
        if (!dev->isLoaded) continue;
        cuFound = resIsCuExistingOnDev(devId, cuProp);
    }
    return (cuFound);
}

/*
 * Check whether the cu list existing in system
 *
 * true: cu list is existing in system
 * false: cu list is NOT existing in system
 *
 * Lock: should enter lock during the checking so that the result is accurate.
 */
bool xrm::system::resIsCuListExisting(cuListProperty* cuListProp) {
    deviceData* dev;
    int32_t i, devId;
    cuProperty* cuProp = NULL;
    bool cuListFound = false;
    bool allCuFound = false;
    bool cuFound = false;

    if (cuListProp->cuNum <= 0 || cuListProp->cuNum > XRM_MAX_LIST_CU_NUM) {
        return (cuListFound);
    }

    if (cuListProp->sameDevice) {
        /*
         * To find the cu list from same device
         */
        for (devId = 0; (devId < m_numDevice) && !cuListFound; devId++) {
            dev = &m_devList[devId];
            if (!dev->isLoaded) continue;
            /* find the cu list from this device */
            allCuFound = true;
            for (i = 0; i < cuListProp->cuNum; i++) {
                cuProp = &cuListProp->cuProps[i];
                if (!resIsCuExistingOnDev(devId, cuProp)) {
                    allCuFound = false;
                    break;
                }
            }
            cuListFound = allCuFound;
        }
    } else {
        /*
         * To find the cu list from any device
         */
        for (i = 0; i < cuListProp->cuNum; i++) {
            cuProp = &cuListProp->cuProps[i];
            /* find the cu from all device */
            for (devId = 0; (devId < m_numDevice) && !cuFound; devId++) {
                dev = &m_devList[devId];
                if (!dev->isLoaded) continue;
                /* find the cu from this device */
                if (resIsCuExistingOnDev(devId, cuProp)) {
                    cuFound = true;
                    break;
                }
            }
            if (!cuFound) break;
            if (i == (cuListProp->cuNum - 1)) cuListFound = true;
        }
    }
    return (cuListFound);
}

/*
 * Check whether the cu group existing in system
 *
 * true: cu group is existing in system
 * false: cu group is NOT existing in system
 *
 * Lock: will not use lock since it will call resIsCuListExisting() to check the resource.
 */
bool xrm::system::resIsCuGroupExisting(cuGroupProperty* cuGroupProp) {
    cuListProperty cuListProp;
    cuListProperty* udfCuListProp;
    cuProperty* cuProp;
    cuProperty* udfCuProp;
    bool cuGroupFound = false;
    int32_t udfCuGroupIdx = -1;

    if (cuGroupProp == NULL) {
        return (cuGroupFound);
    }
    for (uint32_t cuGroupIdx = 0; cuGroupIdx < m_numUdfCuGroup; cuGroupIdx++) {
        /* compare, 0: equal */
        if (!m_udfCuGroups[cuGroupIdx].udfCuGroupName.compare(cuGroupProp->udfCuGroupName.c_str())) {
            udfCuGroupIdx = cuGroupIdx;
            break;
        }
    }
    if (udfCuGroupIdx == -1) {
        logMsg(XRM_LOG_ERROR, "%s : user defined cu group is not declared\n", __func__);
        return (cuGroupFound);
    }
    for (int32_t cuListIdx = 0; cuListIdx < m_udfCuGroups[udfCuGroupIdx].optionUdfCuListNum; cuListIdx++) {
        memset(&cuListProp, 0, sizeof(cuListProperty));
        udfCuListProp = &m_udfCuGroups[udfCuGroupIdx].optionUdfCuListProps[cuListIdx];
        cuListProp.cuNum = udfCuListProp->cuNum;
        cuListProp.sameDevice = udfCuListProp->sameDevice;
        for (int32_t i = 0; i < cuListProp.cuNum; i++) {
            cuProp = &cuListProp.cuProps[i];
            udfCuProp = &udfCuListProp->cuProps[i];
            strncpy(cuProp->cuName, udfCuProp->cuName, XRM_MAX_NAME_LEN - 1);
        }
        cuGroupFound = resIsCuListExisting(&cuListProp);
        if (cuGroupFound) break;
    }
    return (cuGroupFound);
}

/*
 * Alloc one cu (compute unit) based on the request property.
 *
 * XRM_SUCCESS: allocated resouce is recorded by cuRes
 * Otherwise: failed to allocate the cu resource
 *
 * Lock: should enter lock during the cu allocation
 */
int32_t xrm::system::resAllocCu(cuProperty* cuProp, cuResource* cuRes, bool updateId) {
    deviceData* dev;
    cuData* cu;
    int32_t ret = 0;
    bool kernelNameEqual, kernelAliasEqual, cuNameEqual;
    uint64_t clientId = cuProp->clientId;
    int32_t devId, cuId;
    bool cuAcquired = false;
    /* First pass will look for kernels already in-use by proc */
    bool cuAffinityPass = true;

    if ((cuProp == NULL) || (cuRes == NULL)) {
        logMsg(XRM_LOG_ERROR, "cuProp or cuRes is NULL\n");
        return (XRM_ERROR_INVALID);
    }
    if ((cuProp->kernelName[0] == '\0') && (cuProp->kernelAlias[0] == '\0') && (cuProp->cuName[0] == '\0')) {
        logMsg(XRM_LOG_ERROR, "None of kernel name, kernel alias and cu name are presented\n");
        return (XRM_ERROR_INVALID);
    }

cu_alloc_loop:
    for (devId = -1; !cuAcquired && (devId < m_numDevice);) {
        devId = allocDevForClient(&devId, cuProp);
        if (devId < 0) {
            break;
        }

        ret = allocClientFromDev(devId, cuProp);
        if (ret < 0) {
            continue;
        }
        dev = &m_devList[devId];
        /*
         * Now check whether matching cu is on allocated device
         * if not, free device, increment dev count, re-loop
         */
        for (cuId = 0; cuId < XRM_MAX_XILINX_KERNELS && cuId < dev->xclbinInfo.numCu && !cuAcquired; cuId++) {
            cu = &dev->xclbinInfo.cuList[cuId];

            /* first attempt to re-use existing kernels; else, use a new kernel */
            if ((cuAffinityPass && cu->numClient == 0) || (!cuAffinityPass && cu->numClient > 0)) continue;
            /*
             * compare, 0: equal
             *
             * kernel name is presented, compare it; otherwise no need to compare
             */
            kernelNameEqual = true;
            if (cuProp->kernelName[0] != '\0') {
                if (cu->kernelName.compare(cuProp->kernelName)) kernelNameEqual = false;
            }
            /* kernel alias is presented, compare it; otherwise no need to compare */
            kernelAliasEqual = true;
            if (cuProp->kernelAlias[0] != '\0') {
                if (cu->kernelAlias.compare(cuProp->kernelAlias)) kernelAliasEqual = false;
            }
            /* cu name is presented, compare it; otherwise no need to compare */
            cuNameEqual = true;
            if (cuProp->cuName[0] != '\0') {
                if (cu->cuName.compare(cuProp->cuName)) cuNameEqual = false;
            }
            if (!(kernelNameEqual && kernelAliasEqual && cuNameEqual)) continue;
            /* alloc channel and register client id */
            ret = allocChanClientFromCu(cu, cuProp, cuRes);
            if (ret != XRM_SUCCESS) {
                continue;
            }

            cuRes->deviceId = devId;
            cuRes->cuId = cuId;
            strncpy(cuRes->xclbinFileName, dev->xclbinName.c_str(), XRM_MAX_NAME_LEN - 1);
            strncpy(cuRes->uuidStr, dev->xclbinInfo.uuidStr.c_str(), XRM_MAX_NAME_LEN - 1);
            cuAcquired = true;
        }

        if (!cuAcquired) {
            releaseClientOnDev(devId, clientId);
        }
    }

    if (!cuAcquired && cuAffinityPass) {
        cuAffinityPass = false; /* open up search to all kernels */
        goto cu_alloc_loop;
    }

    if (cuAcquired) {
        if (updateId) updateAllocServiceId();
        return (XRM_SUCCESS);
    }

    return (XRM_ERROR_NO_KERNEL);
}

/*
 * Alloc one cu (compute unit) based on the request property version 2.
 *
 * XRM_SUCCESS: allocated resouce is recorded by cuRes
 * Otherwise: failed to allocate the cu resource
 *
 * Lock: should enter lock during the cu allocation
 */
int32_t xrm::system::resAllocCuV2(cuPropertyV2* cuPropV2, cuResource* cuRes, bool updateId) {
    if ((cuPropV2 == NULL) || (cuRes == NULL)) {
        logMsg(XRM_LOG_ERROR, "cuProp or cuRes is NULL\n");
        return (XRM_ERROR_INVALID);
    }
    if ((cuPropV2->kernelName[0] == '\0') && (cuPropV2->kernelAlias[0] == '\0') && (cuPropV2->cuName[0] == '\0')) {
        logMsg(XRM_LOG_ERROR, "None of kernel name, kernel alias and cu name are presented\n");
        return (XRM_ERROR_INVALID);
    }
    uint64_t deviceInfoConstraintType =
        (cuPropV2->deviceInfo >> XRM_DEVICE_INFO_CONSTRAINT_TYPE_SHIFT) & XRM_DEVICE_INFO_CONSTRAINT_TYPE_MASK;
    uint64_t deviceInfoDeviceIndex =
        (cuPropV2->deviceInfo >> XRM_DEVICE_INFO_DEVICE_INDEX_SHIFT) & XRM_DEVICE_INFO_DEVICE_INDEX_MASK;
    cuProperty cuProp;
    strncpy(cuProp.kernelName, cuPropV2->kernelName, XRM_MAX_NAME_LEN - 1);
    cuProp.kernelName[XRM_MAX_NAME_LEN - 1] = '\0';
    strncpy(cuProp.kernelAlias, cuPropV2->kernelAlias, XRM_MAX_NAME_LEN - 1);
    cuProp.kernelAlias[XRM_MAX_NAME_LEN - 1] = '\0';
    strcpy(cuProp.cuName, "");
    cuProp.devExcl = cuPropV2->devExcl;
    cuProp.requestLoadUnified = cuPropV2->requestLoadUnified;
    cuProp.requestLoadOriginal = cuPropV2->requestLoadOriginal;
    cuProp.clientId = cuPropV2->clientId;
    cuProp.clientProcessId = cuPropV2->clientProcessId;
    cuProp.poolId = cuPropV2->poolId;
    switch (deviceInfoConstraintType) {
        case XRM_DEVICE_INFO_CONSTRAINT_TYPE_NULL: {
            if (cuPropV2->policyInfo == XRM_POLICY_INFO_CONSTRAINT_TYPE_DEV_MOST_USED_FIRST ||
                cuPropV2->policyInfo == XRM_POLICY_INFO_CONSTRAINT_TYPE_DEV_LEAST_USED_FIRST)
                return (resAllocCuByDevLoad(cuPropV2, cuRes, updateId));
            if (cuPropV2->policyInfo == XRM_POLICY_INFO_CONSTRAINT_TYPE_CU_MOST_USED_FIRST ||
                cuPropV2->policyInfo == XRM_POLICY_INFO_CONSTRAINT_TYPE_CU_LEAST_USED_FIRST)
                return (resAllocCuByCuLoad(cuPropV2, cuRes, updateId));

            return (resAllocCu(&cuProp, cuRes, updateId));
        }
        case XRM_DEVICE_INFO_CONSTRAINT_TYPE_HARDWARE_DEVICE_INDEX: {
            return (resAllocCuFromDevByCuLoad((uint32_t)deviceInfoDeviceIndex, cuPropV2, cuRes, updateId));
        }
        default: {
            logMsg(XRM_LOG_ERROR, "invalid device info constraint type\n");
            return (XRM_ERROR_INVALID);
        }
    }
}

/*
 * Alloc one cu (compute unit) from target device based on the request property.
 *
 * XRM_SUCCESS: allocated resouce is recorded by cuRes
 * Otherwise: failed to allocate the cu resource
 *
 * Lock: should enter lock during the cu allocation
 */
int32_t xrm::system::resAllocCuFromDev(int32_t deviceId, cuProperty* cuProp, cuResource* cuRes, bool updateId) {
    deviceData* dev;
    cuData* cu;
    int32_t ret = 0;
    bool kernelNameEqual, kernelAliasEqual, cuNameEqual;
    uint64_t clientId = cuProp->clientId;
    int32_t cuId;
    bool cuAcquired = false;
    /* First pass will look for kernels already in-use by proc */
    bool cuAffinityPass = true;

    if ((deviceId < 0) || (deviceId > (m_numDevice - 1))) {
        logMsg(XRM_LOG_ERROR, "deviceId (%d) is out of range\n", deviceId);
        return (XRM_ERROR_INVALID);
    }
    if ((cuProp == NULL) || (cuRes == NULL)) {
        logMsg(XRM_LOG_ERROR, "cuProp or cuRes is NULL\n");
        return (XRM_ERROR_INVALID);
    }
    if ((cuProp->kernelName[0] == '\0') && (cuProp->kernelAlias[0] == '\0') && (cuProp->cuName[0] == '\0')) {
        logMsg(XRM_LOG_ERROR, "None of kernel name, kernel alias and cu name are presented\n");
        return (XRM_ERROR_INVALID);
    }

    dev = &m_devList[deviceId];
    if (!dev->isLoaded) {
        return (XRM_ERROR_NO_DEV);
    }
    if ((dev->isExcl) && (dev->clientProcs[0].clientId != clientId)) {
        /* the device is used by other client exclusively */
        return (XRM_ERROR_NO_DEV);
    }
    ret = allocClientFromDev(deviceId, cuProp);
    if (ret < 0) {
        return (XRM_ERROR_NO_DEV);
    }

cu_alloc_from_dev_loop:
    /*
     * Now check whether matching cu is on allocated device
     * and try to allocate requested cu.
     */
    for (cuId = 0; cuId < XRM_MAX_XILINX_KERNELS && cuId < dev->xclbinInfo.numCu && !cuAcquired; cuId++) {
        cu = &dev->xclbinInfo.cuList[cuId];

        /* first attempt to re-use existing kernels; else, use a new kernel */
        if ((cuAffinityPass && cu->numClient == 0) || (!cuAffinityPass && cu->numClient > 0)) continue;
        /*
         * compare, 0: equal
         *
         * kernel name is presented, compare it; otherwise no need to compare
         */
        kernelNameEqual = true;
        if (cuProp->kernelName[0] != '\0') {
            if (cu->kernelName.compare(cuProp->kernelName)) kernelNameEqual = false;
        }
        /* kernel alias is presented, compare it; otherwise no need to compare */
        kernelAliasEqual = true;
        if (cuProp->kernelAlias[0] != '\0') {
            if (cu->kernelAlias.compare(cuProp->kernelAlias)) kernelAliasEqual = false;
        }
        /* cu name is presented, compare it; otherwise no need to compare */
        cuNameEqual = true;
        if (cuProp->cuName[0] != '\0') {
            if (cu->cuName.compare(cuProp->cuName)) cuNameEqual = false;
        }
        if (!(kernelNameEqual && kernelAliasEqual && cuNameEqual)) continue;
        /* alloc channel and register client id */
        ret = allocChanClientFromCu(cu, cuProp, cuRes);
        if (ret != XRM_SUCCESS) {
            continue;
        }

        cuRes->deviceId = deviceId;
        cuRes->cuId = cuId;
        strncpy(cuRes->xclbinFileName, dev->xclbinName.c_str(), XRM_MAX_NAME_LEN - 1);
        strncpy(cuRes->uuidStr, dev->xclbinInfo.uuidStr.c_str(), XRM_MAX_NAME_LEN - 1);
        cuAcquired = true;
    }

    if (!cuAcquired && cuAffinityPass) {
        cuAffinityPass = false; /* open up search to all kernels */
        goto cu_alloc_from_dev_loop;
    }

    if (cuAcquired) {
        if (updateId) updateAllocServiceId();
        return (XRM_SUCCESS);
    } else {
        releaseClientOnDev(deviceId, clientId);
        return (XRM_ERROR_NO_KERNEL);
    }
}

/*
 * Alloc one cu (compute unit) from target device based on the request property.
 *
 * XRM_SUCCESS: allocated resouce is recorded by cuRes
 * Otherwise: failed to allocate the cu resource
 *
 * Lock: should enter lock during the cu allocation
 */
int32_t xrm::system::resAllocLeastUsedCuFromDev(int32_t deviceId,
                                                cuProperty* cuProp,
                                                cuResource* cuRes,
                                                bool updateId) {
    deviceData* dev;
    cuData* cu;
    int32_t ret = 0;
    bool kernelNameEqual, kernelAliasEqual, cuNameEqual;
    uint64_t clientId = cuProp->clientId;
    int32_t cuId;
    std::vector<std::tuple<int32_t, int32_t> > leastUsedCus;
    bool cuAcquired = false;

    if ((deviceId < 0) || (deviceId > (m_numDevice - 1))) {
        logMsg(XRM_LOG_ERROR, "deviceId (%d) is out of range\n", deviceId);
        return (XRM_ERROR_INVALID);
    }
    if ((cuProp == NULL) || (cuRes == NULL)) {
        logMsg(XRM_LOG_ERROR, "cuProp or cuRes is NULL\n");
        return (XRM_ERROR_INVALID);
    }
    if ((cuProp->kernelName[0] == '\0') && (cuProp->kernelAlias[0] == '\0') && (cuProp->cuName[0] == '\0')) {
        logMsg(XRM_LOG_ERROR, "None of kernel name, kernel alias and cu name are presented\n");
        return (XRM_ERROR_INVALID);
    }

    dev = &m_devList[deviceId];
    if (!dev->isLoaded) {
        return (XRM_ERROR_NO_DEV);
    }
    if ((dev->isExcl) && (dev->clientProcs[0].clientId != clientId)) {
        /* the device is used by other client exclusively */
        return (XRM_ERROR_NO_DEV);
    }
    ret = allocClientFromDev(deviceId, cuProp);
    if (ret < 0) {
        return (XRM_ERROR_NO_DEV);
    }

    /*
     * Now check whether matching cu is on allocated device
     * and try to allocate requested cu.
     */
    for (cuId = 0; cuId < XRM_MAX_XILINX_KERNELS && cuId < dev->xclbinInfo.numCu && !cuAcquired; cuId++) {
        cu = &dev->xclbinInfo.cuList[cuId];

        /* first attempt to use a new kernel */
        if (cu->numClient > 0) continue;
        /*
         * compare, 0: equal
         *
         * kernel name is presented, compare it; otherwise no need to compare
         */
        kernelNameEqual = true;
        if (cuProp->kernelName[0] != '\0') {
            if (cu->kernelName.compare(cuProp->kernelName)) kernelNameEqual = false;
        }
        /* kernel alias is presented, compare it; otherwise no need to compare */
        kernelAliasEqual = true;
        if (cuProp->kernelAlias[0] != '\0') {
            if (cu->kernelAlias.compare(cuProp->kernelAlias)) kernelAliasEqual = false;
        }
        /* cu name is presented, compare it; otherwise no need to compare */
        cuNameEqual = true;
        if (cuProp->cuName[0] != '\0') {
            if (cu->cuName.compare(cuProp->cuName)) cuNameEqual = false;
        }
        if (!(kernelNameEqual && kernelAliasEqual && cuNameEqual)) continue;
        /* alloc channel and register client id */
        ret = allocChanClientFromCu(cu, cuProp, cuRes);
        if (ret != XRM_SUCCESS) {
            continue;
        }

        cuRes->deviceId = deviceId;
        cuRes->cuId = cuId;
        strncpy(cuRes->xclbinFileName, dev->xclbinName.c_str(), XRM_MAX_NAME_LEN - 1);
        strncpy(cuRes->uuidStr, dev->xclbinInfo.uuidStr.c_str(), XRM_MAX_NAME_LEN - 1);
        cuAcquired = true;
    }

    if (!cuAcquired) {
        goto cu_alloc_from_dev_loop;
    }

    if (cuAcquired) {
        if (updateId) updateAllocServiceId();
        return (XRM_SUCCESS);
    } else {
        releaseClientOnDev(deviceId, clientId);
        return (XRM_ERROR_NO_KERNEL);
    }

cu_alloc_from_dev_loop:
    for (cuId = 0; cuId < XRM_MAX_XILINX_KERNELS && cuId < dev->xclbinInfo.numCu; cuId++) {
        cu = &dev->xclbinInfo.cuList[cuId];

        /*
         * compare, 0: equal
         *
         * kernel name is presented, compare it; otherwise no need to compare
         */
        kernelNameEqual = true;
        if (cuProp->kernelName[0] != '\0') {
            if (cu->kernelName.compare(cuProp->kernelName)) kernelNameEqual = false;
        }
        /* kernel alias is presented, compare it; otherwise no need to compare */
        kernelAliasEqual = true;
        if (cuProp->kernelAlias[0] != '\0') {
            if (cu->kernelAlias.compare(cuProp->kernelAlias)) kernelAliasEqual = false;
        }
        /* cu name is presented, compare it; otherwise no need to compare */
        cuNameEqual = true;
        if (cuProp->cuName[0] != '\0') {
            if (cu->cuName.compare(cuProp->cuName)) cuNameEqual = false;
        }
        if (!(kernelNameEqual && kernelAliasEqual && cuNameEqual)) continue;

        int64_t resultLoad = cuProp->requestLoadUnified + cu->totalUsedLoadUnified;
        if (resultLoad <= XRM_MAX_CHAN_LOAD_GRANULARITY_1000000) {
            // We've found a potential CU, lets note down its load, and cuId
            leastUsedCus.emplace_back(cu->totalUsedLoadUnified, cuId);
        }
    }

    std::sort(leastUsedCus.begin(), leastUsedCus.end());

    ret = XRM_ERROR_NO_KERNEL; // If no candidates, we fail

    for (const auto& candidate : leastUsedCus) {
        cuId = std::get<1>(candidate);

        cu = &dev->xclbinInfo.cuList[cuId];

        ret = allocChanClientFromCu(cu, cuProp, cuRes);
        if (ret != XRM_SUCCESS) {
            releaseClientOnDev(deviceId, clientId);
            continue;
        }
        cuRes->deviceId = deviceId;
        cuRes->cuId = cuId;
        strncpy(cuRes->xclbinFileName, dev->xclbinName.c_str(), XRM_MAX_NAME_LEN - 1);
        strncpy(cuRes->uuidStr, dev->xclbinInfo.uuidStr.c_str(), XRM_MAX_NAME_LEN - 1);
        cuAcquired = true;
        if (updateId) updateAllocServiceId();

        return (XRM_SUCCESS);
    }
    return (XRM_ERROR_NO_KERNEL);
}
/*
 * Allocate one cu (compute unit) based on the request property. if fail to allocate cu, then try
 * to load the xclbin to one device and allocate cu from this device.
 *
 * XRM_SUCCESS: allocated resouce is recorded by cuRes
 * Otherwise: failed to allocate the cu resource
 *
 * Lock: should enter lock during the cu acquiration
 */
int32_t xrm::system::resAllocCuWithLoad(cuProperty* cuProp, std::string xclbin, cuResource* cuRes, bool updateId) {
    deviceData* dev;
    cuData* cu;
    int32_t ret = 0;
    bool kernelNameEqual, kernelAliasEqual, cuNameEqual;
    uint64_t clientId = cuProp->clientId;
    int32_t devId, cuId;
    bool cuAcquired = false;
    /* First pass will look for kernels already in-use by proc */
    bool cuAffinityPass = true;

    if ((cuProp == NULL) || (cuRes == NULL)) {
        logMsg(XRM_LOG_ERROR, "cuProp or cuRes is NULL\n");
        return (XRM_ERROR_INVALID);
    }
    if ((cuProp->kernelName[0] == '\0') && (cuProp->kernelAlias[0] == '\0') && (cuProp->cuName[0] == '\0')) {
        logMsg(XRM_LOG_ERROR, "None of kernel name, kernel alias and cu name are presented\n");
        return (XRM_ERROR_INVALID);
    }

    /*
     * try to allocate the cu from existing resource pool
     */
cu_acquire_loop:
    for (devId = -1; !cuAcquired && (devId < m_numDevice);) {
        devId = allocDevForClient(&devId, cuProp);
        if (devId < 0) {
            break;
        }

        ret = allocClientFromDev(devId, cuProp);
        if (ret < 0) {
            continue;
        }
        dev = &m_devList[devId];
        /*
         * Now check whether matching cu is on allocated device
         * if not, free device, increment dev count, re-loop
         */
        for (cuId = 0; cuId < XRM_MAX_XILINX_KERNELS && cuId < dev->xclbinInfo.numCu && !cuAcquired; cuId++) {
            cu = &dev->xclbinInfo.cuList[cuId];

            /* first attempt to re-use existing kernels; else, use a new kernel */
            if ((cuAffinityPass && cu->numClient == 0) || (!cuAffinityPass && cu->numClient > 0)) continue;
            /*
             * compare, 0: equal
             *
             * kernel name is presented, compare it; otherwise no need to compare
             */
            kernelNameEqual = true;
            if (cuProp->kernelName[0] != '\0') {
                if (cu->kernelName.compare(cuProp->kernelName)) kernelNameEqual = false;
            }
            /* kernel alias is presented, compare it; otherwise no need to compare */
            kernelAliasEqual = true;
            if (cuProp->kernelAlias[0] != '\0') {
                if (cu->kernelAlias.compare(cuProp->kernelAlias)) kernelAliasEqual = false;
            }
            /* cu name is presented, compare it; otherwise no need to compare */
            cuNameEqual = true;
            if (cuProp->cuName[0] != '\0') {
                if (cu->cuName.compare(cuProp->cuName)) cuNameEqual = false;
            }
            if (!(kernelNameEqual && kernelAliasEqual && cuNameEqual)) continue;
            /* alloc channel and register client id */
            ret = allocChanClientFromCu(cu, cuProp, cuRes);
            if (ret != XRM_SUCCESS) {
                continue;
            }

            cuRes->deviceId = devId;
            cuRes->cuId = cuId;
            strncpy(cuRes->xclbinFileName, dev->xclbinName.c_str(), XRM_MAX_NAME_LEN - 1);
            strncpy(cuRes->uuidStr, dev->xclbinInfo.uuidStr.c_str(), XRM_MAX_NAME_LEN - 1);
            cuAcquired = true;
        }

        if (!cuAcquired) {
            releaseClientOnDev(devId, clientId);
        }
    }

    if (!cuAcquired && cuAffinityPass) {
        cuAffinityPass = false; /* open up search to all kernels */
        goto cu_acquire_loop;
    }

    if (cuAcquired) {
        if (updateId) updateAllocServiceId();
        return (XRM_SUCCESS);
    }

    /*
     * try to load the xclbin to one possible device
     */
    devId = -1;
    std::string loadErrmsg;
    ret = loadAnyDevice(&devId, xclbin, loadErrmsg);
    if (ret != XRM_SUCCESS) {
        logMsg(XRM_LOG_NOTICE, "%s fail to load xclbin to any device\n", __func__);
        goto cu_acquire_exit;
    }

    /*
     * try to allocate the cu from the device loaded with xclbin file
     */
    ret = allocClientFromDev(devId, cuProp);
    if (ret < 0) {
        goto cu_acquire_exit;
    }
    dev = &m_devList[devId];
    /* Now check whether matching cu is on allocated device, if not, free device. */
    for (cuId = 0; cuId < XRM_MAX_XILINX_KERNELS && cuId < dev->xclbinInfo.numCu && !cuAcquired; cuId++) {
        cu = &dev->xclbinInfo.cuList[cuId];

        /* first attempt to re-use existing kernels; else, use a new kernel */
        if ((cuAffinityPass && cu->numClient == 0) || (!cuAffinityPass && cu->numClient > 0)) continue;
        /*
         * compare, 0: equal
         *
         * kernel name is presented, compare it; otherwise no need to compare
         */
        kernelNameEqual = true;
        if (cuProp->kernelName[0] != '\0') {
            if (cu->kernelName.compare(cuProp->kernelName)) kernelNameEqual = false;
        }
        /* kernel alias is presented, compare it; otherwise no need to compare */
        kernelAliasEqual = true;
        if (cuProp->kernelAlias[0] != '\0') {
            if (cu->kernelAlias.compare(cuProp->kernelAlias)) kernelAliasEqual = false;
        }
        /* cu name is presented, compare it; otherwise no need to compare */
        cuNameEqual = true;
        if (cuProp->cuName[0] != '\0') {
            if (cu->cuName.compare(cuProp->cuName)) cuNameEqual = false;
        }
        if (!(kernelNameEqual && kernelAliasEqual && cuNameEqual)) continue;
        /* alloc channel and register client id */
        ret = allocChanClientFromCu(cu, cuProp, cuRes);
        if (ret != XRM_SUCCESS) {
            continue;
        }

        cuRes->deviceId = devId;
        cuRes->cuId = cuId;
        strncpy(cuRes->xclbinFileName, dev->xclbinName.c_str(), XRM_MAX_NAME_LEN - 1);
        strncpy(cuRes->uuidStr, dev->xclbinInfo.uuidStr.c_str(), XRM_MAX_NAME_LEN - 1);
        cuAcquired = true;
    }

    if (cuAcquired) {
        if (updateId) updateAllocServiceId();
        ret = XRM_SUCCESS;
    } else {
        releaseClientOnDev(devId, clientId);
        ret = XRM_ERROR_NO_KERNEL;
    }

cu_acquire_exit:
    return (ret);
}

/*
 * Allocate the least used cu (compute unit) based on the request property.
 * if an unused cu cannot be allocated, then try
 * to load the xclbin to one device and allocate cu from that device.
 * if there are no available devices, try to share the least used cu.
 * Additionally, it strictly enforces that the allocated CU comes
 * from a device that was loaded with the user specified xclbinFileName.
 * XRM_SUCCESS: allocated resouce is recorded by cuRes
 * Otherwise: failed to allocate the cu resource
 *
 * Lock: should enter lock during the cu acquiration
 */
int32_t xrm::system::resAllocCuLeastUsedWithLoad(cuProperty* cuProp,
                                                 std::string xclbin,
                                                 cuResource* cuRes,
                                                 bool updateId) {
    deviceData* dev;
    cuData* cu;
    int32_t ret = 0;
    bool kernelNameEqual, kernelAliasEqual, cuNameEqual;
    uint64_t clientId = cuProp->clientId;
    int32_t devId, cuId;
    bool cuAcquired = false;

    // Vector of tuples (cuData::totalUsedLoadUnified, devId, cuId)
    std::vector<std::tuple<int32_t, int32_t, int32_t> > leastUsedCus;

    if ((cuProp == NULL) || (cuRes == NULL)) {
        logMsg(XRM_LOG_ERROR, "cuProp or cuRes is NULL\n");
        return (XRM_ERROR_INVALID);
    }
    if ((cuProp->kernelName[0] == '\0') && (cuProp->kernelAlias[0] == '\0') && (cuProp->cuName[0] == '\0')) {
        logMsg(XRM_LOG_ERROR, "None of kernel name, kernel alias and cu name are presented\n");
        return (XRM_ERROR_INVALID);
    }

    /*
     * try to allocate the cu from existing resource pool
     */
    for (devId = -1; !cuAcquired && (devId < m_numDevice);) {
        devId = allocDevForClient(&devId, cuProp);
        if (devId < 0) {
            break;
        }

        // If user's requested xclbin does not match, keep searching
        if (m_devList[devId].xclbinName != xclbin) continue;

        ret = allocClientFromDev(devId, cuProp);
        if (ret < 0) {
            continue;
        }
        dev = &m_devList[devId];
        /*
         * Now check whether matching cu is on allocated device
         * if not, free device, increment dev count, re-loop
         */
        for (cuId = 0; cuId < XRM_MAX_XILINX_KERNELS && cuId < dev->xclbinInfo.numCu && !cuAcquired; cuId++) {
            cu = &dev->xclbinInfo.cuList[cuId];

            /* first attempt to use a new kernel */
            if (cu->numClient > 0) continue;
            /*
             * compare, 0: equal
             *
             * kernel name is presented, compare it; otherwise no need to compare
             */
            kernelNameEqual = true;
            if (cuProp->kernelName[0] != '\0') {
                if (cu->kernelName.compare(cuProp->kernelName)) kernelNameEqual = false;
            }
            /* kernel alias is presented, compare it; otherwise no need to compare */
            kernelAliasEqual = true;
            if (cuProp->kernelAlias[0] != '\0') {
                if (cu->kernelAlias.compare(cuProp->kernelAlias)) kernelAliasEqual = false;
            }
            /* cu name is presented, compare it; otherwise no need to compare */
            cuNameEqual = true;
            if (cuProp->cuName[0] != '\0') {
                if (cu->cuName.compare(cuProp->cuName)) cuNameEqual = false;
            }
            if (!(kernelNameEqual && kernelAliasEqual && cuNameEqual)) continue;
            /* alloc channel and register client id */
            ret = allocChanClientFromCu(cu, cuProp, cuRes);
            if (ret != XRM_SUCCESS) {
                continue;
            }

            cuRes->deviceId = devId;
            cuRes->cuId = cuId;
            strncpy(cuRes->xclbinFileName, dev->xclbinName.c_str(), XRM_MAX_NAME_LEN - 1);
            strncpy(cuRes->uuidStr, dev->xclbinInfo.uuidStr.c_str(), XRM_MAX_NAME_LEN - 1);
            cuAcquired = true;
        }

        if (!cuAcquired) {
            releaseClientOnDev(devId, clientId);
        }
    }

    if (cuAcquired) {
        if (updateId) updateAllocServiceId();
        return (XRM_SUCCESS);
    }

    /*
     * try to load the xclbin to one possible device
     */
    devId = -1;
    std::string loadErrmsg;
    ret = loadAnyDevice(&devId, xclbin, loadErrmsg);
    if (ret != XRM_SUCCESS) {
        logMsg(XRM_LOG_NOTICE, "%s fail to load xclbin to any device\n", __func__);
        goto cu_acquire_least_used_loop;
    }

    /*
     * try to allocate the cu from the device loaded with xclbin file
     */
    ret = allocClientFromDev(devId, cuProp);
    if (ret < 0) {
        goto cu_acquire_least_used_loop;
    }
    dev = &m_devList[devId];
    /* Now check whether matching cu is on allocated device, if not, free device. */
    for (cuId = 0; cuId < XRM_MAX_XILINX_KERNELS && cuId < dev->xclbinInfo.numCu && !cuAcquired; cuId++) {
        cu = &dev->xclbinInfo.cuList[cuId];

        /*
         * compare, 0: equal
         *
         * kernel name is presented, compare it; otherwise no need to compare
         */
        kernelNameEqual = true;
        if (cuProp->kernelName[0] != '\0') {
            if (cu->kernelName.compare(cuProp->kernelName)) kernelNameEqual = false;
        }
        /* kernel alias is presented, compare it; otherwise no need to compare */
        kernelAliasEqual = true;
        if (cuProp->kernelAlias[0] != '\0') {
            if (cu->kernelAlias.compare(cuProp->kernelAlias)) kernelAliasEqual = false;
        }
        /* cu name is presented, compare it; otherwise no need to compare */
        cuNameEqual = true;
        if (cuProp->cuName[0] != '\0') {
            if (cu->cuName.compare(cuProp->cuName)) cuNameEqual = false;
        }
        if (!(kernelNameEqual && kernelAliasEqual && cuNameEqual)) continue;
        /* alloc channel and register client id */
        ret = allocChanClientFromCu(cu, cuProp, cuRes);
        if (ret != XRM_SUCCESS) {
            continue;
        }

        cuRes->deviceId = devId;
        cuRes->cuId = cuId;
        strncpy(cuRes->xclbinFileName, dev->xclbinName.c_str(), XRM_MAX_NAME_LEN - 1);
        strncpy(cuRes->uuidStr, dev->xclbinInfo.uuidStr.c_str(), XRM_MAX_NAME_LEN - 1);
        cuAcquired = true;
    }

    if (cuAcquired) {
        if (updateId) updateAllocServiceId();
        ret = XRM_SUCCESS;
        goto cu_acquire_exit;
    } else {
        releaseClientOnDev(devId, clientId);
        ret = XRM_ERROR_NO_KERNEL;
    }

cu_acquire_least_used_loop:

    // Search all possible devices
    for (devId = -1; devId < m_numDevice;) {
        devId = allocDevForClient(&devId, cuProp);
        if (devId < 0) {
            break;
        }

        // If user's requested xclbin does not match, keep searching
        if (m_devList[devId].xclbinName != xclbin) continue;

        ret = allocClientFromDev(devId, cuProp);
        if (ret < 0) {
            continue;
        }
        dev = &m_devList[devId];
        /*
         * Now check whether matching cu is on allocated device
         * if not, free device, increment dev count, re-loop
         */
        // Search all possible cus
        for (cuId = 0; cuId < XRM_MAX_XILINX_KERNELS && cuId < dev->xclbinInfo.numCu; cuId++) {
            cu = &dev->xclbinInfo.cuList[cuId];

            /*
             * compare, 0: equal
             *
             * kernel name is presented, compare it; otherwise no need to compare
             */
            kernelNameEqual = true;
            if (cuProp->kernelName[0] != '\0') {
                if (cu->kernelName.compare(cuProp->kernelName)) kernelNameEqual = false;
            }
            /* kernel alias is presented, compare it; otherwise no need to compare */
            kernelAliasEqual = true;
            if (cuProp->kernelAlias[0] != '\0') {
                if (cu->kernelAlias.compare(cuProp->kernelAlias)) kernelAliasEqual = false;
            }
            /* cu name is presented, compare it; otherwise no need to compare */
            cuNameEqual = true;
            if (cuProp->cuName[0] != '\0') {
                if (cu->cuName.compare(cuProp->cuName)) cuNameEqual = false;
            }
            if (!(kernelNameEqual && kernelAliasEqual && cuNameEqual)) continue;

            int64_t resultLoad = cuProp->requestLoadUnified + cu->totalUsedLoadUnified;
            if (resultLoad <= XRM_MAX_CHAN_LOAD_GRANULARITY_1000000) {
                // We've found a potential CU, lets note down its load, devId, and cuId
                leastUsedCus.emplace_back(cu->totalUsedLoadUnified, devId, cuId);
            }
        }

        // Release this device
        releaseClientOnDev(devId, clientId);
    }

    std::sort(leastUsedCus.begin(), leastUsedCus.end());

    ret = XRM_ERROR_NO_KERNEL; // If no candidates, we fail

    for (const auto& candidate : leastUsedCus) {
        devId = std::get<1>(candidate);
        cuId = std::get<2>(candidate);

        ret = allocClientFromDev(devId, cuProp);
        if (ret < 0) {
            continue;
        }

        dev = &m_devList[devId];
        cu = &dev->xclbinInfo.cuList[cuId];

        ret = allocChanClientFromCu(cu, cuProp, cuRes);
        if (ret != XRM_SUCCESS) {
            releaseClientOnDev(devId, clientId);
            continue;
        }

        cuRes->deviceId = devId;
        cuRes->cuId = cuId;
        strncpy(cuRes->xclbinFileName, dev->xclbinName.c_str(), XRM_MAX_NAME_LEN - 1);
        strncpy(cuRes->uuidStr, dev->xclbinInfo.uuidStr.c_str(), XRM_MAX_NAME_LEN - 1);
        cuAcquired = true;
        if (updateId) updateAllocServiceId();
        return (XRM_SUCCESS);
    }

cu_acquire_exit:
    return (ret);
}

/*
 * Alloc one cu list based on the request property.
 *
 * XRM_SUCCESS: allocated resouce is filled into cuListRes
 * Otherwise: failed to allocate the cu list
 *
 * Lock: should enter lock during the cu list allocation
 */
int32_t xrm::system::resAllocCuList(cuListProperty* cuListProp, cuListResource* cuListRes) {
    int32_t i, devId, ret;
    cuProperty* cuProp = NULL;
    cuResource* cuRes = NULL;

    if (cuListProp == NULL || cuListRes == NULL) {
        return (XRM_ERROR_INVALID);
    }
    if (cuListProp->cuNum <= 0 || cuListProp->cuNum > XRM_MAX_LIST_CU_NUM) {
        return (XRM_ERROR_INVALID);
    }

    cuListRes->cuNum = 0;
    ret = XRM_ERROR;
    if (cuListProp->sameDevice) {
        /*
         * To alloc the cu list from same device
         */
        for (devId = -1; devId < m_numDevice;) {
            /* find one available device for first cu*/
            cuProp = &cuListProp->cuProps[0];
            devId = allocDevForClient(&devId, cuProp);
            if (devId < 0) {
                return (XRM_ERROR_NO_KERNEL);
            }
            /* alloc the cu list from this device */
            for (i = 0; i < cuListProp->cuNum; i++) {
                cuProp = &cuListProp->cuProps[i];
                cuRes = &cuListRes->cuResources[i];
                ret = allocClientFromDev(devId, cuProp);
                if (ret != XRM_SUCCESS) {
                    resReleaseCuList(cuListRes);
                    memset(cuListRes, 0, sizeof(cuListResource));
                    break;
                }
                ret = allocCuFromDev(devId, cuProp, cuRes);
                if (ret != XRM_SUCCESS) {
                    releaseClientOnDev(devId, cuProp->clientId);
                    resReleaseCuList(cuListRes);
                    memset(cuListRes, 0, sizeof(cuListResource));
                    break;
                }
                cuListRes->cuNum++;
            }
            if (ret == XRM_SUCCESS) {
                updateAllocServiceId();
                return (XRM_SUCCESS);
            }
        }
        if (ret == XRM_SUCCESS)
            return (XRM_SUCCESS);
        else
            return (XRM_ERROR_NO_KERNEL);
    } else {
        /*
         * To alloc the cu list from any device
         */
        for (i = 0; i < cuListProp->cuNum; i++) {
            cuProp = &cuListProp->cuProps[i];
            cuRes = &cuListRes->cuResources[i];
            bool updateId = false;
            ret = resAllocCu(cuProp, cuRes, updateId);
            if (ret != XRM_SUCCESS) {
                resReleaseCuList(cuListRes);
                memset(cuListRes, 0, sizeof(cuListResource));
                return (XRM_ERROR_NO_KERNEL);
            }
            cuListRes->cuNum++;
        }
        updateAllocServiceId();
        return (XRM_SUCCESS);
    }
}

/*
 * copy cu property from src prop (V2) to des prop.
 */
void xrm::system::cuPropertyCopyFromV2(cuProperty* desCuProp, cuPropertyV2* srcCuPropV2) {
    if (desCuProp != NULL && srcCuPropV2 != NULL) {
        strncpy(desCuProp->kernelName, srcCuPropV2->kernelName, XRM_MAX_NAME_LEN - 1);
        desCuProp->kernelName[XRM_MAX_NAME_LEN - 1] = '\0';
        strncpy(desCuProp->kernelAlias, srcCuPropV2->kernelAlias, XRM_MAX_NAME_LEN - 1);
        desCuProp->kernelAlias[XRM_MAX_NAME_LEN - 1] = '\0';
        strcpy(desCuProp->cuName, "");
        desCuProp->devExcl = srcCuPropV2->devExcl;
        desCuProp->requestLoadUnified = srcCuPropV2->requestLoadUnified;
        desCuProp->requestLoadOriginal = srcCuPropV2->requestLoadOriginal;
        desCuProp->clientId = srcCuPropV2->clientId;
        desCuProp->clientProcessId = srcCuPropV2->clientProcessId;
        desCuProp->poolId = srcCuPropV2->poolId;
    }
}

/*
 * copy cu resource from src to des.
 */
void xrm::system::cuResourceCopy(cuResource* desCuRes, cuResource* srcCuRes) {
    if (desCuRes != NULL && srcCuRes != NULL) {
        strncpy(desCuRes->xclbinFileName, srcCuRes->xclbinFileName, XRM_MAX_NAME_LEN - 1);
        desCuRes->xclbinFileName[XRM_MAX_NAME_LEN - 1] = '\0';
        strncpy(desCuRes->kernelPluginFileName, srcCuRes->kernelPluginFileName, XRM_MAX_NAME_LEN - 1);
        desCuRes->kernelPluginFileName[XRM_MAX_NAME_LEN - 1] = '\0';
        strncpy(desCuRes->kernelName, srcCuRes->kernelName, XRM_MAX_NAME_LEN - 1);
        desCuRes->kernelName[XRM_MAX_NAME_LEN - 1] = '\0';
        strncpy(desCuRes->kernelAlias, srcCuRes->kernelAlias, XRM_MAX_NAME_LEN - 1);
        desCuRes->kernelAlias[XRM_MAX_NAME_LEN - 1] = '\0';
        strncpy(desCuRes->instanceName, srcCuRes->instanceName, XRM_MAX_NAME_LEN - 1);
        desCuRes->instanceName[XRM_MAX_NAME_LEN - 1] = '\0';
        strncpy(desCuRes->cuName, srcCuRes->cuName, XRM_MAX_NAME_LEN - 1);
        desCuRes->cuName[XRM_MAX_NAME_LEN - 1] = '\0';
        strncpy(desCuRes->uuidStr, srcCuRes->uuidStr, XRM_MAX_NAME_LEN - 1);
        desCuRes->uuidStr[XRM_MAX_NAME_LEN - 1] = '\0';
        desCuRes->deviceId = srcCuRes->deviceId;
        desCuRes->cuId = srcCuRes->cuId;
        desCuRes->channelId = srcCuRes->channelId;
        desCuRes->cuType = srcCuRes->cuType;
        desCuRes->baseAddr = srcCuRes->baseAddr;
        desCuRes->membankId = srcCuRes->membankId;
        desCuRes->membankType = srcCuRes->membankType;
        desCuRes->membankSize = srcCuRes->membankSize;
        desCuRes->membankBaseAddr = srcCuRes->membankBaseAddr;
        desCuRes->allocServiceId = srcCuRes->allocServiceId;
        desCuRes->clientId = srcCuRes->clientId;
        desCuRes->channelLoadUnified = srcCuRes->channelLoadUnified;
        desCuRes->channelLoadOriginal = srcCuRes->channelLoadOriginal;
        desCuRes->poolId = srcCuRes->poolId;
    }
}

/* check whether devId in device id list */
bool xrm::system::isInDeviceIdList(deviceIdListPropertyV2* devIdList, int32_t devId) {
    bool ret = false;
    if (devIdList->deviceNum < 0 || devIdList->deviceNum > XRM_MAX_XILINX_DEVICES) return (ret);
    if (devId < 0 || devId > (XRM_MAX_XILINX_DEVICES - 1)) return (ret);
    for (int32_t i = 0; i < devIdList->deviceNum; i++) {
        if (devIdList->deviceIds[i] == (uint64_t)devId) {
            ret = true;
            break;
        }
    }
    return (ret);
}

/* insert one devId into device id list, device id in list is unique */
uint32_t xrm::system::insertDeviceIdList(deviceIdListPropertyV2* devIdList, int32_t devId) {
    uint32_t ret = XRM_ERROR;
    if (devIdList->deviceNum < 0 || devIdList->deviceNum > XRM_MAX_XILINX_DEVICES) return (ret);
    if (devId < 0 || devId > (XRM_MAX_XILINX_DEVICES - 1)) return (ret);
    for (int32_t i = 0; i < devIdList->deviceNum; i++) {
        if (devIdList->deviceIds[i] == (uint64_t)devId) {
            ret = XRM_SUCCESS;
            return (ret);
        }
    }
    if (devIdList->deviceNum < XRM_MAX_XILINX_DEVICES) {
        devIdList->deviceIds[devIdList->deviceNum] = (uint64_t)devId;
        devIdList->deviceNum++;
        ret = XRM_SUCCESS;
    } else {
        ret = XRM_ERROR;
    }
    return (ret);
}

/*
 * To allocate sub list of cu from same device, the device should be different from that in used deviceId list.
 * but will not update AllocServiceId
 */
int32_t xrm::system::resAllocCuSubListFromSameDevice(cuSubListPropertyV2* cuSubListPropV2,
                                                     deviceIdListPropertyV2* usedDevIdListV2,
                                                     cuSubListResourceV2* cuSubListResV2) {
    int32_t devId, ret, i;
    cuProperty* cuProp = NULL;
    cuResource* cuRes = NULL;

    if (cuSubListPropV2 == NULL || usedDevIdListV2 == NULL || cuSubListResV2 == NULL) {
        return (XRM_ERROR_INVALID);
    }
    if (cuSubListPropV2->cuNum <= 0 || cuSubListPropV2->cuNum > XRM_MAX_LIST_CU_NUM_V2 ||
        usedDevIdListV2->deviceNum < 0 || usedDevIdListV2->deviceNum > XRM_MAX_XILINX_DEVICES) {
        return (XRM_ERROR_INVALID);
    }

    for (devId = -1; devId < m_numDevice;) {
        /* find one available device for first cu*/
        cuProp = &cuSubListPropV2->cuProps[0];
    alloc_device:
        devId = allocDevForClient(&devId, cuProp);
        if (devId < 0) {
            return (XRM_ERROR_NO_KERNEL);
        }
        /* check whether the device is in used devId list, if yes, then find another one */
        if (isInDeviceIdList(usedDevIdListV2, devId)) {
            goto alloc_device;
        }
        /* alloc the cu list from this device */
        for (i = 0; i < cuSubListPropV2->cuNum; i++) {
            cuProp = &cuSubListPropV2->cuProps[i];
            cuRes = &cuSubListResV2->cuResources[i];
            ret = allocClientFromDev(devId, cuProp);
            if (ret != XRM_SUCCESS) {
                resReleaseCuSubList(cuSubListResV2);
                memset(cuSubListResV2, 0, sizeof(cuSubListResourceV2));
                break;
            }
            ret = allocCuFromDev(devId, cuProp, cuRes);
            if (ret != XRM_SUCCESS) {
                releaseClientOnDev(devId, cuProp->clientId);
                resReleaseCuSubList(cuSubListResV2);
                memset(cuSubListResV2, 0, sizeof(cuSubListResourceV2));
                break;
            }
            cuSubListResV2->cuNum++;
        }
        if (ret == XRM_SUCCESS) {
            // will not update AllocServiceId
            return (XRM_SUCCESS);
        }
    }
    if (ret == XRM_SUCCESS)
        return (XRM_SUCCESS);
    else
        return (XRM_ERROR_NO_KERNEL);
}

int32_t xrm::system::resReleaseCuSubList(cuSubListResourceV2* cuSubListResV2) {
    int32_t i, ret;
    cuResource* cuRes;

    if (cuSubListResV2 == NULL) return (XRM_ERROR_INVALID);
    if (cuSubListResV2->cuNum < 0 || cuSubListResV2->cuNum > XRM_MAX_LIST_CU_NUM_V2) return (XRM_ERROR_INVALID);
    /* No resource need to be released */
    if (cuSubListResV2->cuNum == 0) return (XRM_SUCCESS);

    for (i = 0; i < cuSubListResV2->cuNum; i++) {
        cuRes = &cuSubListResV2->cuResources[i];
        ret = resReleaseCuV2(cuRes);
        if (ret != XRM_SUCCESS) break;
    }
    return (ret);
}

// #define GET_DEVICE_INFO_CONSTRAINT_TYPE(deviceInfo) (((deviceInfo) >> XRM_DEVICE_INFO_CONSTRAINT_TYPE_SHIFT) &
// XRM_DEVICE_INFO_CONSTRAINT_TYPE_MASK) #define GET_DEVICE_INFO_DEVICE_INDEX(deviceInfo) (((deviceInfo) >>
// XRM_DEVICE_INFO_DEVICE_INDEX_SHIFT) & XRM_DEVICE_INFO_DEVICE_INDEX_MASK)

/*
 * Alloc one cu list based on the request property.
 *
 * For each CU:
 * 1) if device constraint not set: allocate it from any device
 * 2) if device constraint set with hw index: allocate it from target device
 * 3) if device constraint set with virtual index: find all others with same virtual index, put them into
 *    one sub-list, and allocate requested CU from same device, the device id should be different from that
 *    for previous sub-lists request from virtual index.
 *
 * XRM_SUCCESS: allocated resouce is filled into cuListRes
 * Otherwise: failed to allocate the cu list
 *
 * Lock: should enter lock during the cu list allocation
 */
int32_t xrm::system::resAllocCuListV2(cuListPropertyV2* cuListPropV2, cuListResourceV2* cuListResV2) {
    int32_t index, ret;
    cuPropertyV2* cuPropV2 = NULL;
    cuResource* cuRes = NULL;
    uint64_t deviceInfoConstraintType;
    uint64_t deviceInfoDeviceIndex;
    itemFlag* itemFlags;
    deviceIdListPropertyV2* usedDevIdList = NULL;
    cuProperty cuProp;
    int32_t subListCuNum;

    if (cuListPropV2 == NULL || cuListResV2 == NULL) {
        return (XRM_ERROR_INVALID);
    }
    if (cuListPropV2->cuNum <= 0 || cuListPropV2->cuNum > XRM_MAX_LIST_CU_NUM_V2) {
        return (XRM_ERROR_INVALID);
    }

    /* use to set the flag to record whether the item is handled */
    itemFlags = (itemFlag*)malloc(sizeof(itemFlag) * XRM_MAX_LIST_CU_NUM_V2);
    memset(itemFlags, 0, sizeof(itemFlag) * XRM_MAX_LIST_CU_NUM_V2);

    /* use to record used device id for sub-list which request from different virtual device index */
    usedDevIdList = (deviceIdListPropertyV2*)malloc(sizeof(deviceIdListPropertyV2));
    memset(usedDevIdList, 0, sizeof(deviceIdListPropertyV2));

    ret = XRM_ERROR;

    /*
     * To allocate the requested cu:
     * 1) if device constraint not set: allocate it from any device
     * 2) if device constraint set with hw index: allocate it from target device
     * 3) if device constraint set with virtual index: find all others with same virtual index, put them into
     *    one sub-list, and allocate requested CU from same device, the device id should be different from that
     *    for previous sub-lists request from virtual index.
     */
    for (index = 0; index < cuListPropV2->cuNum; index++) {
        if (itemFlags[index].handled) continue;
        itemFlags[index].handled = true;
        cuPropV2 = &cuListPropV2->cuProps[index];
        deviceInfoConstraintType =
            (((cuPropV2->deviceInfo) >> XRM_DEVICE_INFO_CONSTRAINT_TYPE_SHIFT) & XRM_DEVICE_INFO_CONSTRAINT_TYPE_MASK);
        deviceInfoDeviceIndex =
            (((cuPropV2->deviceInfo) >> XRM_DEVICE_INFO_DEVICE_INDEX_SHIFT) & XRM_DEVICE_INFO_DEVICE_INDEX_MASK);
        cuRes = &cuListResV2->cuResources[index];
        switch (deviceInfoConstraintType) {
            case XRM_DEVICE_INFO_CONSTRAINT_TYPE_NULL:
                cuPropertyCopyFromV2(&cuProp, cuPropV2);
                cuRes = &cuListResV2->cuResources[index];
                ret = resAllocCu(&cuProp, cuRes, false);
                if (ret == XRM_SUCCESS) {
                    cuListResV2->cuNum++;
                    itemFlags[index].allocated = true;
                }
                break;
            case XRM_DEVICE_INFO_CONSTRAINT_TYPE_HARDWARE_DEVICE_INDEX:
                cuPropertyCopyFromV2(&cuProp, cuPropV2);
                cuRes = &cuListResV2->cuResources[index];
                ret = resAllocCuFromDev((uint32_t)deviceInfoDeviceIndex, &cuProp, cuRes, false);
                if (ret == XRM_SUCCESS) {
                    cuListResV2->cuNum++;
                    itemFlags[index].allocated = true;
                }
                break;
            case XRM_DEVICE_INFO_CONSTRAINT_TYPE_VIRTUAL_DEVICE_INDEX:
                /*
                 * This is the first cu with different virtual device index from privious CUes.
                 * Here need create a new sub-list to handle all cu with same virtual device.
                 */
                cuSubListPropertyV2* cuSubListProp;
                cuSubListResourceV2* cuSubListRes;
                cuSubListProp = (cuSubListPropertyV2*)malloc(sizeof(cuSubListPropertyV2));
                memset(cuSubListProp, 0, sizeof(cuSubListPropertyV2));

                /* put current cu property into sub list */
                subListCuNum = 1;
                cuPropertyCopyFromV2(&cuSubListProp->cuProps[subListCuNum - 1], cuPropV2);
                cuSubListProp->indexInOriList[subListCuNum - 1] = index;

                /*
                 * look through the rest of the original list, put the cu property into sub list if
                 * virtual device index is same as the current one.
                 */
                uint64_t deviceInfoConstraintTypeNext;
                uint64_t deviceInfoDeviceIndexNext;
                for (int32_t subIndex = index + 1; subIndex < cuListPropV2->cuNum; subIndex++) {
                    if (itemFlags[subIndex].handled) continue;
                    cuPropV2 = &cuListPropV2->cuProps[subIndex];
                    deviceInfoConstraintTypeNext = (((cuPropV2->deviceInfo) >> XRM_DEVICE_INFO_CONSTRAINT_TYPE_SHIFT) &
                                                    XRM_DEVICE_INFO_CONSTRAINT_TYPE_MASK);
                    deviceInfoDeviceIndexNext = (((cuPropV2->deviceInfo) >> XRM_DEVICE_INFO_DEVICE_INDEX_SHIFT) &
                                                 XRM_DEVICE_INFO_DEVICE_INDEX_MASK);
                    if ((deviceInfoConstraintTypeNext == XRM_DEVICE_INFO_CONSTRAINT_TYPE_VIRTUAL_DEVICE_INDEX) &&
                        (deviceInfoDeviceIndexNext == deviceInfoDeviceIndex)) {
                        /* put the cu property into sub list */
                        subListCuNum++;
                        cuPropertyCopyFromV2(&cuSubListProp->cuProps[subListCuNum - 1], cuPropV2);
                        cuSubListProp->indexInOriList[subListCuNum - 1] = subIndex;
                        itemFlags[subIndex].handled = true;
                    }
                }
                cuSubListProp->cuNum = subListCuNum;
                cuSubListRes = (cuSubListResourceV2*)malloc(sizeof(cuSubListResourceV2));
                memset(cuSubListRes, 0, sizeof(cuSubListResourceV2));
                ret = resAllocCuSubListFromSameDevice(cuSubListProp, usedDevIdList, cuSubListRes);
                if (ret == XRM_SUCCESS) {
                    int32_t indexInOriList;
                    for (int32_t i = 0; i < cuSubListRes->cuNum; i++) {
                        indexInOriList = cuSubListProp->indexInOriList[i];
                        cuResourceCopy(&cuListResV2->cuResources[indexInOriList], &cuSubListRes->cuResources[i]);
                        itemFlags[indexInOriList].allocated = true;
                    }
                    cuListResV2->cuNum += cuSubListRes->cuNum;
                    // record the used device id (all cu in this sub list from same device)
                    insertDeviceIdList(usedDevIdList, cuSubListRes->cuResources[0].deviceId);
                }
                free(cuSubListProp);
                free(cuSubListRes);
                break;
            default:
                logMsg(XRM_LOG_ERROR, "invalid device info [%d] constraint type\n", index);
                ret = XRM_ERROR_INVALID;
                break;
        }
        if (ret != XRM_SUCCESS) break;
    }
    if (ret == XRM_SUCCESS) {
        updateAllocServiceId();
    } else {
        for (index = 0; index < cuListPropV2->cuNum; index++) {
            if (itemFlags[index].allocated) {
                cuRes = &cuListResV2->cuResources[index];
                resReleaseCuV2(cuRes);
            }
            cuListResV2->cuNum = 0;
        }
    }
    free(itemFlags);
    free(usedDevIdList);
    return (ret);
}

/*
 * Load xclbin to one device and alloc all cu from this device.
 *
 * XRM_SUCCESS: allocated resouce is filled into cuListRes
 * Otherwise: failed to load xclbin and allocate all cu from this device
 *
 * Lock: should enter lock during the operation
 */
int32_t xrm::system::resLoadAndAllocAllCu(std::string xclbin,
                                          uint64_t clientId,
                                          pid_t clientProcessId,
                                          cuListResource* cuListRes) {
    int32_t i, devId, ret;

    if (cuListRes == NULL) {
        return (XRM_ERROR_INVALID);
    }

    /*
     * try to load the xclbin to one possible device
     */
    devId = -1;
    std::string loadErrmsg;
    ret = loadAnyDevice(&devId, xclbin, loadErrmsg);
    if (ret != XRM_SUCCESS) {
        return (ret);
    }

    /*
     * allocate all the cu from this device
     */
    deviceData* dev = &m_devList[devId];
    dev->clientProcs[0].clientId = clientId;
    dev->clientProcs[0].clientProcessId = clientProcessId;
    dev->clientProcs[0].ref = 1;
    if (dev->xclbinInfo.numCu < XRM_MAX_LIST_CU_NUM)
        cuListRes->cuNum = dev->xclbinInfo.numCu;
    else
        cuListRes->cuNum = XRM_MAX_LIST_CU_NUM;
    cuData* cu = NULL;
    channelData* channel = NULL;
    cuResource* cuRes = NULL;
    uint64_t allocServiceId = getNextAllocServiceId();
    for (i = 0; i < cuListRes->cuNum; i++) {
        cuRes = &cuListRes->cuResources[i];
        cu = &dev->xclbinInfo.cuList[i];

        strncpy(cuRes->xclbinFileName, dev->xclbinName.c_str(), XRM_MAX_NAME_LEN - 1);
        strncpy(cuRes->kernelName, cu->kernelName.c_str(), XRM_MAX_NAME_LEN - 1);
        strncpy(cuRes->instanceName, cu->instanceName.c_str(), XRM_MAX_NAME_LEN - 1);
        strncpy(cuRes->kernelAlias, cu->kernelAlias.c_str(), XRM_MAX_NAME_LEN - 1);
        strncpy(cuRes->cuName, cu->cuName.c_str(), XRM_MAX_NAME_LEN - 1);
        strncpy(cuRes->kernelPluginFileName, cu->kernelPluginFileName.c_str(), XRM_MAX_NAME_LEN - 1);
        strncpy(cuRes->uuidStr, dev->xclbinInfo.uuidStr.c_str(), XRM_MAX_NAME_LEN - 1);
        cuRes->deviceId = devId;
        cuRes->cuId = i;
        cuRes->cuType = cu->cuType;
        cuRes->channelId = 0;
        cuRes->clientId = clientId;
        cuRes->channelLoadUnified = XRM_MAX_CHAN_LOAD_GRANULARITY_1000000;
        cuRes->channelLoadOriginal = XRM_MAX_CHAN_LOAD_GRANULARITY_100;
        cuRes->allocServiceId = allocServiceId;
        cuRes->baseAddr = cu->baseAddr;
        cuRes->membankId = cu->membankId;
        cuRes->membankType = cu->membankType;
        cuRes->membankSize = cu->membankSize;
        cuRes->membankBaseAddr = cu->membankBaseAddr;
        cuRes->poolId = 0;

        cu->totalUsedLoadUnified = XRM_MAX_CU_LOAD_GRANULARITY_1000000;
        cu->numChanInuse = 1;
        cu->clients[0] = clientId;
        cu->numClient = 1;
        channel = &cu->channels[0];
        channel->channelLoadUnified = XRM_MAX_CHAN_LOAD_GRANULARITY_1000000;
        channel->channelLoadOriginal = XRM_MAX_CHAN_LOAD_GRANULARITY_100;
        channel->allocServiceId = allocServiceId;
        channel->clientId = clientId;
        channel->clientProcessId = clientProcessId;
    }
    updateDeviceLoad(devId, 0, XRM_MAX_CU_LOAD_GRANULARITY_1000000 * cuListRes->cuNum);
    updateAllocServiceId();
    return (XRM_SUCCESS);
}

/*
 * flush the information data of user define cu group
 */
void xrm::system::flushUdfCuGroupInfo(uint32_t udfCuGroupIdx) {
    m_udfCuGroups[udfCuGroupIdx].udfCuGroupName = "";
    m_udfCuGroups[udfCuGroupIdx].optionUdfCuListNum = 0;
    for (int32_t cuListIdx = 0; cuListIdx < m_udfCuGroups[udfCuGroupIdx].optionUdfCuListNum; cuListIdx++) {
        memset(&m_udfCuGroups[udfCuGroupIdx].optionUdfCuListProps[cuListIdx], 0, sizeof(cuListProperty));
    }
}

/*
 * flush the information data of user define cu group V2
 */
void xrm::system::flushUdfCuGroupInfoV2(uint32_t udfCuGroupIdx) {
    m_udfCuGroupsV2[udfCuGroupIdx].udfCuGroupName = "";
    m_udfCuGroupsV2[udfCuGroupIdx].optionUdfCuListNum = 0;
    for (int32_t cuListIdx = 0; cuListIdx < m_udfCuGroupsV2[udfCuGroupIdx].optionUdfCuListNum; cuListIdx++) {
        memset(&m_udfCuGroupsV2[udfCuGroupIdx].optionUdfCuListProps[cuListIdx], 0, sizeof(cuListPropertyV2));
    }
}

/*
 * declare one user defined cu group type based on the input information.
 *
 * XRM_SUCCESS: the user defined cu group is added into end of m_udfCuGroups[], no hole
 * Otherwise: failed to declare the user defined cu group
 *
 * Lock: should enter lock during the cu group declaration
 */
int32_t xrm::system::resUdfCuGroupDeclare(udfCuGroupInformation* udfCuGroupInfo) {
    cuListProperty* cuListProp;
    cuListProperty* udfCuListProp;
    int32_t udfCuGroupIdx = -1;

    if (udfCuGroupInfo == NULL) {
        return (XRM_ERROR_INVALID);
    }
    for (uint32_t cuGroupIdx = 0; cuGroupIdx < m_numUdfCuGroup; cuGroupIdx++) {
        /* compare, 0: equal */
        if (!m_udfCuGroups[cuGroupIdx].udfCuGroupName.compare(udfCuGroupInfo->udfCuGroupName.c_str())) {
            udfCuGroupIdx = (int32_t)cuGroupIdx;
            break;
        }
    }
    if (udfCuGroupIdx != -1) {
        logMsg(XRM_LOG_ERROR, "%s : user defined cu group is already existing\n", __func__);
        return (XRM_ERROR_INVALID);
    }

    m_udfCuGroups[m_numUdfCuGroup].udfCuGroupName = udfCuGroupInfo->udfCuGroupName;
    m_udfCuGroups[m_numUdfCuGroup].optionUdfCuListNum = udfCuGroupInfo->optionUdfCuListNum;
    for (int32_t cuListIdx = 0; cuListIdx < udfCuGroupInfo->optionUdfCuListNum; cuListIdx++) {
        cuListProp = &m_udfCuGroups[m_numUdfCuGroup].optionUdfCuListProps[cuListIdx];
        udfCuListProp = &udfCuGroupInfo->optionUdfCuListProps[cuListIdx];
        memset(cuListProp, 0, sizeof(cuListProperty));
        cuListProp->cuNum = udfCuListProp->cuNum;
        cuListProp->sameDevice = udfCuListProp->sameDevice;
        for (int32_t i = 0; i < cuListProp->cuNum; i++) {
            strncpy(cuListProp->cuProps[i].cuName, udfCuListProp->cuProps[i].cuName, XRM_MAX_NAME_LEN - 1);
            cuListProp->cuProps[i].devExcl = udfCuListProp->cuProps[i].devExcl;
            cuListProp->cuProps[i].requestLoadUnified = udfCuListProp->cuProps[i].requestLoadUnified;
            cuListProp->cuProps[i].requestLoadOriginal = udfCuListProp->cuProps[i].requestLoadOriginal;
        }
    }
    m_numUdfCuGroup++;
    return (XRM_SUCCESS);
}

/*
 * declare one user defined cu group type based on the input information.
 *
 * XRM_SUCCESS: the user defined cu group is added into end of m_udfCuGroupsV2[], no hole
 * Otherwise: failed to declare the user defined cu group
 *
 * Lock: should enter lock during the cu group declaration
 */
int32_t xrm::system::resUdfCuGroupDeclareV2(udfCuGroupInformationV2* udfCuGroupInfoV2) {
    cuListPropertyV2* cuListProp;
    cuListPropertyV2* udfCuListProp;
    int32_t udfCuGroupIdx = -1;

    if (udfCuGroupInfoV2 == NULL) {
        return (XRM_ERROR_INVALID);
    }
    for (uint32_t cuGroupIdx = 0; cuGroupIdx < m_numUdfCuGroupV2; cuGroupIdx++) {
        /* compare, 0: equal */
        if (!m_udfCuGroupsV2[cuGroupIdx].udfCuGroupName.compare(udfCuGroupInfoV2->udfCuGroupName.c_str())) {
            udfCuGroupIdx = (int32_t)cuGroupIdx;
            break;
        }
    }
    if (udfCuGroupIdx != -1) {
        logMsg(XRM_LOG_ERROR, "%s : user defined cu group is already existing\n", __func__);
        return (XRM_ERROR_INVALID);
    }

    m_udfCuGroupsV2[m_numUdfCuGroupV2].udfCuGroupName = udfCuGroupInfoV2->udfCuGroupName;
    m_udfCuGroupsV2[m_numUdfCuGroupV2].optionUdfCuListNum = udfCuGroupInfoV2->optionUdfCuListNum;
    for (int32_t cuListIdx = 0; cuListIdx < udfCuGroupInfoV2->optionUdfCuListNum; cuListIdx++) {
        cuListProp = &m_udfCuGroupsV2[m_numUdfCuGroupV2].optionUdfCuListProps[cuListIdx];
        udfCuListProp = &udfCuGroupInfoV2->optionUdfCuListProps[cuListIdx];
        memset(cuListProp, 0, sizeof(cuListPropertyV2));
        cuListProp->cuNum = udfCuListProp->cuNum;
        for (int32_t i = 0; i < cuListProp->cuNum; i++) {
            strncpy(cuListProp->cuProps[i].cuName, udfCuListProp->cuProps[i].cuName, XRM_MAX_NAME_LEN - 1);
            cuListProp->cuProps[i].devExcl = udfCuListProp->cuProps[i].devExcl;
            cuListProp->cuProps[i].deviceInfo = udfCuListProp->cuProps[i].deviceInfo;
            cuListProp->cuProps[i].memoryInfo = udfCuListProp->cuProps[i].memoryInfo;
            cuListProp->cuProps[i].requestLoadUnified = udfCuListProp->cuProps[i].requestLoadUnified;
            cuListProp->cuProps[i].requestLoadOriginal = udfCuListProp->cuProps[i].requestLoadOriginal;
        }
    }
    m_numUdfCuGroupV2++;
    return (XRM_SUCCESS);
}

/*
 * undeclare one user defined cu group type based on the input information.
 *
 * XRM_SUCCESS: the user defined cu group is removed from m_udfCuGroups[], no hole
 * Otherwise: failed to undeclare the user defined cu group
 *
 * Lock: should enter lock during the cu group undeclaration
 */
int32_t xrm::system::resUdfCuGroupUndeclare(udfCuGroupInformation* udfCuGroupInfo) {
    cuListProperty* desCuListProp;
    cuListProperty* srcCuListProp;
    bool udfCuGroupFound = false;
    uint32_t udfCuGroupIdx;

    if (udfCuGroupInfo == NULL) {
        return (XRM_ERROR_INVALID);
    }
    for (udfCuGroupIdx = 0; udfCuGroupIdx < m_numUdfCuGroup; udfCuGroupIdx++) {
        /* compare, 0: equal */
        if (!m_udfCuGroups[udfCuGroupIdx].udfCuGroupName.compare(udfCuGroupInfo->udfCuGroupName.c_str())) {
            udfCuGroupFound = true;
            break;
        }
    }
    if (!udfCuGroupFound) {
        logMsg(XRM_LOG_ERROR, "%s : user defined cu group is NOT existing\n", __func__);
        return (XRM_ERROR_INVALID);
    }

    /* move slot one step forward */
    for (; udfCuGroupIdx < (m_numUdfCuGroup - 1); udfCuGroupIdx++) {
        m_udfCuGroups[udfCuGroupIdx].udfCuGroupName = m_udfCuGroups[udfCuGroupIdx + 1].udfCuGroupName;
        m_udfCuGroups[udfCuGroupIdx].optionUdfCuListNum = m_udfCuGroups[udfCuGroupIdx + 1].optionUdfCuListNum;
        for (int32_t cuListIdx = 0; cuListIdx < m_udfCuGroups[udfCuGroupIdx].optionUdfCuListNum; cuListIdx++) {
            desCuListProp = &m_udfCuGroups[udfCuGroupIdx].optionUdfCuListProps[cuListIdx];
            srcCuListProp = &m_udfCuGroups[udfCuGroupIdx + 1].optionUdfCuListProps[cuListIdx];
            memset(desCuListProp, 0, sizeof(cuListProperty));
            desCuListProp->cuNum = srcCuListProp->cuNum;
            desCuListProp->sameDevice = srcCuListProp->sameDevice;
            for (int32_t i = 0; i < desCuListProp->cuNum; i++) {
                strncpy(desCuListProp->cuProps[i].cuName, srcCuListProp->cuProps[i].cuName, XRM_MAX_NAME_LEN - 1);
                desCuListProp->cuProps[i].devExcl = srcCuListProp->cuProps[i].devExcl;
                desCuListProp->cuProps[i].requestLoadUnified = srcCuListProp->cuProps[i].requestLoadUnified;
                desCuListProp->cuProps[i].requestLoadOriginal = srcCuListProp->cuProps[i].requestLoadOriginal;
            }
        }
    }

    /* flush the last slot */
    flushUdfCuGroupInfo(udfCuGroupIdx);
    m_numUdfCuGroup--;

    return (XRM_SUCCESS);
}

/*
 * undeclare one user defined cu group type based on the input information.
 *
 * XRM_SUCCESS: the user defined cu group is removed from m_udfCuGroupsV2[], no hole
 * Otherwise: failed to undeclare the user defined cu group
 *
 * Lock: should enter lock during the cu group undeclaration
 */
int32_t xrm::system::resUdfCuGroupUndeclareV2(udfCuGroupInformationV2* udfCuGroupInfoV2) {
    cuListPropertyV2* desCuListProp;
    cuListPropertyV2* srcCuListProp;
    bool udfCuGroupFound = false;
    uint32_t udfCuGroupIdx;

    if (udfCuGroupInfoV2 == NULL) {
        return (XRM_ERROR_INVALID);
    }
    for (udfCuGroupIdx = 0; udfCuGroupIdx < m_numUdfCuGroupV2; udfCuGroupIdx++) {
        /* compare, 0: equal */
        if (!m_udfCuGroupsV2[udfCuGroupIdx].udfCuGroupName.compare(udfCuGroupInfoV2->udfCuGroupName.c_str())) {
            udfCuGroupFound = true;
            break;
        }
    }
    if (!udfCuGroupFound) {
        logMsg(XRM_LOG_ERROR, "%s : user defined cu group is NOT existing\n", __func__);
        return (XRM_ERROR_INVALID);
    }

    /* move slot one step forward */
    for (; udfCuGroupIdx < (m_numUdfCuGroupV2 - 1); udfCuGroupIdx++) {
        m_udfCuGroupsV2[udfCuGroupIdx].udfCuGroupName = m_udfCuGroupsV2[udfCuGroupIdx + 1].udfCuGroupName;
        m_udfCuGroupsV2[udfCuGroupIdx].optionUdfCuListNum = m_udfCuGroupsV2[udfCuGroupIdx + 1].optionUdfCuListNum;
        for (int32_t cuListIdx = 0; cuListIdx < m_udfCuGroupsV2[udfCuGroupIdx].optionUdfCuListNum; cuListIdx++) {
            desCuListProp = &m_udfCuGroupsV2[udfCuGroupIdx].optionUdfCuListProps[cuListIdx];
            srcCuListProp = &m_udfCuGroupsV2[udfCuGroupIdx + 1].optionUdfCuListProps[cuListIdx];
            memset(desCuListProp, 0, sizeof(cuListPropertyV2));
            desCuListProp->cuNum = srcCuListProp->cuNum;
            for (int32_t i = 0; i < desCuListProp->cuNum; i++) {
                strncpy(desCuListProp->cuProps[i].cuName, srcCuListProp->cuProps[i].cuName, XRM_MAX_NAME_LEN - 1);
                desCuListProp->cuProps[i].devExcl = srcCuListProp->cuProps[i].devExcl;
                desCuListProp->cuProps[i].deviceInfo = srcCuListProp->cuProps[i].deviceInfo;
                desCuListProp->cuProps[i].memoryInfo = srcCuListProp->cuProps[i].memoryInfo;
                desCuListProp->cuProps[i].requestLoadUnified = srcCuListProp->cuProps[i].requestLoadUnified;
                desCuListProp->cuProps[i].requestLoadOriginal = srcCuListProp->cuProps[i].requestLoadOriginal;
            }
        }
    }

    /* flush the last slot */
    flushUdfCuGroupInfoV2(udfCuGroupIdx);
    m_numUdfCuGroupV2--;

    return (XRM_SUCCESS);
}

/*
 * Alloc one cu group based on the request property.
 *
 * XRM_SUCCESS: allocated resouce is filled into cuGroupRes
 * Otherwise: failed to allocate the cu group
 *
 * Lock: should enter lock during the cu group allocation
 */
int32_t xrm::system::resAllocCuGroup(cuGroupProperty* cuGroupProp, cuGroupResource* cuGroupRes) {
    int32_t ret = XRM_ERROR;
    cuListProperty cuListProp;
    cuListProperty* udfCuListProp;
    cuProperty* cuProp;
    cuProperty* udfCuProp;
    int32_t udfCuGroupIdx = -1;

    if (cuGroupProp == NULL || cuGroupRes == NULL) {
        return (XRM_ERROR_INVALID);
    }
    for (uint32_t cuGroupIdx = 0; cuGroupIdx < m_numUdfCuGroup; cuGroupIdx++) {
        /* compare, 0: equal */
        if (!m_udfCuGroups[cuGroupIdx].udfCuGroupName.compare(cuGroupProp->udfCuGroupName.c_str())) {
            udfCuGroupIdx = cuGroupIdx;
            break;
        }
    }
    if (udfCuGroupIdx == -1) {
        logMsg(XRM_LOG_ERROR, "%s : user defined cu group is not declared\n", __func__);
        return (XRM_ERROR_INVALID);
    }
    for (int32_t cuListIdx = 0; cuListIdx < m_udfCuGroups[udfCuGroupIdx].optionUdfCuListNum; cuListIdx++) {
        memset(&cuListProp, 0, sizeof(cuListProperty));
        udfCuListProp = &m_udfCuGroups[udfCuGroupIdx].optionUdfCuListProps[cuListIdx];
        cuListProp.cuNum = udfCuListProp->cuNum;
        cuListProp.sameDevice = udfCuListProp->sameDevice;
        for (int32_t i = 0; i < cuListProp.cuNum; i++) {
            cuProp = &cuListProp.cuProps[i];
            udfCuProp = &udfCuListProp->cuProps[i];
            strncpy(cuProp->cuName, udfCuProp->cuName, XRM_MAX_NAME_LEN - 1);
            cuProp->devExcl = udfCuProp->devExcl;
            cuProp->requestLoadUnified = udfCuProp->requestLoadUnified;
            cuProp->requestLoadOriginal = udfCuProp->requestLoadOriginal;
            cuProp->clientId = cuGroupProp->clientId;
            cuProp->clientProcessId = cuGroupProp->clientProcessId;
            cuProp->poolId = cuGroupProp->poolId;
        }
        memset(cuGroupRes, 0, sizeof(cuGroupResource));
        /*
         * NOTE: here since the cuListResource has same member of cuGroupResource, so it's fine to
         * use cuGroupRes to call resAllocCuList() directly, otherwise should use cuListResource instead.
         */
        ret = resAllocCuList(&cuListProp, (cuListResource*)cuGroupRes);
        if (ret == XRM_SUCCESS) break;
    }
    return (ret);
}

/*
 * Alloc one cu group based on the request property.
 *
 * XRM_SUCCESS: allocated resouce is filled into cuGroupRes
 * Otherwise: failed to allocate the cu group
 *
 * Lock: should enter lock during the cu group allocation
 */
int32_t xrm::system::resAllocCuGroupV2(cuGroupPropertyV2* cuGroupPropV2, cuGroupResourceV2* cuGroupResV2) {
    int32_t ret = XRM_ERROR;
    cuListPropertyV2 cuListProp;
    cuListPropertyV2* udfCuListProp;
    cuPropertyV2* cuProp;
    cuPropertyV2* udfCuProp;
    int32_t udfCuGroupIdx = -1;

    if (cuGroupPropV2 == NULL || cuGroupResV2 == NULL) {
        return (XRM_ERROR_INVALID);
    }
    for (uint32_t cuGroupIdx = 0; cuGroupIdx < m_numUdfCuGroupV2; cuGroupIdx++) {
        /* compare, 0: equal */
        if (!m_udfCuGroupsV2[cuGroupIdx].udfCuGroupName.compare(cuGroupPropV2->udfCuGroupName.c_str())) {
            udfCuGroupIdx = cuGroupIdx;
            break;
        }
    }
    if (udfCuGroupIdx == -1) {
        logMsg(XRM_LOG_ERROR, "%s : user defined cu group is not declared\n", __func__);
        return (XRM_ERROR_INVALID);
    }
    for (int32_t cuListIdx = 0; cuListIdx < m_udfCuGroupsV2[udfCuGroupIdx].optionUdfCuListNum; cuListIdx++) {
        memset(&cuListProp, 0, sizeof(cuListPropertyV2));
        udfCuListProp = &m_udfCuGroupsV2[udfCuGroupIdx].optionUdfCuListProps[cuListIdx];
        cuListProp.cuNum = udfCuListProp->cuNum;
        for (int32_t i = 0; i < cuListProp.cuNum; i++) {
            cuProp = &cuListProp.cuProps[i];
            udfCuProp = &udfCuListProp->cuProps[i];
            strncpy(cuProp->cuName, udfCuProp->cuName, XRM_MAX_NAME_LEN - 1);
            cuProp->devExcl = udfCuProp->devExcl;
            cuProp->deviceInfo = udfCuProp->deviceInfo;
            cuProp->memoryInfo = udfCuProp->memoryInfo;
            cuProp->requestLoadUnified = udfCuProp->requestLoadUnified;
            cuProp->requestLoadOriginal = udfCuProp->requestLoadOriginal;
            cuProp->clientId = cuGroupPropV2->clientId;
            cuProp->clientProcessId = cuGroupPropV2->clientProcessId;
            cuProp->poolId = cuGroupPropV2->poolId;
        }
        memset(cuGroupResV2, 0, sizeof(cuGroupResourceV2));
        /*
         * NOTE: here since the cuListResource has same member of cuGroupResource, so it's fine to
         * use cuGroupResV2 to call resAllocCuList() directly, otherwise should use cuListResource instead.
         */
        ret = resAllocCuListV2(&cuListProp, (cuListResourceV2*)cuGroupResV2);
        if (ret == XRM_SUCCESS) break;
    }
    return (ret);
}

/*
 * the interface to free one cu resource
 *
 * lock: need to enter lock to protect the resource pool access during the free process
 */
int32_t xrm::system::resReleaseCu(cuResource* cuRes) {
    if (cuRes == NULL) return (XRM_ERROR_INVALID);

    deviceData* dev;
    cuData* cu;
    int32_t devId, cuId, chanId;
    uint64_t allocServiceId;
    uint64_t clientId = cuRes->clientId;
    int32_t i, reserveIdx, procChanCnt = 0;
    int32_t ret;

    devId = cuRes->deviceId;
    if (devId < 0 || devId > m_numDevice) return (XRM_ERROR_INVALID);
    cuId = cuRes->cuId;
    if (cuId < 0 || cuId > XRM_MAX_XILINX_KERNELS) return (XRM_ERROR_INVALID);
    chanId = cuRes->channelId;
    if (chanId < 0 || chanId > XRM_MAX_KERNEL_CHANNELS) return (XRM_ERROR_INVALID);
    allocServiceId = cuRes->allocServiceId;

    ret = XRM_ERROR;
    dev = &m_devList[devId];
    if (!dev->isLoaded) {
        return (ret);
    }
    if (cuId >= dev->xclbinInfo.numCu) {
        return (ret);
    }
    cu = &dev->xclbinInfo.cuList[cuId];

    for (i = 0; i < XRM_MAX_KERNEL_CHANNELS; i++) {
        if (cu->channels[i].allocServiceId != allocServiceId || cu->channels[i].poolId != cuRes->poolId ||
            cu->channels[i].clientId != clientId || cu->channels[i].channelId != chanId) {
            if (cu->channels[i].clientId == clientId) /* count how many other channel used by the same client */
                procChanCnt++;
            continue;
        }

        /*
         * For the release cu:
         * if not from reserve pool, then return to Big pool
         * if from reserve pool, pool is active, return to pool
         * if from reserve pool, pool is in-active, return to Big pool
         */
        if (cuRes->poolId) {
            /* from the reserve pool */
            reserveIdx = isReservePoolUsingCu(cu, cuRes->poolId);
            if (reserveIdx != -1) {
                if (cu->reserves[reserveIdx].clientIsActive) {
                    /* From reserve pool, reserve client is active, return to reserve pool */
                    cu->reserves[reserveIdx].reserveUsedLoadUnified -= cu->channels[i].channelLoadUnified;
                } else {
                    /* From reserve pool, reserve client is NOT active, return resource to default pool */
                    cu->totalUsedLoadUnified -= cu->channels[i].channelLoadUnified;
                    updateDeviceLoad(cu->deviceId, -cu->channels[i].channelLoadUnified, -1);
                }
            } else {
                /* From reserve pool, reserve client is NOT active, return resource to default pool */
                cu->totalUsedLoadUnified -= cu->channels[i].channelLoadUnified;
                updateDeviceLoad(cu->deviceId, -cu->channels[i].channelLoadUnified, -1);
            }
        } else {
            /* return the resource into default pool */
            cu->totalUsedLoadUnified -= cu->channels[i].channelLoadUnified;
            updateDeviceLoad(cu->deviceId, -cu->channels[i].channelLoadUnified, -1);
        }
        cu->numChanInuse--;
        cu->channels[i].allocServiceId = 0;
        cu->channels[i].poolId = 0;
        cu->channels[i].clientId = 0;
        cu->channels[i].clientProcessId = 0;
        cu->channels[i].channelLoadUnified = 0;
        cu->channels[i].channelLoadOriginal = 0;

        /* update dev->clientProcs ref count */
        releaseClientOnDev(devId, clientId);

        ret = XRM_SUCCESS;
    }

    /* If there is NO other channel used by the same client */
    if (!procChanCnt) {
        /* update cu->clients */
        removeClientOnCu(cu, clientId);
    }

    return (ret);
}

/*
 * the interface to free one cu resource
 *
 * lock: need to enter lock to protect the resource pool access during the free process
 */
int32_t xrm::system::resReleaseCuV2(cuResource* cuRes) {
    int32_t ret = resReleaseCu(cuRes);
    return (ret);
}

int32_t xrm::system::resReleaseCuList(cuListResource* cuListRes) {
    int32_t i, ret;
    cuResource* cuRes;

    if (cuListRes == NULL) return (XRM_ERROR_INVALID);
    if (cuListRes->cuNum < 0 || cuListRes->cuNum > XRM_MAX_LIST_CU_NUM) return (XRM_ERROR_INVALID);
    /* No resource need to be released */
    if (cuListRes->cuNum == 0) return (XRM_SUCCESS);

    for (i = 0; i < cuListRes->cuNum; i++) {
        cuRes = &cuListRes->cuResources[i];
        ret = resReleaseCu(cuRes);
        if (ret != XRM_SUCCESS) break;
    }
    return (ret);
}

int32_t xrm::system::resReleaseCuListV2(cuListResourceV2* cuListResV2) {
    int32_t i, ret;
    cuResource* cuRes;

    if (cuListResV2 == NULL) return (XRM_ERROR_INVALID);
    if (cuListResV2->cuNum < 0 || cuListResV2->cuNum > XRM_MAX_LIST_CU_NUM_V2) return (XRM_ERROR_INVALID);
    /* No resource need to be released */
    if (cuListResV2->cuNum == 0) return (XRM_SUCCESS);

    for (i = 0; i < cuListResV2->cuNum; i++) {
        cuRes = &cuListResV2->cuResources[i];
        ret = resReleaseCuV2(cuRes);
        if (ret != XRM_SUCCESS) break;
    }
    return (ret);
}

int32_t xrm::system::resReleaseCuGroup(cuGroupResource* cuGroupRes) {
    int32_t i, ret;
    cuResource* cuRes;

    if (cuGroupRes == NULL) return (XRM_ERROR_INVALID);
    if (cuGroupRes->cuNum < 0 || cuGroupRes->cuNum > XRM_MAX_GROUP_CU_NUM) return (XRM_ERROR_INVALID);
    /* No resource need to be released */
    if (cuGroupRes->cuNum == 0) return (XRM_SUCCESS);

    for (i = 0; i < cuGroupRes->cuNum; i++) {
        cuRes = &cuGroupRes->cuResources[i];
        ret = resReleaseCu(cuRes);
        if (ret != XRM_SUCCESS) break;
    }
    return (ret);
}

int32_t xrm::system::resReleaseCuGroupV2(cuGroupResourceV2* cuGroupRes) {
    int32_t i, ret;
    cuResource* cuRes;

    if (cuGroupRes == NULL) return (XRM_ERROR_INVALID);
    if (cuGroupRes->cuNum < 0 || cuGroupRes->cuNum > XRM_MAX_GROUP_CU_NUM_V2) return (XRM_ERROR_INVALID);
    /* No resource need to be released */
    if (cuGroupRes->cuNum == 0) return (XRM_SUCCESS);

    for (i = 0; i < cuGroupRes->cuNum; i++) {
        cuRes = &cuGroupRes->cuResources[i];
        ret = resReleaseCuV2(cuRes);
        if (ret != XRM_SUCCESS) break;
    }
    return (ret);
}

/*
 * To get next free device for client, but do not allocate client on the device.
 *
 * The dev->clientProcs is NOT updated.
 *
 * Called while holding lock, so not enter lock in the function
 *
 */
int32_t xrm::system::allocDevForClient(int32_t* devId, cuProperty* cuProp) {
    if (cuProp == NULL) return (XRM_ERROR_INVALID);

    while (*devId < m_numDevice) {
        int ret;

        /* the next free device id is updated to "devId" */
        ret = getNextFreeDevForClient(devId, cuProp);
        if (ret < 0) {
            return (ret);
        }
        return (*devId);
    }

    return (XRM_ERROR);
}

/*
 * find next available device for specified client
 *
 * XRM_SUCCESS: find one free/available device, and set the id of this device to *devId
 * XRM_ERROR_NO_DEV: there is no free device
 */
int32_t xrm::system::getNextFreeDevForClient(int32_t* devId, cuProperty* cuProp) {
    if (cuProp == NULL) return (XRM_ERROR_INVALID);

    deviceData* dev = NULL;
    uint64_t clientId = cuProp->clientId;
    int32_t i;

    /* start search from next device: *devId + 1 */
    for (i = *devId >= 0 ? *devId + 1 : 0; i < m_numDevice; i++) {
        dev = &m_devList[i];
        if (!dev->isLoaded) continue;

        if (dev->isExcl) {
            if (dev->clientProcs[0].clientId == clientId) {
                /* the client is still inuse */
                *devId = i;
                return (XRM_SUCCESS);
            }
            continue;
        }
        *devId = i;
        return (XRM_SUCCESS);
    }
    return (XRM_ERROR_NO_DEV);
}

/*
 * To alloc the client resource on specified device
 *
 * The dev->clientProcs ref count is updated, it's used for dev alloc/release.
 */
int32_t xrm::system::allocClientFromDev(int32_t devId, cuProperty* cuProp) {
    if (devId < 0 || devId >= m_numDevice) {
        return (XRM_ERROR_INVALID);
    }
    if (cuProp == NULL) {
        return (XRM_ERROR_INVALID);
    }

    deviceData* deviceList = m_devList;
    uint64_t clientId = cuProp->clientId;
    pid_t clientProcessId = cuProp->clientProcessId;
    bool excl = cuProp->devExcl;
    int32_t pidIdx;
    int32_t ref;

    /* does process already have exclusive access? */
    if (deviceList[devId].isExcl) {
        if (deviceList[devId].clientProcs[0].clientId == clientId) {
            deviceList[devId].clientProcs[0].ref++;
            return (XRM_SUCCESS);
        } else {
            return (XRM_ERROR_NO_DEV);
        }
    }

    /*
     * register client as using exclusive device
     */
    if (excl) {
        ref = 0;
        /* is another client already using this as a non-exclusive device? */
        for (pidIdx = 0; pidIdx < XRM_MAX_DEV_CLIENTS; pidIdx++) {
            if (deviceList[devId].clientProcs[pidIdx].clientId &&
                deviceList[devId].clientProcs[pidIdx].clientId != clientId) {
                return (XRM_ERROR_NO_DEV);
            } else {
                if (deviceList[devId].clientProcs[pidIdx].clientId == clientId)
                    ref = deviceList[devId].clientProcs[pidIdx].ref;
            }
        }
        deviceList[devId].isExcl = true;
        memset(deviceList[devId].clientProcs, 0, sizeof(clientData) * XRM_MAX_DEV_CLIENTS);
        deviceList[devId].clientProcs[0].clientId = clientId;
        deviceList[devId].clientProcs[0].clientProcessId = clientProcessId;
        deviceList[devId].clientProcs[0].ref = ref + 1;
        return (XRM_SUCCESS);
    }

    /*
     * register client as using non-exclusive device
     */
    /* client is already using this non-exclusive device? */
    for (pidIdx = 0; pidIdx < XRM_MAX_DEV_CLIENTS; pidIdx++)
        if (deviceList[devId].clientProcs[pidIdx].clientId == clientId) {
            deviceList[devId].clientProcs[pidIdx].ref++;
            return (XRM_SUCCESS);
        }
    /* There maybe empty slot in dev->clientProcs */
    for (pidIdx = 0; pidIdx < XRM_MAX_DEV_CLIENTS; pidIdx++)
        if (!deviceList[devId].clientProcs[pidIdx].clientId) {
            deviceList[devId].clientProcs[pidIdx].clientId = clientId;
            deviceList[devId].clientProcs[pidIdx].clientProcessId = clientProcessId;
            deviceList[devId].clientProcs[pidIdx].ref = 1;
            return (XRM_SUCCESS);
        }

    return (XRM_ERROR_NO_DEV);
}

/*
 * To check whether the client process pid is still alive.
 * This function will NOT work within container environment.
 *
 * XRM_ERROR: client is not alive
 * XRM_SUCCESS: client is alive
 */
int32_t xrm::system::verifyProcess(pid_t pid) {
    struct stat statBuf;
    int32_t ret;
    char procfsPid[20] = {0};

    snprintf(procfsPid, sizeof(procfsPid), "/proc/%d", pid);
    ret = stat(procfsPid, &statBuf);
    if (ret && errno == ENOENT) {
        return (XRM_ERROR);
    }

    return (XRM_SUCCESS);
}

/*
 * Alloc one cu (compute unit) from specified device(devId) based on the request property.
 *
 * XRM_SUCCESS: allocated resouce is recorded by cuRes
 * Otherwise: failed to allocate the cu resource
 *
 * call while holding system lock
 */
int32_t xrm::system::allocCuFromDev(int32_t devId, cuProperty* cuProp, cuResource* cuRes) {
    deviceData* dev;
    cuData* cu;
    int32_t ret = 0;
    bool kernelNameEqual, kernelAliasEqual, cuNameEqual;
    int32_t cuId;
    bool cuAcquired = false;
    /* First pass will look for kernels already in-use by proc */
    bool cuAffinityPass = true;

    if (devId < 0 || devId >= m_numDevice) {
        return (XRM_ERROR_INVALID);
    }
    if ((cuProp == NULL) || (cuRes == NULL)) {
        logMsg(XRM_LOG_ERROR, "cuProp or cuRes is NULL\n");
        return (XRM_ERROR_INVALID);
    }
    if ((cuProp->kernelName[0] == '\0') && (cuProp->kernelAlias[0] == '\0') && (cuProp->cuName[0] == '\0')) {
        logMsg(XRM_LOG_ERROR, "None of kernel name, kernel alias and cu name are presented\n");
        return (XRM_ERROR_INVALID);
    }

    dev = &m_devList[devId];
cu_alloc_again:
    if (!cuAcquired) {
        /*
         * Now check whether matching cu is on allocated device
         * if not, free device, increment dev count, re-loop
         */
        for (cuId = 0; cuId < XRM_MAX_XILINX_KERNELS && cuId < dev->xclbinInfo.numCu && !cuAcquired; cuId++) {
            cu = &dev->xclbinInfo.cuList[cuId];

            /* first attempt to re-use existing kernels; else, use a new kernel */
            if ((cuAffinityPass && cu->numClient == 0) || (!cuAffinityPass && cu->numClient > 0)) continue;
            /*
             * compare, 0: equal
             *
             * kernel name is presented, compare it; otherwise no need to compare
             */
            kernelNameEqual = true;
            if (cuProp->kernelName[0] != '\0') {
                if (cu->kernelName.compare(cuProp->kernelName)) kernelNameEqual = false;
            }
            /* kernel alias is presented, compare it; otherwise no need to compare */
            kernelAliasEqual = true;
            if (cuProp->kernelAlias[0] != '\0') {
                if (cu->kernelAlias.compare(cuProp->kernelAlias)) kernelAliasEqual = false;
            }
            /* cu name is presented, compare it; otherwise no need to compare */
            cuNameEqual = true;
            if (cuProp->cuName[0] != '\0') {
                if (cu->cuName.compare(cuProp->cuName)) cuNameEqual = false;
            }
            if (!(kernelNameEqual && kernelAliasEqual && cuNameEqual)) continue;
            /* alloc channel and register client id */
            ret = allocChanClientFromCu(cu, cuProp, cuRes);
            if (ret != XRM_SUCCESS) continue;

            cuRes->deviceId = devId;
            cuRes->cuId = cuId;
            strncpy(cuRes->xclbinFileName, dev->xclbinName.c_str(), XRM_MAX_NAME_LEN - 1);
            strncpy(cuRes->uuidStr, dev->xclbinInfo.uuidStr.c_str(), XRM_MAX_NAME_LEN - 1);
            cuAcquired = true;
        }
    }

    if (!cuAcquired && cuAffinityPass) {
        cuAffinityPass = false; /* open up search to all kernels */
        goto cu_alloc_again;
    }

    if (cuAcquired) {
        return (XRM_SUCCESS);
    }

    return (XRM_ERROR_NO_KERNEL);
}

/*
 * call while holding system lock
 *
 * Alloc channel from cu, update cu resource pool.
 * if reservation id is set, then allocate the resource from reserve pool.
 * if the policyInfo is set, current cu load is better than precu load, it
 * will update precu to current cu and return XRM_FURTHER_CHECK
 *
 */
int32_t xrm::system::allocChanClientFromCu(
    cuData* cu, cuProperty* cuProp, cuResource* cuRes, uint64_t policyInfo, cuData** preCu) {
    uint64_t clientId = cuProp->clientId;
    pid_t clientProcessId = cuProp->clientProcessId;
    int32_t requestLoadUnified = cuProp->requestLoadUnified;
    int32_t requestLoadOriginal = cuProp->requestLoadOriginal;
    int32_t reserveIdx = 0;
    uint64_t reservePoolId = 0;
    int32_t chanId;

    if ((cu == NULL) || (cuProp == NULL) || (cuRes == NULL)) return (XRM_ERROR_INVALID);
    if (requestLoadUnified <= 0) return (XRM_ERROR_INVALID);

    reservePoolId = cuProp->poolId;
    if (reservePoolId) {
        reserveIdx = isReservePoolUsingCu(cu, reservePoolId);
        if (reserveIdx == -1) {
            return (XRM_ERROR_NO_KERNEL);
        }
        if (!cu->reserves[reserveIdx].clientIsActive) {
            return (XRM_ERROR_NO_KERNEL);
        }
        if (cu->reserves[reserveIdx].reserveUsedLoadUnified + requestLoadUnified >
            cu->reserves[reserveIdx].reserveLoadUnified) {
            return (XRM_ERROR_NO_KERNEL);
        }

        if ((policyInfo == XRM_POLICY_INFO_CONSTRAINT_TYPE_CU_MOST_USED_FIRST ||
             policyInfo == XRM_POLICY_INFO_CONSTRAINT_TYPE_DEV_MOST_USED_FIRST) &&
            cu->reserves[reserveIdx].reserveUsedLoadUnified + requestLoadUnified !=
                cu->reserves[reserveIdx].reserveLoadUnified) { // most used case
            if (*preCu == NULL || (cu->reserves[reserveIdx].reserveUsedLoadUnified + requestLoadUnified) >
                                      (*preCu)->reserves[reserveIdx].reserveUsedLoadUnified) {
                *preCu = cu;
            }
            return XRM_FURTHER_CHECK;
        }
        if ((policyInfo == XRM_POLICY_INFO_CONSTRAINT_TYPE_CU_LEAST_USED_FIRST ||
             policyInfo == XRM_POLICY_INFO_CONSTRAINT_TYPE_DEV_LEAST_USED_FIRST) &&
            cu->reserves[reserveIdx].reserveUsedLoadUnified != 0) { // least used case
            if (*preCu == NULL || (cu->reserves[reserveIdx].reserveUsedLoadUnified + requestLoadUnified) <
                                      (*preCu)->reserves[reserveIdx].reserveUsedLoadUnified) {
                *preCu = cu;
            }
            return XRM_FURTHER_CHECK;
        }
    } else {
        if (cu->totalUsedLoadUnified + requestLoadUnified > XRM_MAX_CHAN_LOAD_GRANULARITY_1000000) {
            return (XRM_ERROR_NO_KERNEL);
        }
        if ((policyInfo == XRM_POLICY_INFO_CONSTRAINT_TYPE_CU_MOST_USED_FIRST ||
             policyInfo == XRM_POLICY_INFO_CONSTRAINT_TYPE_DEV_MOST_USED_FIRST) &&
            cu->totalUsedLoadUnified + requestLoadUnified != XRM_MAX_CHAN_LOAD_GRANULARITY_1000000) {
            if (*preCu == NULL || (cu->totalUsedLoadUnified > (*preCu)->totalUsedLoadUnified)) {
                *preCu = cu;
            }
            return XRM_FURTHER_CHECK;
        }
        if ((policyInfo == XRM_POLICY_INFO_CONSTRAINT_TYPE_CU_LEAST_USED_FIRST ||
             policyInfo == XRM_POLICY_INFO_CONSTRAINT_TYPE_DEV_LEAST_USED_FIRST) &&
            cu->totalUsedLoadUnified != 0) {
            if (*preCu == NULL || (cu->totalUsedLoadUnified < (*preCu)->totalUsedLoadUnified)) {
                *preCu = cu;
            }
            return XRM_FURTHER_CHECK;
        }
    }
    if (cu->numChanInuse == 0) { /* unused kernel */
        chanId = 0;
        cu->channels[chanId].clientId = clientId;
        cu->channels[chanId].clientProcessId = clientProcessId;
        cu->channels[chanId].channelLoadUnified = requestLoadUnified;
        cu->channels[chanId].channelLoadOriginal = requestLoadOriginal;
        uint64_t allocServiceId = getNextAllocServiceId();
        cu->channels[chanId].allocServiceId = allocServiceId;
        cu->channels[chanId].poolId = reservePoolId;
        if (reservePoolId)
            cu->reserves[reserveIdx].reserveUsedLoadUnified += requestLoadUnified;
        else {
            cu->totalUsedLoadUnified += requestLoadUnified;
            updateDeviceLoad(cu->deviceId, requestLoadUnified, -1);
        }
        cu->numChanInuse++;
        /* Update cu->clients[], no empty slot in it */
        addClientToCu(cu, clientId);

        strncpy(cuRes->kernelName, cu->kernelName.c_str(), XRM_MAX_NAME_LEN - 1);
        strncpy(cuRes->instanceName, cu->instanceName.c_str(), XRM_MAX_NAME_LEN - 1);
        strncpy(cuRes->kernelAlias, cu->kernelAlias.c_str(), XRM_MAX_NAME_LEN - 1);
        strncpy(cuRes->cuName, cu->cuName.c_str(), XRM_MAX_NAME_LEN - 1);
        strncpy(cuRes->kernelPluginFileName, cu->kernelPluginFileName.c_str(), XRM_MAX_NAME_LEN - 1);
        cuRes->cuType = cu->cuType;
        cuRes->channelId = chanId;
        cuRes->clientId = clientId;
        cuRes->channelLoadUnified = requestLoadUnified;
        cuRes->channelLoadOriginal = requestLoadOriginal;
        cuRes->allocServiceId = allocServiceId;
        cuRes->baseAddr = cu->baseAddr;
        cuRes->membankId = cu->membankId;
        cuRes->membankType = cu->membankType;
        cuRes->membankSize = cu->membankSize;
        cuRes->membankBaseAddr = cu->membankBaseAddr;
        cuRes->poolId = reservePoolId;

        return (XRM_SUCCESS);
    } else if (cu->numChanInuse < XRM_MAX_KERNEL_CHANNELS) { /* not maxed the space */
                                                             /* kernel can support request load */
        chanId = cuFindFreeChannelId(cu);
        if (chanId < 0 || chanId > XRM_MAX_KERNEL_CHANNELS) {
            /* no free channel */
            return (XRM_ERROR_NO_KERNEL);
        }

        cu->channels[chanId].clientId = clientId;
        cu->channels[chanId].clientProcessId = clientProcessId;
        cu->channels[chanId].channelLoadUnified = requestLoadUnified;
        cu->channels[chanId].channelLoadOriginal = requestLoadOriginal;
        uint64_t allocServiceId = getNextAllocServiceId();
        cu->channels[chanId].allocServiceId = allocServiceId;
        cu->channels[chanId].poolId = reservePoolId;
        if (reservePoolId)
            cu->reserves[reserveIdx].reserveUsedLoadUnified += requestLoadUnified;
        else {
            cu->totalUsedLoadUnified += requestLoadUnified;
            updateDeviceLoad(cu->deviceId, requestLoadUnified, -1);
        }
        cu->numChanInuse++;
        /* Update cu->clients[], no empty slot in it */
        addClientToCu(cu, clientId);

        strncpy(cuRes->kernelName, cu->kernelName.c_str(), XRM_MAX_NAME_LEN - 1);
        strncpy(cuRes->instanceName, cu->instanceName.c_str(), XRM_MAX_NAME_LEN - 1);
        strncpy(cuRes->kernelAlias, cu->kernelAlias.c_str(), XRM_MAX_NAME_LEN - 1);
        strncpy(cuRes->cuName, cu->cuName.c_str(), XRM_MAX_NAME_LEN - 1);
        strncpy(cuRes->kernelPluginFileName, cu->kernelPluginFileName.c_str(), XRM_MAX_NAME_LEN - 1);
        cuRes->cuType = cu->cuType;
        cuRes->channelId = chanId;
        cuRes->clientId = clientId;
        cuRes->channelLoadUnified = requestLoadUnified;
        cuRes->channelLoadOriginal = requestLoadOriginal;
        cuRes->allocServiceId = allocServiceId;
        cuRes->baseAddr = cu->baseAddr;
        cuRes->membankId = cu->membankId;
        cuRes->membankType = cu->membankType;
        cuRes->membankSize = cu->membankSize;
        cuRes->membankBaseAddr = cu->membankBaseAddr;
        cuRes->poolId = reservePoolId;

        return (XRM_SUCCESS);
    }

    /* no free channel */
    return (XRM_ERROR_NO_KERNEL);
}

/*
 * Add client id at the end of cu->clients[] list. One client uses multiple
 * channel will only be recorded with one slot in cu->clients[], no reference
 * count recorded. So for remove, the caller need to make sure there is no other
 * channels are still used by this client.
 *
 * There is no empty slot from clients[0] to clients[numClient - 1]
 */
void xrm::system::addClientToCu(cuData* cu, uint64_t clientId) {
    int32_t i = -1;

    if (cu == NULL) return;

    i = isClientUsingCu(cu, clientId);
    if (i >= 0) return;

    /* advance clients pointer to next empty slot */
    for (i = 0; cu->clients[i]; i++)
        ;

    cu->clients[i] = clientId;
    cu->numClient++;

    return;
}

/*
 * check client is using the cu or not
 *
 * There is no empty slot from clients[0] to clients[numClient - 1]
 *
 * -1: not using
 * otherwise: return the position of client (clients[i] is using it)
 */
int32_t xrm::system::isClientUsingCu(cuData* cu, uint64_t clientId) {
    int32_t i;

    for (i = 0; i < cu->numClient && i < XRM_MAX_KERNEL_CHANNELS; i++) {
        if (cu->clients[i] != clientId) continue;

        return (i);
    }
    return (-1);
}

/*
 * To release client on specified device
 *
 * The dev->clientProcs ref count is updated.
 */
int32_t xrm::system::releaseClientOnDev(int32_t devId, uint64_t clientId) {
    if (devId < 0 || devId >= m_numDevice) return (XRM_ERROR_INVALID);
    deviceData* deviceList = m_devList;

    if (!deviceList[devId].isLoaded) return (XRM_ERROR_NO_DEV);

    if (deviceList[devId].isExcl) {
        if (deviceList[devId].clientProcs[0].clientId != clientId) {
            return (XRM_ERROR_INVALID);
        }
        deviceList[devId].clientProcs[0].ref--;
        if (deviceList[devId].clientProcs[0].ref == 0) {
            deviceList[devId].isExcl = false;
            deviceList[devId].clientProcs[0].clientId = 0;
            deviceList[devId].clientProcs[0].clientProcessId = 0;
        }
        return (XRM_SUCCESS);
    } else {
        for (int32_t pidIdx = 0; pidIdx < XRM_MAX_DEV_CLIENTS; pidIdx++)
            if (deviceList[devId].clientProcs[pidIdx].clientId == clientId) {
                deviceList[devId].clientProcs[pidIdx].ref--;
                if (deviceList[devId].clientProcs[pidIdx].ref == 0) {
                    deviceList[devId].clientProcs[pidIdx].clientId = 0;
                    deviceList[devId].clientProcs[pidIdx].clientProcessId = 0;
                }
                return (XRM_SUCCESS);
            }
    }
    return (XRM_ERROR_INVALID);
}

/*
 * Release all cu channels on specified device with specified client id
 * The cu->channels and cu->clients are updated
 */
void xrm::system::releaseAllCuChanClientOnDev(deviceData* dev, uint64_t clientId) {
    int32_t i, j;

    if (dev == NULL) return;

    for (i = 0; i < XRM_MAX_XILINX_KERNELS && i < dev->xclbinInfo.numCu; i++) {
        cuData* cu = &dev->xclbinInfo.cuList[i];

        /*
         * for each CU:
         * 1) resource NOT allocated from reserve pool: return to Big pool (default pool)
         * 2) resource allocated from reserve pool:
         *    2.1) reserve pool is active: return to reserver pool
         *    2.2) reserve pool is in-active: return to Big pool (default pool)
         */

        /* Determine if client is using this cu */
        if (clientId && (isClientUsingCu(cu, clientId) < 0)) continue;

        /* Update cu->clients, no empty slot in cu->clients */
        removeClientOnCu(cu, clientId);

        /* clear the channel entries, there maybe empty slot in cu->channels */
        for (j = 0; j < XRM_MAX_KERNEL_CHANNELS; j++) {
            uint64_t cu_client = cu->channels[j].clientId;

            /* clientId is 0, the channelLoadUnified are also 0 */
            if (!cu_client || cu_client != clientId) continue;

            /*
             * if not from reserve pool, then return to Big pool
             * if from reserve pool, pool is active, return to pool
             * if from reserve pool, pool is in-active, return to Big pool
             */
            if (cu->channels[j].poolId == 0) {
                /* Not allocated from reserve pool, then return to Big pool */
                cu->totalUsedLoadUnified -= cu->channels[j].channelLoadUnified;
                updateDeviceLoad(cu->deviceId, -cu->channels[j].channelLoadUnified, -1);
                cu->numChanInuse--;
                cu->channels[j].clientId = 0;
                cu->channels[j].clientProcessId = 0;
                cu->channels[j].channelLoadUnified = 0;
                cu->channels[j].channelLoadOriginal = 0;
                cu->channels[j].allocServiceId = 0;
                cu->channels[j].poolId = 0;
            } else {
                /* Allocated from reserve pool */
                int32_t reservePoolIdx = -1;
                for (int32_t reserveIdx = 0; reserveIdx < cu->numReserve; reserveIdx++) {
                    if (!cu->reserves[reserveIdx].clientIsActive) continue;
                    if (cu->reserves[reserveIdx].reservePoolId == cu->channels[j].poolId) {
                        reservePoolIdx = reserveIdx;
                        break;
                    }
                }

                if (reservePoolIdx != -1) {
                    /* from reserve pool, reserve pool is active, return to reserve pool */
                    cu->reserves[reservePoolIdx].reserveUsedLoadUnified -= cu->channels[j].channelLoadUnified;
                    cu->numChanInuse--;
                    cu->channels[j].clientId = 0;
                    cu->channels[j].clientProcessId = 0;
                    cu->channels[j].channelLoadUnified = 0;
                    cu->channels[j].channelLoadOriginal = 0;
                    cu->channels[j].allocServiceId = 0;
                    cu->channels[j].poolId = 0;
                } else {
                    /* from reserve pool, reserve pool is in-active, return to Big pool */
                    cu->totalUsedLoadUnified -= cu->channels[j].channelLoadUnified;
                    updateDeviceLoad(cu->deviceId, -cu->channels[j].channelLoadUnified, -1);
                    cu->numChanInuse--;
                    cu->channels[j].clientId = 0;
                    cu->channels[j].clientProcessId = 0;
                    cu->channels[j].channelLoadUnified = 0;
                    cu->channels[j].channelLoadOriginal = 0;
                    cu->channels[j].allocServiceId = 0;
                    cu->channels[j].poolId = 0;
                }
            } // end from reserve pool
        }     /* end channel clearing loop */
    }         /* end cu loop */
}

/*
 * Check whether client is using the cu or not, if yes, remove it from
 * cu->clients[] list and fill the hole. One client uses multiple
 * channel will only be recorded with one slot in cu->clients[], no reference
 * count recorded. So for remove, the caller need to make sure there is no other
 * channels are still used by this client.
 *
 * There is no empty slot from clients[0] to clients[numClient - 1]
 */
void xrm::system::removeClientOnCu(cuData* cu, uint64_t clientId) {
    if (cu == NULL) return;

    int32_t i = isClientUsingCu(cu, clientId);

    if (i < 0) {
        return;
    }

    /* Remove client and defragment list */
    for (; i < cu->numClient - 1 && i < XRM_MAX_KERNEL_CHANNELS - 1; i++) cu->clients[i] = cu->clients[i + 1];

    /* Zero the last item on the old list after defrag */
    cu->clients[i] = 0;
    cu->numClient--;

    return;
}

/*
 * check cu stat
 *
 * -1: not existing cu
 * otherwise: store the stat of the cu
 */
int32_t xrm::system::checkCuStat(cuResource* cuRes, cuStatus* cuStat) {
    if (cuRes == NULL || cuStat == NULL) return (XRM_ERROR_INVALID);

    int32_t devId, cuId;
    deviceData* dev;
    cuData* cu;

    devId = cuRes->deviceId;
    if (devId < 0 || devId > m_numDevice) return (XRM_ERROR_INVALID);

    cuId = cuRes->cuId;
    if (cuId < 0 || cuId > XRM_MAX_XILINX_KERNELS) return (XRM_ERROR_INVALID);

    dev = &m_devList[devId];
    cu = &dev->xclbinInfo.cuList[cuId];

    /* TODO:: need to get xrt/xma support to get the hardware stat */
    if (cu->totalUsedLoadUnified > 0)
        cuStat->isBusy = true;
    else
        cuStat->isBusy = false;
    cuStat->usedLoadUnified = cu->totalUsedLoadUnified;
    cuStat->usedLoadOriginal = (cu->totalUsedLoadUnified << 8);

    return (XRM_SUCCESS);
}

/*
 * increase number of concurrent client
 */
bool xrm::system::incNumConcurrentClient() {
    if (m_numConcurrentClient < m_limitConcurrentClient) {
        m_numConcurrentClient++;
        return (true);
    } else {
        logMsg(XRM_LOG_DEBUG, "Reach limit of concurrent client (%d)\n", m_limitConcurrentClient);
        return (false);
    }
}

/*
 * decrease number of concurrent client
 */
bool xrm::system::decNumConcurrentClient() {
    if (m_numConcurrentClient > 0) {
        m_numConcurrentClient--;
        return (true);
    } else {
        return (false);
    }
}

/*
 * get number of concurrent client
 *
 * Need to be proected by lock.
 */
uint32_t xrm::system::getNumConcurrentClient() {
    uint32_t numConcurrentClient;
    numConcurrentClient = m_numConcurrentClient;
    return (numConcurrentClient);
}

/*
 * get next allocation service id (0x1 - 0xFFFFFFFFFFFFFFFF)
 *
 * return: next allocation service id
 */
uint64_t xrm::system::getNextAllocServiceId() {
    if (m_allocServiceId == 0xFFFFFFFFFFFFFFFF)
        return (1);
    else
        return (m_allocServiceId + 1);
}

/*
 * update current allocation service id(0x1 - 0xFFFFFFFFFFFFFFFF)
 *
 */
void xrm::system::updateAllocServiceId() {
    if (m_allocServiceId == 0xFFFFFFFFFFFFFFFF)
        m_allocServiceId = 1;
    else
        m_allocServiceId++;
}

/*
 * Query allocated cu resource from system based on the query property.
 *
 * XRM_SUCCESS: queried resouces are filled into cuListRes
 * Otherwise: failed to query the allocated resource
 *
 * Lock: should enter lock during the resource allocation query
 */
int32_t xrm::system::resAllocationQuery(allocationQueryInfo* allocQuery, cuListResource* cuListRes) {
    int32_t devId, cuId, chanId;
    uint64_t allocServiceId;
    bool kernelNameEqual, kernelAliasEqual;
    deviceData* dev = NULL;
    cuData* cu = NULL;
    channelData* chan = NULL;
    cuResource* cuRes = NULL;
    int32_t cuNum;

    if (allocQuery == NULL || cuListRes == NULL) {
        return (XRM_ERROR_INVALID);
    }
    allocServiceId = allocQuery->allocServiceId;
    if (allocServiceId == 0) {
        return (XRM_ERROR_INVALID);
    }

    cuNum = 0;
    memset(cuListRes, 0, sizeof(cuListResource));
    /* Check all the devices */
    for (devId = 0; devId < m_numDevice; devId++) {
        dev = &m_devList[devId];
        /* Check all the cu on this device */
        for (cuId = 0; cuId < dev->xclbinInfo.numCu; cuId++) {
            cu = &dev->xclbinInfo.cuList[cuId];
            /* kernel name is presented, compare it; otherwise no need to compare */
            kernelNameEqual = true;
            if (allocQuery->kernelName[0] != '\0') {
                if (cu->kernelName.compare(allocQuery->kernelName)) kernelNameEqual = false;
            }
            /* kernel alias is presented, compare it; otherwise no need to compare */
            kernelAliasEqual = true;
            if (allocQuery->kernelAlias[0] != '\0') {
                if (cu->kernelAlias.compare(allocQuery->kernelAlias)) kernelAliasEqual = false;
            }
            if (!(kernelNameEqual && kernelAliasEqual)) continue;
            /* Check all the channels on this cu */
            for (chanId = 0; chanId < XRM_MAX_KERNEL_CHANNELS; chanId++) {
                chan = &cu->channels[chanId];
                if (chan->allocServiceId == allocServiceId) {
                    /* out of cu list limitation */
                    if (cuNum >= XRM_MAX_LIST_CU_NUM) {
                        memset(cuListRes, 0, sizeof(cuListResource));
                        return (XRM_ERROR);
                    }
                    cuRes = &cuListRes->cuResources[cuNum];
                    strncpy(cuRes->xclbinFileName, dev->xclbinName.c_str(), XRM_MAX_NAME_LEN - 1);
                    strncpy(cuRes->uuidStr, dev->xclbinInfo.uuidStr.c_str(), XRM_MAX_NAME_LEN - 1);
                    strncpy(cuRes->kernelPluginFileName, cu->kernelPluginFileName.c_str(), XRM_MAX_NAME_LEN - 1);
                    strncpy(cuRes->kernelName, cu->kernelName.c_str(), XRM_MAX_NAME_LEN - 1);
                    strncpy(cuRes->instanceName, cu->instanceName.c_str(), XRM_MAX_NAME_LEN - 1);
                    strncpy(cuRes->kernelAlias, cu->kernelAlias.c_str(), XRM_MAX_NAME_LEN - 1);
                    strncpy(cuRes->cuName, cu->cuName.c_str(), XRM_MAX_NAME_LEN - 1);
                    cuRes->cuType = cu->cuType;
                    cuRes->baseAddr = cu->baseAddr;
                    cuRes->membankId = cu->membankId;
                    cuRes->membankType = cu->membankType;
                    cuRes->membankSize = cu->membankSize;
                    cuRes->membankBaseAddr = cu->membankBaseAddr;
                    cuRes->deviceId = devId;
                    cuRes->cuId = cuId;
                    cuRes->channelId = chanId;
                    cuRes->allocServiceId = allocServiceId;
                    cuRes->poolId = chan->poolId;
                    cuRes->clientId = chan->clientId;
                    cuRes->channelLoadUnified = chan->channelLoadUnified;
                    cuRes->channelLoadOriginal = chan->channelLoadOriginal;
                    cuNum++;
                }
            }
        }
    }
    cuListRes->cuNum = cuNum;
    return (XRM_SUCCESS);
}

/*
 * Query allocated cu resource from system based on the query property.
 *
 * XRM_SUCCESS: queried resouces are filled into cuListRes
 * Otherwise: failed to query the allocated resource
 *
 * Lock: should enter lock during the resource allocation query
 */
int32_t xrm::system::resAllocationQueryV2(allocationQueryInfoV2* allocQueryV2, cuListResourceV2* cuListResV2) {
    int32_t devId, cuId, chanId;
    uint64_t allocServiceId;
    bool kernelNameEqual, kernelAliasEqual;
    deviceData* dev = NULL;
    cuData* cu = NULL;
    channelData* chan = NULL;
    cuResource* cuRes = NULL;
    int32_t cuNum;

    if (allocQueryV2 == NULL || cuListResV2 == NULL) {
        return (XRM_ERROR_INVALID);
    }
    allocServiceId = allocQueryV2->allocServiceId;
    if (allocServiceId == 0) {
        return (XRM_ERROR_INVALID);
    }

    cuNum = 0;
    memset(cuListResV2, 0, sizeof(cuListResourceV2));
    /* Check all the devices */
    for (devId = 0; devId < m_numDevice; devId++) {
        dev = &m_devList[devId];
        /* Check all the cu on this device */
        for (cuId = 0; cuId < dev->xclbinInfo.numCu; cuId++) {
            cu = &dev->xclbinInfo.cuList[cuId];
            /* kernel name is presented, compare it; otherwise no need to compare */
            kernelNameEqual = true;
            if (allocQueryV2->kernelName[0] != '\0') {
                if (cu->kernelName.compare(allocQueryV2->kernelName)) kernelNameEqual = false;
            }
            /* kernel alias is presented, compare it; otherwise no need to compare */
            kernelAliasEqual = true;
            if (allocQueryV2->kernelAlias[0] != '\0') {
                if (cu->kernelAlias.compare(allocQueryV2->kernelAlias)) kernelAliasEqual = false;
            }
            if (!(kernelNameEqual && kernelAliasEqual)) continue;
            /* Check all the channels on this cu */
            for (chanId = 0; chanId < XRM_MAX_KERNEL_CHANNELS; chanId++) {
                chan = &cu->channels[chanId];
                if (chan->allocServiceId == allocServiceId) {
                    /* out of cu list limitation */
                    if (cuNum >= XRM_MAX_LIST_CU_NUM_V2) {
                        memset(cuListResV2, 0, sizeof(cuListResource));
                        return (XRM_ERROR);
                    }
                    cuRes = &cuListResV2->cuResources[cuNum];
                    strncpy(cuRes->xclbinFileName, dev->xclbinName.c_str(), XRM_MAX_NAME_LEN - 1);
                    strncpy(cuRes->uuidStr, dev->xclbinInfo.uuidStr.c_str(), XRM_MAX_NAME_LEN - 1);
                    strncpy(cuRes->kernelPluginFileName, cu->kernelPluginFileName.c_str(), XRM_MAX_NAME_LEN - 1);
                    strncpy(cuRes->kernelName, cu->kernelName.c_str(), XRM_MAX_NAME_LEN - 1);
                    strncpy(cuRes->instanceName, cu->instanceName.c_str(), XRM_MAX_NAME_LEN - 1);
                    strncpy(cuRes->kernelAlias, cu->kernelAlias.c_str(), XRM_MAX_NAME_LEN - 1);
                    strncpy(cuRes->cuName, cu->cuName.c_str(), XRM_MAX_NAME_LEN - 1);
                    cuRes->cuType = cu->cuType;
                    cuRes->baseAddr = cu->baseAddr;
                    cuRes->membankId = cu->membankId;
                    cuRes->membankType = cu->membankType;
                    cuRes->membankSize = cu->membankSize;
                    cuRes->membankBaseAddr = cu->membankBaseAddr;
                    cuRes->deviceId = devId;
                    cuRes->cuId = cuId;
                    cuRes->channelId = chanId;
                    cuRes->allocServiceId = allocServiceId;
                    cuRes->poolId = chan->poolId;
                    cuRes->clientId = chan->clientId;
                    cuRes->channelLoadUnified = chan->channelLoadUnified;
                    cuRes->channelLoadOriginal = chan->channelLoadOriginal;
                    cuNum++;
                }
            }
        }
    }
    cuListResV2->cuNum = cuNum;
    return (XRM_SUCCESS);
}

/*
 * Recycle all resource from client whose connection is broken.
 */
void xrm::system::recycleResource(uint64_t clientId) {
    deviceData* dev = NULL;
    cuData* cu = NULL;
    int32_t devId;
    int64_t tmp_sum = 0;

    /* Check all the devices */
    for (devId = 0; devId < m_numDevice; devId++) {
        dev = &m_devList[devId];
        if (!dev->isLoaded) continue;

        /* relinquish all reserved resource from the client */
        /*
         * for each CU:
         * 1) no resource still allocated from pool of this client: reserved resource back to
         *    Big pool (default pool), de-active the client and remove reserve pool
         * 2) resource allocated from pool of this client: reserved but not allocated back
         *    to Big pool (default pool), de-active the client and remove reserve pool
         */
        for (int32_t cuId = 0; cuId < dev->xclbinInfo.numCu; cuId++) {
            cu = &dev->xclbinInfo.cuList[cuId];
            for (int32_t reserveIdx = 0; reserveIdx < cu->numReserve; reserveIdx++) {
                if (!cu->reserves[reserveIdx].clientIsActive) continue;
                if (cu->reserves[reserveIdx].clientId == clientId) {
                    int64_t tmp =
                        cu->reserves[reserveIdx].reserveLoadUnified - cu->reserves[reserveIdx].reserveUsedLoadUnified;
                    cu->totalUsedLoadUnified -= tmp;
                    tmp_sum -= tmp;
                    cu->totalReservedLoadUnified -= cu->reserves[reserveIdx].reserveLoadUnified;
                    /*
                     * To de-active the client and remove reserve pool.
                     * The allocated resource from this reserve pool will be directly return
                     * to Big pool in future (either during release or recycle) since the pool is de-active.
                     */
                    cu->reserves[reserveIdx].clientIsActive = false;
                    cu->reserves[reserveIdx].reserveLoadUnified = 0;

                    /* switch the slot until the last one */
                    int32_t i;
                    for (i = reserveIdx; i < cu->numReserve - 1; i++) {
                        cu->reserves[i].reserveLoadUnified = cu->reserves[i + 1].reserveLoadUnified;
                        cu->reserves[i].reserveUsedLoadUnified = cu->reserves[i + 1].reserveUsedLoadUnified;
                        cu->reserves[i].reservePoolId = cu->reserves[i + 1].reservePoolId;
                        cu->reserves[i].clientIsActive = cu->reserves[i + 1].clientIsActive;
                        cu->reserves[i].clientId = cu->reserves[i + 1].clientId;
                        cu->reserves[i].clientProcessId = cu->reserves[i + 1].clientProcessId;
                    }
                    /* empty the last slot */
                    cu->reserves[i].reserveLoadUnified = 0;
                    cu->reserves[i].reserveUsedLoadUnified = 0;
                    cu->reserves[i].reservePoolId = 0;
                    cu->reserves[i].clientIsActive = false;
                    cu->reserves[i].clientId = 0;
                    cu->reserves[i].clientProcessId = 0;
                    cu->numReserve--;
                    /* removed one slot, so set the reserveIdx to the right one */
                    reserveIdx--;
                }
            }
        } // endif relinquish

        updateDeviceLoad(devId, tmp_sum, 0);
        /* release all allocated resource from the client */
        /*
         * for each CU:
         * 1) resource NOT allocated from reserve pool: return to Big pool (default pool)
         * 2) resource allocated from reserve pool:
         *    2.1) reserve pool is active: return to reserver pool
         *    2.2) reserve pool is in-active: return to Big pool (default pool)
         */
        if (dev->isExcl) {
            if (dev->clientProcs[0].ref == 0) break;
            /* recycle the resource from the client */
            if (dev->clientProcs[0].clientId == clientId) {
                releaseAllCuChanClientOnDev(dev, clientId);
                dev->isExcl = false;
                memset(dev->clientProcs, 0, sizeof(clientData) * XRM_MAX_DEV_CLIENTS);
            }
        } else {
            for (int32_t i = 0; i < XRM_MAX_DEV_CLIENTS; i++) {
                if (dev->clientProcs[i].ref == 0) continue;
                /* recycle the resource from the client */
                if (dev->clientProcs[i].clientId == clientId) {
                    releaseAllCuChanClientOnDev(dev, clientId);
                    dev->clientProcs[i].clientId = 0;
                    dev->clientProcs[i].clientProcessId = 0;
                    dev->clientProcs[i].ref = 0;
                }
            }
        } // end of release
    }

    decNumConcurrentClient();
    /* The save() function is time cost operation, so decide to not it here. */
    // save();
}

/* the implementation of resource reserve and relinquish */

/*
 * get next reserve pool id (0x1 - 0xFFFFFFFFFFFFFFFF)
 *
 * return: next reservation id
 */
uint64_t xrm::system::getNextReservePoolId() {
    if (m_reservePoolId == 0xFFFFFFFFFFFFFFFF)
        return (1);
    else
        return (m_reservePoolId + 1);
}

/*
 * update current reservation id(0x1 - 0xFFFFFFFFFFFFFFFF)
 *
 */
void xrm::system::updateReservePoolId() {
    if (m_reservePoolId == 0xFFFFFFFFFFFFFFFF)
        m_reservePoolId = 1;
    else
        m_reservePoolId++;
}

/*
 * check reservation pool id is using the cu or not
 *
 * There is no empty slot from reserves[0] to reserves[numReserve - 1]
 *
 * The input reservePoolId > 0
 *
 * -1: not using
 * otherwise: return the index of reserve (reserves[i] is using it)
 */
int32_t xrm::system::isReservePoolUsingCu(cuData* cu, uint64_t reservePoolId) {
    int32_t i;

    if (reservePoolId < 1) return (-1);
    for (i = 0; i < cu->numReserve && i < XRM_MAX_KERNEL_RESERVES; i++) {
        if (cu->reserves[i].reservePoolId != reservePoolId) continue;

        return (i);
    }
    return (-1);
}

/*
 * call while holding system lock
 *
 * Reserve request load from on specified cu, update cu resource pool.
 *
 */
int32_t xrm::system::reserveLoadFromCu(uint64_t reservePoolId, cuData* cu, cuProperty* cuProp) {
    int32_t requestLoadUnified = cuProp->requestLoadUnified;
    int32_t reserveIdx;

    if ((cu == NULL) || (cuProp == NULL)) return (XRM_ERROR_INVALID);
    if (reservePoolId < 1) return (XRM_ERROR_INVALID);
    if (requestLoadUnified <= 0 || requestLoadUnified > XRM_MAX_CHAN_LOAD_GRANULARITY_1000000)
        return (XRM_ERROR_INVALID);

    if (((cu->totalUsedLoadUnified + requestLoadUnified) <= XRM_MAX_CHAN_LOAD_GRANULARITY_1000000) &&
        ((cu->totalReservedLoadUnified + requestLoadUnified) <= XRM_MAX_CHAN_LOAD_GRANULARITY_1000000)) {
        reserveIdx = isReservePoolUsingCu(cu, reservePoolId);
        if (reserveIdx != -1) {
            /* in use, update the existing reserve slot to record the information */
            cu->totalUsedLoadUnified += requestLoadUnified;
            updateDeviceLoad(cu->deviceId, requestLoadUnified, -1);
            cu->totalReservedLoadUnified += requestLoadUnified;
            cu->reserves[reserveIdx].reserveLoadUnified += requestLoadUnified;
            return (XRM_SUCCESS);
        } else if (cu->numReserve < XRM_MAX_KERNEL_RESERVES) {
            /* not in use, get a new reserve slot to record the information */
            cu->totalUsedLoadUnified += requestLoadUnified;
            updateDeviceLoad(cu->deviceId, requestLoadUnified, -1);
            cu->totalReservedLoadUnified += requestLoadUnified;
            cu->reserves[cu->numReserve].reserveLoadUnified = requestLoadUnified;
            cu->reserves[cu->numReserve].reservePoolId = reservePoolId;
            cu->reserves[cu->numReserve].clientIsActive = true;
            cu->reserves[cu->numReserve].clientId = cuProp->clientId;
            cu->reserves[cu->numReserve].clientProcessId = cuProp->clientProcessId;
            cu->numReserve++;
            return (XRM_SUCCESS);
        }
    }

    /* over load */
    return (XRM_ERROR_NO_KERNEL);
}

/*
 * Reserve one CU (Compute Unit) based on the request property.
 *
 * Please be noted that the reservation will record the client information
 * (clientId) in the cu->reserves[].
 *
 * return:
 *   XRM_SUCCESS: reserve the cu resouce and associate it with the reserve pool id
 *   Otherwise: failed to reserve the cu resource with specified pool id
 *
 * Lock: should already in lock protection during the cu reservation
 */
int32_t xrm::system::resReserveCu(uint64_t reservePoolId, cuProperty* cuProp, uint64_t* fromDevId) {
    deviceData* dev;
    cuData* cu;
    int32_t ret = 0;
    bool kernelNameEqual, kernelAliasEqual;
    int32_t devId, cuId;
    bool cuAcquired = false;
    /* First pass will look for kernels already in-use by proc */
    bool cuAffinityPass = true;

    if (cuProp == NULL) {
        logMsg(XRM_LOG_ERROR, "cuProp or cuRes is NULL\n");
        return (XRM_ERROR_INVALID);
    }
    if ((cuProp->kernelName[0] == '\0') && (cuProp->kernelAlias[0] == '\0')) {
        logMsg(XRM_LOG_ERROR, "neither kernel name nor alias are presented\n");
        return (XRM_ERROR_INVALID);
    }

cu_reserve_loop:
    for (devId = -1; !cuAcquired && (devId < m_numDevice);) {
        devId = allocDevForClient(&devId, cuProp);
        if (devId < 0) {
            break;
        }

        dev = &m_devList[devId];
        *fromDevId = (uint64_t)devId;
        /*
         * Now check whether matching cu is on allocated device
         * if not, free device, increment dev count, re-loop
         */
        for (cuId = 0; cuId < XRM_MAX_XILINX_KERNELS && cuId < dev->xclbinInfo.numCu && !cuAcquired; cuId++) {
            cu = &dev->xclbinInfo.cuList[cuId];

            /* first attempt to re-use existing kernels; else, use a new kernel */
            if ((cuAffinityPass && cu->numClient == 0) || (!cuAffinityPass && cu->numClient > 0)) continue;
            /*
             * compare, 0: equal
             *
             * kernel name is presented, compare it; otherwise no need to compare
             */
            kernelNameEqual = true;
            if (cuProp->kernelName[0] != '\0') {
                if (cu->kernelName.compare(cuProp->kernelName)) kernelNameEqual = false;
            }
            /* kernel alias is presented, compare it; otherwise no need to compare */
            kernelAliasEqual = true;
            if (cuProp->kernelAlias[0] != '\0') {
                if (cu->kernelAlias.compare(cuProp->kernelAlias)) kernelAliasEqual = false;
            }
            if (!(kernelNameEqual && kernelAliasEqual)) continue;
            /* Reserve request load from the specified cu */
            ret = reserveLoadFromCu(reservePoolId, cu, cuProp);
            if (ret != XRM_SUCCESS) {
                continue;
            }

            cuAcquired = true;
        }
    }

    if (!cuAcquired && cuAffinityPass) {
        cuAffinityPass = false; /* open up search to all kernels */
        goto cu_reserve_loop;
    }

    if (cuAcquired) {
        return (XRM_SUCCESS);
    }

    return (XRM_ERROR_NO_KERNEL);
}

/*
 * Reserve one CU (Compute Unit) from specified device(devId) based on the request property.
 *
 * Please be noted that the reservation will NOT record the client information
 * (clientId etc).
 *
 * return:
 *   XRM_SUCCESS: reserve the cu resouce and associate it with the reserve pool id
 *   Otherwise: failed to reserve the cu resource with specified reserve pool id
 *
 * Lock: should already in lock protection during the cu reservation
 */
int32_t xrm::system::reserveCuFromDev(uint64_t reservePoolId, int32_t devId, cuProperty* cuProp) {
    deviceData* dev;
    cuData* cu;
    int32_t ret = 0;
    bool kernelNameEqual, kernelAliasEqual;
    int32_t cuId;
    bool cuAcquired = false;
    /* First pass will look for kernels already in-use by proc */
    bool cuAffinityPass = true;

    if (devId < 0 || devId >= m_numDevice) {
        return (XRM_ERROR_INVALID);
    }
    if (cuProp == NULL) {
        logMsg(XRM_LOG_ERROR, "cuProp or cuRes is NULL\n");
        return (XRM_ERROR_INVALID);
    }
    if ((cuProp->kernelName[0] == '\0') && (cuProp->kernelAlias[0] == '\0')) {
        logMsg(XRM_LOG_ERROR, "neither kernel name nor alias are presented\n");
        return (XRM_ERROR_INVALID);
    }

    dev = &m_devList[devId];
cu_reserve_again:
    if (!cuAcquired) {
        /*
         * Now check whether matching cu is on allocated device
         * if not, free device, increment dev count, re-loop
         */
        for (cuId = 0; cuId < XRM_MAX_XILINX_KERNELS && cuId < dev->xclbinInfo.numCu && !cuAcquired; cuId++) {
            cu = &dev->xclbinInfo.cuList[cuId];

            /* first attempt to re-use existing kernels; else, use a new kernel */
            if ((cuAffinityPass && cu->numClient == 0) || (!cuAffinityPass && cu->numClient > 0)) continue;
            /*
             * compare, 0: equal
             *
             * kernel name is presented, compare it; otherwise no need to compare
             */
            kernelNameEqual = true;
            if (cuProp->kernelName[0] != '\0') {
                if (cu->kernelName.compare(cuProp->kernelName)) kernelNameEqual = false;
            }
            /* kernel alias is presented, compare it; otherwise no need to compare */
            kernelAliasEqual = true;
            if (cuProp->kernelAlias[0] != '\0') {
                if (cu->kernelAlias.compare(cuProp->kernelAlias)) kernelAliasEqual = false;
            }
            if (!(kernelNameEqual && kernelAliasEqual)) continue;
            /* Reserve request load from the specified cu */
            ret = reserveLoadFromCu(reservePoolId, cu, cuProp);
            if (ret != XRM_SUCCESS) {
                continue;
            }
            cuAcquired = true;
        }
    }

    if (!cuAcquired && cuAffinityPass) {
        cuAffinityPass = false; /* open up search to all kernels */
        goto cu_reserve_again;
    }

    if (cuAcquired) {
        return (XRM_SUCCESS);
    }

    return (XRM_ERROR_NO_KERNEL);
}

/*
 * Reserve one cu list based on the request property.
 *
 * Please be noted that the reservation will NOT record the client information
 * (clientId etc).
 *
 * return:
 *   XRM_SUCCESS: reserve the cu list and associate it with the reserve pool id
 *   Otherwise: failed to reserve the cu list with specified reserve pool id
 *
 * Lock: should already in lock protection during the cu list reservation.
 */
int32_t xrm::system::resReserveCuList(uint64_t reservePoolId, cuListProperty* cuListProp) {
    int32_t i, devId, ret;
    cuProperty* cuProp = NULL;

    if (cuListProp == NULL) {
        return (XRM_ERROR_INVALID);
    }
    if (cuListProp->cuNum <= 0 || cuListProp->cuNum > XRM_MAX_LIST_CU_NUM) {
        return (XRM_ERROR_INVALID);
    }

    ret = XRM_ERROR;
    if (cuListProp->sameDevice) {
        /*
         * To reserve the cu list from same device
         */
        for (devId = -1; devId < m_numDevice;) {
            /* find one available device for first cu */
            cuProp = &cuListProp->cuProps[0];
            devId = allocDevForClient(&devId, cuProp);
            if (devId < 0) {
                return (ret);
            }
            /* reserve the cu list from this device */
            for (i = 0; i < cuListProp->cuNum; i++) {
                cuProp = &cuListProp->cuProps[i];
                ret = reserveCuFromDev(reservePoolId, devId, cuProp);
                if (ret != XRM_SUCCESS) {
                    resRelinquishCuList(reservePoolId);
                    break;
                }
            }
            if (ret == XRM_SUCCESS) {
                return (ret);
            }
        }
        return (ret);
    } else {
        /*
         * To reserve the cu list from any device
         */
        uint64_t fromDevId;
        for (i = 0; i < cuListProp->cuNum; i++) {
            cuProp = &cuListProp->cuProps[i];
            ret = resReserveCu(reservePoolId, cuProp, &fromDevId);
            if (ret != XRM_SUCCESS) {
                resRelinquishCuList(reservePoolId);
                return (ret);
            }
        }
        return (ret);
    }
}

/*
 * Reserve one cu list based on the request property.
 *
 * Please be noted that the reservation will NOT record the client information
 * (clientId etc).
 *
 * return:
 *   XRM_SUCCESS: reserve the cu list and associate it with the reserve pool id
 *   Otherwise: failed to reserve the cu list with specified reserve pool id
 *
 * Lock: should already in lock protection during the cu list reservation.
 */
int32_t xrm::system::resReserveCuSubListFromSameDevice(uint64_t reservePoolId,
                                                       cuSubListPropertyV2* cuSubListProp,
                                                       deviceIdListPropertyV2* usedDevIdList,
                                                       uint64_t* fromDevId) {
    int32_t i, devId, ret;
    int32_t relinquishIdx, relinquishRet;
    cuProperty* cuProp = NULL;

    if (cuSubListProp == NULL || usedDevIdList == NULL) {
        return (XRM_ERROR_INVALID);
    }
    if (cuSubListProp->cuNum <= 0 || cuSubListProp->cuNum > XRM_MAX_LIST_CU_NUM_V2 || usedDevIdList->deviceNum < 0 ||
        usedDevIdList->deviceNum > XRM_MAX_XILINX_DEVICES) {
        return (XRM_ERROR_INVALID);
    }

    ret = XRM_ERROR;
    /*
     * To reserve the cu sub list from same device
     */
    for (devId = -1; devId < m_numDevice;) {
        /* find one available device for first cu */
    allocate_device:
        cuProp = &cuSubListProp->cuProps[0];
        devId = allocDevForClient(&devId, cuProp);
        if (devId < 0) {
            return (ret);
        }
        /* check whether the device is in used devId list, if yes, then find another one */
        if (isInDeviceIdList(usedDevIdList, devId)) {
            goto allocate_device;
        }

        /* reserve the cu list from this device */
        for (i = 0; i < cuSubListProp->cuNum; i++) {
            cuProp = &cuSubListProp->cuProps[i];
            ret = reserveCuFromDev(reservePoolId, devId, cuProp);
            if (ret != XRM_SUCCESS) {
                /* relinquish the cu resouce just reserved */
                for (relinquishIdx = 0; relinquishIdx < i; relinquishIdx++) {
                    cuProp = &cuSubListProp->cuProps[relinquishIdx];
                    relinquishRet = relinquishCuFromDev(reservePoolId, devId, cuProp);
                    if (relinquishRet != XRM_SUCCESS) {
                        logMsg(XRM_LOG_ERROR, "%s: failed to relinquish cu [%d]\n", __func__, relinquishIdx);
                        return (ret);
                    }
                }
                break;
            }
        }
        if (ret == XRM_SUCCESS) {
            *fromDevId = (uint64_t)devId;
            return (ret);
        }
    }
    return (ret);
}

/*
 * Reserve one cu list based on the request property.
 *
 * Please be noted that the reservation will NOT record the client information
 * (clientId etc).
 *
 * For each CU:
 * 1) if device constraint not set: reserve it from any device
 * 2) if device constraint set with hw index: reserve it from target device
 * 3) if device constraint set with virtual index: find all others with same virtual index, put them into
 *    one sub-list, and reserve requested CU from same device, the device id should be different from that
 *    for previous sub-lists request from virtual index.
 *
 * XRM_SUCCESS: allocated resouce is filled into cuListRes
 * Otherwise: failed to allocate the cu list
 *
 * Lock: should enter lock during the cu list allocation
 */
int32_t xrm::system::resReserveCuListV2(uint64_t reservePoolId,
                                        cuListPropertyV2* cuListPropV2,
                                        cuListResInforV2* cuListResInforV2) {
    int32_t index, ret;
    cuPropertyV2* cuPropV2 = NULL;
    cuProperty cuProp;
    uint64_t deviceInfoConstraintType, deviceInfoConstraintTypeNext;
    uint64_t deviceInfoDeviceIndex, deviceInfoDeviceIndexNext;
    deviceIdListPropertyV2* usedDevIdList = NULL;
    cuSubListPropertyV2* cuSubListProp;
    itemFlag* itemFlags;
    int32_t subListCuNum;
    int32_t subIndex, oriIndex;
    uint64_t fromDevId;

    if ((cuListPropV2 == NULL) || (cuListResInforV2 == NULL)) {
        return (XRM_ERROR_INVALID);
    }
    if (cuListPropV2->cuNum <= 0 || cuListPropV2->cuNum > XRM_MAX_LIST_CU_NUM_V2) {
        return (XRM_ERROR_INVALID);
    }

    /* use to set the flag to record whether the item is handled */
    itemFlags = (itemFlag*)malloc(sizeof(itemFlag) * XRM_MAX_LIST_CU_NUM_V2);
    memset(itemFlags, 0, sizeof(itemFlag) * XRM_MAX_LIST_CU_NUM_V2);

    /* use to record used device id for sub-list which request from different virtual device index */
    usedDevIdList = (deviceIdListPropertyV2*)malloc(sizeof(deviceIdListPropertyV2));
    memset(usedDevIdList, 0, sizeof(deviceIdListPropertyV2));

    ret = XRM_ERROR;

    /*
     * To reserve the requested cu:
     * 1) if device constraint not set: reserve it from any device
     * 2) if device constraint set with hw index: reserve it from target device
     * 3) if device constraint set with virtual index: find all others with same virtual index, put them into
     *    one sub-list, and reserve requested CU from same device, the device id should be different from that
     *    for previous sub-lists request from virtual index.
     */
    for (index = 0; index < cuListPropV2->cuNum; index++) {
        if (itemFlags[index].handled) continue;
        itemFlags[index].handled = true;
        cuPropV2 = &cuListPropV2->cuProps[index];
        deviceInfoConstraintType =
            (((cuPropV2->deviceInfo) >> XRM_DEVICE_INFO_CONSTRAINT_TYPE_SHIFT) & XRM_DEVICE_INFO_CONSTRAINT_TYPE_MASK);
        deviceInfoDeviceIndex =
            (((cuPropV2->deviceInfo) >> XRM_DEVICE_INFO_DEVICE_INDEX_SHIFT) & XRM_DEVICE_INFO_DEVICE_INDEX_MASK);
        switch (deviceInfoConstraintType) {
            case XRM_DEVICE_INFO_CONSTRAINT_TYPE_NULL:
                cuPropertyCopyFromV2(&cuProp, cuPropV2);
                ret = resReserveCu(reservePoolId, &cuProp, &fromDevId);
                cuListResInforV2->cuResInfor[index].deviceId = fromDevId;
                break;
            case XRM_DEVICE_INFO_CONSTRAINT_TYPE_HARDWARE_DEVICE_INDEX:
                cuPropertyCopyFromV2(&cuProp, cuPropV2);
                ret = reserveCuFromDev(reservePoolId, (int32_t)deviceInfoDeviceIndex, &cuProp);
                cuListResInforV2->cuResInfor[index].deviceId = deviceInfoDeviceIndex;
                break;
            case XRM_DEVICE_INFO_CONSTRAINT_TYPE_VIRTUAL_DEVICE_INDEX:
                /*
                 * This is the first cu with different virtual device index from privious CUes.
                 * Here need create a new sub-list to handle all cu with same virtual device.
                 */
                cuSubListProp = (cuSubListPropertyV2*)malloc(sizeof(cuSubListPropertyV2));
                memset(cuSubListProp, 0, sizeof(cuSubListPropertyV2));

                /* put current cu property into sub list */
                subListCuNum = 1;
                cuPropertyCopyFromV2(&cuSubListProp->cuProps[subListCuNum - 1], cuPropV2);
                cuSubListProp->indexInOriList[subListCuNum - 1] = index;

                /*
                 * look through the rest of the original list, put the cu property into sub list if
                 * virtual device index is same as the current one.
                 */
                for (subIndex = index + 1; subIndex < cuListPropV2->cuNum; subIndex++) {
                    if (itemFlags[subIndex].handled) continue;
                    cuPropV2 = &cuListPropV2->cuProps[subIndex];
                    deviceInfoConstraintTypeNext = (((cuPropV2->deviceInfo) >> XRM_DEVICE_INFO_CONSTRAINT_TYPE_SHIFT) &
                                                    XRM_DEVICE_INFO_CONSTRAINT_TYPE_MASK);
                    deviceInfoDeviceIndexNext = (((cuPropV2->deviceInfo) >> XRM_DEVICE_INFO_DEVICE_INDEX_SHIFT) &
                                                 XRM_DEVICE_INFO_DEVICE_INDEX_MASK);
                    if ((deviceInfoConstraintTypeNext == XRM_DEVICE_INFO_CONSTRAINT_TYPE_VIRTUAL_DEVICE_INDEX) &&
                        (deviceInfoDeviceIndexNext == deviceInfoDeviceIndex)) {
                        /* put the cu property into sub list */
                        subListCuNum++;
                        cuPropertyCopyFromV2(&cuSubListProp->cuProps[subListCuNum - 1], cuPropV2);
                        cuSubListProp->indexInOriList[subListCuNum - 1] = subIndex;
                        itemFlags[subIndex].handled = true;
                    }
                }
                cuSubListProp->cuNum = subListCuNum;
                ret = resReserveCuSubListFromSameDevice(reservePoolId, cuSubListProp, usedDevIdList, &fromDevId);
                if (ret == XRM_SUCCESS) {
                    // record the used device id (all cu in this sub list from same device)
                    insertDeviceIdList(usedDevIdList, fromDevId);
                    // record the device id infor with original index
                    for (subIndex = 0; subIndex < cuSubListProp->cuNum; subIndex++) {
                        oriIndex = cuSubListProp->indexInOriList[subIndex];
                        cuListResInforV2->cuResInfor[oriIndex].deviceId = fromDevId;
                    }
                }
                free(cuSubListProp);
                break;
            default:
                logMsg(XRM_LOG_ERROR, "invalid device info [%d] constraint type\n", index);
                ret = XRM_ERROR_INVALID;
                break;
        }
        if (ret != XRM_SUCCESS) break;
    }
    if (ret != XRM_SUCCESS) resRelinquishCuListV2(reservePoolId);
    free(itemFlags);
    free(usedDevIdList);
    return (ret);
}

/*
 * relinquish one CU (Compute Unit) from specified device(devId) based on the request property.
 *
 * lock: should already in lock protection during the cu relinquish process
 */
int32_t xrm::system::relinquishCuFromDev(uint64_t reservePoolId, int32_t devId, cuProperty* cuProp) {
    deviceData* dev;
    cuData* cu;
    bool kernelNameEqual, kernelAliasEqual;
    int32_t cuId;
    int32_t ret = XRM_ERROR;

    if (reservePoolId == 0) {
        logMsg(XRM_LOG_ERROR, "reservePoolId is invalid\n");
        return (XRM_ERROR_INVALID);
    }
    if (devId < 0 || devId >= m_numDevice) {
        logMsg(XRM_LOG_ERROR, "devId is invalid\n");
        return (XRM_ERROR_INVALID);
    }
    if (cuProp == NULL) {
        logMsg(XRM_LOG_ERROR, "cuProp is NULL\n");
        return (XRM_ERROR_INVALID);
    }
    if ((cuProp->kernelName[0] == '\0') && (cuProp->kernelAlias[0] == '\0')) {
        logMsg(XRM_LOG_ERROR, "neither kernel name nor alias are presented\n");
        return (XRM_ERROR_INVALID);
    }

    dev = &m_devList[devId];
    if (!dev->isLoaded) {
        logMsg(XRM_LOG_ERROR, "device [%d] is not loaded\n", devId);
        return (ret);
    }
    for (cuId = 0; cuId < dev->xclbinInfo.numCu; cuId++) {
        cu = &dev->xclbinInfo.cuList[cuId];
        kernelNameEqual = true;
        if (cuProp->kernelName[0] != '\0') {
            if (cu->kernelName.compare(cuProp->kernelName)) kernelNameEqual = false;
        }
        /* kernel alias is presented, compare it; otherwise no need to compare */
        kernelAliasEqual = true;
        if (cuProp->kernelAlias[0] != '\0') {
            if (cu->kernelAlias.compare(cuProp->kernelAlias)) kernelAliasEqual = false;
        }
        if (!(kernelNameEqual && kernelAliasEqual)) continue;

        for (int32_t i = 0; i < cu->numReserve; i++) {
            if ((cu->reserves[i].reservePoolId == reservePoolId) &&
                (cu->reserves[i].reserveLoadUnified == cuProp->requestLoadUnified)) {
                /* there maybe same cu reserved, try others */
                if (cu->reserves[i].reserveUsedLoadUnified) {
                    continue;
                }
                cu->totalUsedLoadUnified -= cu->reserves[i].reserveLoadUnified;
                updateDeviceLoad(devId, -cu->reserves[i].reserveLoadUnified, -1);
                cu->totalReservedLoadUnified -= cu->reserves[i].reserveLoadUnified;
                /* switch the slot until the last one */
                for (; i < cu->numReserve - 1; i++) {
                    cu->reserves[i].reserveLoadUnified = cu->reserves[i + 1].reserveLoadUnified;
                    cu->reserves[i].reserveUsedLoadUnified = cu->reserves[i + 1].reserveUsedLoadUnified;
                    cu->reserves[i].reservePoolId = cu->reserves[i + 1].reservePoolId;
                    cu->reserves[i].clientIsActive = cu->reserves[i + 1].clientIsActive;
                    cu->reserves[i].clientId = cu->reserves[i + 1].clientId;
                    cu->reserves[i].clientProcessId = cu->reserves[i + 1].clientProcessId;
                }
                /* empty the last slot */
                cu->reserves[i].reserveLoadUnified = 0;
                cu->reserves[i].reserveUsedLoadUnified = 0;
                cu->reserves[i].reservePoolId = 0;
                cu->reserves[i].clientIsActive = false;
                cu->reserves[i].clientId = 0;
                cu->reserves[i].clientProcessId = 0;
                cu->numReserve--;
                /* relinquished the cu successfully */
                ret = XRM_SUCCESS;
                return (ret);
            }
        }
    }
    return (ret);
}

/*
 * the interface to relinquish cu list resource based on the reserve pool id,
 * it will actually relinquish whole cu pool resource.
 *
 * lock: should already in lock protection during the cu list relinquish process
 */
int32_t xrm::system::resRelinquishCuList(uint64_t reservePoolId) {
    if (reservePoolId == 0) return (XRM_ERROR_INVALID);

    deviceData* dev;
    cuData* cu;
    int32_t devId, cuId;

    for (devId = 0; devId < m_numDevice; devId++) {
        dev = &m_devList[devId];
        if (!dev->isLoaded) continue;
        for (cuId = 0; cuId < dev->xclbinInfo.numCu; cuId++) {
            cu = &dev->xclbinInfo.cuList[cuId];
            for (int32_t i = 0; i < cu->numReserve; i++) {
                if (cu->reserves[i].reservePoolId == reservePoolId) {
                    if (cu->reserves[i].reserveUsedLoadUnified) {
                        return (XRM_ERROR);
                    }
                    cu->totalUsedLoadUnified -= cu->reserves[i].reserveLoadUnified;
                    updateDeviceLoad(devId, -cu->reserves[i].reserveLoadUnified, -1);
                    cu->totalReservedLoadUnified -= cu->reserves[i].reserveLoadUnified;
                    /* switch the slot until the last one */
                    for (; i < cu->numReserve - 1; i++) {
                        cu->reserves[i].reserveLoadUnified = cu->reserves[i + 1].reserveLoadUnified;
                        cu->reserves[i].reserveUsedLoadUnified = cu->reserves[i + 1].reserveUsedLoadUnified;
                        cu->reserves[i].reservePoolId = cu->reserves[i + 1].reservePoolId;
                        cu->reserves[i].clientIsActive = cu->reserves[i + 1].clientIsActive;
                        cu->reserves[i].clientId = cu->reserves[i + 1].clientId;
                        cu->reserves[i].clientProcessId = cu->reserves[i + 1].clientProcessId;
                    }
                    /* empty the last slot */
                    cu->reserves[i].reserveLoadUnified = 0;
                    cu->reserves[i].reserveUsedLoadUnified = 0;
                    cu->reserves[i].reservePoolId = 0;
                    cu->reserves[i].clientIsActive = false;
                    cu->reserves[i].clientId = 0;
                    cu->reserves[i].clientProcessId = 0;
                    cu->numReserve--;
                    break;
                }
            }
        }
    }
    return (XRM_SUCCESS);
}

/*
 * the interface to relinquish cu list resource based on the reserve pool id,
 * it will actually relinquish whole cu pool resource.
 *
 * lock: should already in lock protection during the cu list relinquish process
 */
int32_t xrm::system::resRelinquishCuListV2(uint64_t reservePoolId) {
    int32_t ret = resRelinquishCuList(reservePoolId);
    return (ret);
}

/*
 * Reserve CU (Compute Unit) of one whole xclbin based on the request property.
 *
 * Please be noted that the reservation will NOT record the client information
 * (clientId etc).
 *
 * return:
 *   XRM_SUCCESS: reserve the cu resouce and associate it with the reserve pool id
 *   Otherwise: failed to reserve the cu resource with specified reserve pool id
 *
 * Lock: should already in lock protection during the cu reservation
 */
int32_t xrm::system::resReserveCuOfXclbin(
    uint64_t reservePoolId, uuid_t uuid, uint64_t clientId, pid_t clientProcessId, uint64_t* fromDevId) {
    deviceData* dev;
    cuData* cu;
    int32_t cuId;

    if (reservePoolId == 0) {
        logMsg(XRM_LOG_ERROR, "%s(), reserve pool is is 0\n", __func__);
        return (XRM_ERROR_INVALID);
    }

    for (int32_t devId = 0; devId < m_numDevice; devId++) {
        dev = &m_devList[devId];
        if (!dev->isLoaded) continue;
        /* To check whether the device is loaded with xclbin of specified uuid */
        if (!isDeviceLoadedXclbinWithUuid(devId, uuid)) continue;
        /* Whether device is already in used */
        if (isDeviceBusy(devId)) continue;
        /* reserve the all the cu on this device */
        for (cuId = 0; cuId < dev->xclbinInfo.numCu; cuId++) {
            cu = &dev->xclbinInfo.cuList[cuId];
            cu->totalUsedLoadUnified = XRM_MAX_CU_LOAD_GRANULARITY_1000000; // will update dev load at end of loop
            cu->totalReservedLoadUnified = XRM_MAX_CU_LOAD_GRANULARITY_1000000;
            cu->reserves[0].reserveLoadUnified = XRM_MAX_CU_LOAD_GRANULARITY_1000000;
            cu->reserves[0].reserveUsedLoadUnified = 0;
            cu->reserves[0].reservePoolId = reservePoolId;
            cu->reserves[0].clientIsActive = true;
            cu->reserves[0].clientId = clientId;
            cu->reserves[0].clientProcessId = clientProcessId;
            cu->numReserve = 1;
        }
        updateDeviceLoad(devId, 0, dev->xclbinInfo.numCu * XRM_MAX_CU_LOAD_GRANULARITY_1000000);
        *fromDevId = devId;
        return (XRM_SUCCESS);
    }

    return (XRM_ERROR_NO_KERNEL);
}

/*
 * Reserve CU (Compute Unit) of one whole device based on the request property.
 *
 * Please be noted that the reservation will NOT record the client information
 * (clientId etc).
 *
 * return:
 *   XRM_SUCCESS: reserve the cu resouce and associate it with the reserve pool id
 *   Otherwise: failed to reserve the cu resource with specified reserve pool id
 *
 * Lock: should already in lock protection during the cu reservation
 */
int32_t xrm::system::resReserveDevice(uint64_t reservePoolId,
                                      uint64_t deviceId,
                                      uint64_t clientId,
                                      pid_t clientProcessId) {
    deviceData* dev;
    int32_t devId = (int32_t)(deviceId & 0xFFFFFFFF);
    cuData* cu;
    int32_t cuId;

    if (reservePoolId == 0) {
        logMsg(XRM_LOG_ERROR, "%s(), reserve pool is is 0\n", __func__);
        return (XRM_ERROR_INVALID);
    }

    if (devId < 0 || devId > (XRM_MAX_XILINX_DEVICES - 1)) {
        logMsg(XRM_LOG_ERROR, "%s(), device id %d is out of range\n", __func__, devId);
        return (XRM_ERROR_INVALID);
    }

    dev = &m_devList[devId];
    if (!dev->isLoaded) return (XRM_ERROR_NO_KERNEL);
    /* Whether device is already in used */
    if (isDeviceBusy(devId)) return (XRM_ERROR_NO_KERNEL);
    /* reserve the all the cu on this device */
    for (cuId = 0; cuId < dev->xclbinInfo.numCu; cuId++) {
        cu = &dev->xclbinInfo.cuList[cuId];
        cu->totalUsedLoadUnified = XRM_MAX_CU_LOAD_GRANULARITY_1000000; // end of loop for dev load
        cu->totalReservedLoadUnified = XRM_MAX_CU_LOAD_GRANULARITY_1000000;
        cu->reserves[0].reserveLoadUnified = XRM_MAX_CU_LOAD_GRANULARITY_1000000;
        cu->reserves[0].reserveUsedLoadUnified = 0;
        cu->reserves[0].reservePoolId = reservePoolId;
        cu->reserves[0].clientIsActive = true;
        cu->reserves[0].clientId = clientId;
        cu->reserves[0].clientProcessId = clientProcessId;
        cu->numReserve = 1;
    }
    updateDeviceLoad(devId, 0, dev->xclbinInfo.numCu * XRM_MAX_CU_LOAD_GRANULARITY_1000000);
    return (XRM_SUCCESS);
}

/*
 * Reserve one cu pool based on the request property.
 *
 * Please be noted that the reservation will NOT record the client information
 * (clientId etc).
 *
 *   reservePoolId: reserve cu pool successed
 *   0: failed to reserve the cu pool resource
 *
 * Lock: should enter lock during the cu pool reservation process
 */
uint64_t xrm::system::resReserveCuPool(cuPoolProperty* cuPoolProp) {
    int32_t ret;
    uint64_t reservePoolId = 0;
    cuListProperty* cuListProp = NULL;
    int32_t reserveCuListNum = 0;
    int32_t reserveXclbinNum = 0;

    if (cuPoolProp == NULL) {
        return (reservePoolId);
    }
    if (cuPoolProp->cuListNum < 0 || cuPoolProp->xclbinNum < 0) {
        return (reservePoolId);
    }
    cuListProp = &(cuPoolProp->cuListProp);
    if (cuListProp->cuNum < 0 || cuListProp->cuNum > XRM_MAX_LIST_CU_NUM) {
        return (reservePoolId);
    }

    ret = XRM_ERROR;
    reservePoolId = getNextReservePoolId();
    uint64_t fromDevId;
    /*
     * to reserve whole device at first, then reserve the cu list in case cu list
     * resource available on multiple different devices.
     */
    while (reserveXclbinNum < cuPoolProp->xclbinNum) {
        /*
         * To reserve all the cu resources of one xclbin
         */
        ret = resReserveCuOfXclbin(reservePoolId, cuPoolProp->xclbinUuid, cuPoolProp->clientId,
                                   cuPoolProp->clientProcessId, &fromDevId);
        if (ret != XRM_SUCCESS) {
            resRelinquishCuPool(reservePoolId);
            return (0);
        }
        reserveXclbinNum++;
    }
    while (reserveCuListNum < cuPoolProp->cuListNum) {
        /*
         * To reserve the cu list
         */
        ret = resReserveCuList(reservePoolId, cuListProp);
        if (ret != XRM_SUCCESS) {
            resRelinquishCuPool(reservePoolId);
            return (0);
        }
        reserveCuListNum++;
    }

    updateReservePoolId();
    return (reservePoolId);
}

/*
 * Reserve one cu pool based on the request property.
 *
 * Please be noted that the reservation will NOT record the client information
 * (clientId etc).
 *
 *   reservePoolId: reserve cu pool successed
 *   0: failed to reserve the cu pool resource
 *
 * Lock: should enter lock during the cu pool reservation process
 */
uint64_t xrm::system::resReserveCuPoolV2(cuPoolPropertyV2* cuPoolProp, cuPoolResInforV2* cuPoolResInfor) {
    int32_t ret;
    uint64_t reservePoolId = 0;
    cuListPropertyV2* cuListProp = NULL;
    deviceIdListPropertyV2* deviceIdListProp = NULL;
    int32_t reserveCuListNum = 0;
    int32_t reserveXclbinNum = 0;
    cuListResInforV2* cuListResInfor = NULL;

    if (cuPoolProp == NULL) {
        return (reservePoolId);
    }
    if (cuPoolProp->cuListNum < 0 || cuPoolProp->xclbinNum < 0 || cuPoolProp->deviceIdListProp.deviceNum < 0 ||
        cuPoolProp->deviceIdListProp.deviceNum > XRM_MAX_XILINX_DEVICES) {
        return (reservePoolId);
    }
    cuListProp = &(cuPoolProp->cuListProp);
    if (cuListProp->cuNum < 0 || cuListProp->cuNum > XRM_MAX_LIST_CU_NUM_V2) {
        return (reservePoolId);
    }

    ret = XRM_ERROR;
    reservePoolId = getNextReservePoolId();
    uint64_t fromDevId;
    /*
     * first to reserve specified device (hardware device id);
     * second to reserve whole device (xclbin uuid);
     * third to reserve the cu list in case cu list
     * resource available on multiple different devices.
     */
    deviceIdListProp = &(cuPoolProp->deviceIdListProp);
    for (int32_t i = 0; i < deviceIdListProp->deviceNum; i++) {
        /*
         * To reserve all the cu resources of one device
         */
        ret = resReserveDevice(reservePoolId, deviceIdListProp->deviceIds[i], cuPoolProp->clientId,
                               cuPoolProp->clientProcessId);
        if (ret != XRM_SUCCESS) {
            resRelinquishCuPoolV2(reservePoolId);
            return (0);
        }
    }
    while (reserveXclbinNum < cuPoolProp->xclbinNum) {
        /*
         * To reserve all the cu resources of one xclbin
         */
        ret = resReserveCuOfXclbin(reservePoolId, cuPoolProp->xclbinUuid, cuPoolProp->clientId,
                                   cuPoolProp->clientProcessId, &fromDevId);
        if (ret != XRM_SUCCESS) {
            resRelinquishCuPoolV2(reservePoolId);
            return (0);
        }
        cuPoolResInfor->xclbinResInfor[reserveXclbinNum].deviceId = fromDevId;
        reserveXclbinNum++;
    }
    cuPoolResInfor->xclbinNum = cuPoolProp->xclbinNum;
    while (reserveCuListNum < cuPoolProp->cuListNum) {
        /*
         * To reserve the cu list
         */
        cuListResInfor = &(cuPoolResInfor->cuListResInfor[reserveCuListNum]);
        ret = resReserveCuListV2(reservePoolId, cuListProp, cuListResInfor);
        if (ret != XRM_SUCCESS) {
            resRelinquishCuPoolV2(reservePoolId);
            return (0);
        }
        cuListResInfor->cuNum = cuListProp->cuNum;
        reserveCuListNum++;
    }
    cuPoolResInfor->cuListNum = cuPoolProp->cuListNum;

    updateReservePoolId();
    return (reservePoolId);
}

/*
 * the interface to relinquish cu pool resource based on the reservation id
 *
 * lock: need to enter lock to protect the resource pool access during the relinquish process
 */
int32_t xrm::system::resRelinquishCuPool(uint64_t reservePoolId) {
    if (reservePoolId == 0) return (XRM_ERROR_INVALID);

    deviceData* dev;
    cuData* cu;
    int32_t devId, cuId;

    for (devId = 0; devId < m_numDevice; devId++) {
        dev = &m_devList[devId];
        if (!dev->isLoaded) continue;
        for (cuId = 0; cuId < dev->xclbinInfo.numCu; cuId++) {
            cu = &dev->xclbinInfo.cuList[cuId];
            for (int32_t i = 0; i < cu->numReserve; i++) {
                if (cu->reserves[i].reservePoolId == reservePoolId) {
                    if (cu->reserves[i].reserveUsedLoadUnified) {
                        return (XRM_ERROR);
                    }
                    cu->totalUsedLoadUnified -= cu->reserves[i].reserveLoadUnified;
                    updateDeviceLoad(devId, -cu->reserves[i].reserveLoadUnified, -1);
                    cu->totalReservedLoadUnified -= cu->reserves[i].reserveLoadUnified;
                    /* switch the slot until the last one */
                    for (; i < cu->numReserve - 1; i++) {
                        cu->reserves[i].reserveLoadUnified = cu->reserves[i + 1].reserveLoadUnified;
                        cu->reserves[i].reserveUsedLoadUnified = cu->reserves[i + 1].reserveUsedLoadUnified;
                        cu->reserves[i].reservePoolId = cu->reserves[i + 1].reservePoolId;
                        cu->reserves[i].clientIsActive = cu->reserves[i + 1].clientIsActive;
                        cu->reserves[i].clientId = cu->reserves[i + 1].clientId;
                        cu->reserves[i].clientProcessId = cu->reserves[i + 1].clientProcessId;
                    }
                    /* empty the last slot */
                    cu->reserves[i].reserveLoadUnified = 0;
                    cu->reserves[i].reserveUsedLoadUnified = 0;
                    cu->reserves[i].reservePoolId = 0;
                    cu->reserves[i].clientIsActive = false;
                    cu->reserves[i].clientId = 0;
                    cu->reserves[i].clientProcessId = 0;
                    cu->numReserve--;
                    break;
                }
            }
        }
    }
    return (XRM_SUCCESS);
}

/*
 * the interface to relinquish cu list resource based on the reservation id
 *
 * lock: need to enter lock to protect the resource pool access during the relinquish process
 */
int32_t xrm::system::resRelinquishCuPoolV2(uint64_t reservePoolId) {
    int32_t ret = resRelinquishCuPool(reservePoolId);
    return (ret);
}

/*
 * Query reserve cu resource from system based on the reserve pool id.
 *
 * XRM_SUCCESS: queried resouces are filled into cuPoolRes
 * Otherwise: failed to query the reserve resource
 *
 * Lock: should enter lock during the resource allocation query
 */
int32_t xrm::system::resReservationQuery(uint64_t reservePoolId, cuPoolResource* cuPoolRes) {
    int32_t devId, cuId, reserveIdx;
    deviceData* dev = NULL;
    cuData* cu = NULL;
    reserveData* reserve = NULL;
    cuResource* cuRes = NULL;
    int32_t cuNum;

    if (cuPoolRes == NULL) {
        return (XRM_ERROR_INVALID);
    }
    if (reservePoolId == 0) {
        return (XRM_ERROR_INVALID);
    }

    cuNum = 0;
    memset(cuPoolRes, 0, sizeof(cuPoolResource));
    /* Check all the devices */
    for (devId = 0; devId < m_numDevice; devId++) {
        dev = &m_devList[devId];
        /* Check all the cu on this device */
        for (cuId = 0; cuId < dev->xclbinInfo.numCu; cuId++) {
            cu = &dev->xclbinInfo.cuList[cuId];
            /* Check all the reservations on this cu */
            for (reserveIdx = 0; reserveIdx < cu->numReserve; reserveIdx++) {
                reserve = &cu->reserves[reserveIdx];
                if (reserve->reservePoolId == reservePoolId) {
                    /* out of cu pool limitation */
                    if (cuNum >= XRM_MAX_POOL_CU_NUM) {
                        break;
                        /* to just return max number of cu, instead of return error.
                         * // memset(cuPoolRes, 0, sizeof(cuPoolResource));
                         * // return (XRM_ERROR);
                         */
                    }
                    cuRes = &cuPoolRes->cuResources[cuNum];
                    strncpy(cuRes->xclbinFileName, dev->xclbinName.c_str(), XRM_MAX_NAME_LEN - 1);
                    strncpy(cuRes->uuidStr, dev->xclbinInfo.uuidStr.c_str(), XRM_MAX_NAME_LEN - 1);
                    strncpy(cuRes->kernelPluginFileName, cu->kernelPluginFileName.c_str(), XRM_MAX_NAME_LEN - 1);
                    strncpy(cuRes->kernelName, cu->kernelName.c_str(), XRM_MAX_NAME_LEN - 1);
                    strncpy(cuRes->instanceName, cu->instanceName.c_str(), XRM_MAX_NAME_LEN - 1);
                    strncpy(cuRes->kernelAlias, cu->kernelAlias.c_str(), XRM_MAX_NAME_LEN - 1);
                    strncpy(cuRes->cuName, cu->cuName.c_str(), XRM_MAX_NAME_LEN - 1);
                    cuRes->cuType = cu->cuType;
                    cuRes->baseAddr = cu->baseAddr;
                    cuRes->membankId = cu->membankId;
                    cuRes->membankType = cu->membankType;
                    cuRes->membankSize = cu->membankSize;
                    cuRes->membankBaseAddr = cu->membankBaseAddr;
                    cuRes->deviceId = devId;
                    cuRes->cuId = cuId;
                    cuRes->poolId = reservePoolId;
                    cuNum++;
                }
            }
        }
    }
    cuPoolRes->cuNum = cuNum;
    return (XRM_SUCCESS);
}

/*
 * Query reserve cu resource from system based on the reserve pool id.
 *
 * XRM_SUCCESS: queried resouces are filled into cuPoolRes
 * Otherwise: failed to query the reserve resource
 *
 * Lock: should enter lock during the resource allocation query
 */
int32_t xrm::system::resReservationQueryV2(reservationQueryInfoV2* reserveQueryInfo, cuPoolResourceV2* cuPoolRes) {
    int32_t devId, cuId, reserveIdx;
    deviceData* dev = NULL;
    cuData* cu = NULL;
    bool kernelNameEqual, kernelAliasEqual;
    reserveData* reserve = NULL;
    cuResource* cuRes = NULL;
    int32_t cuNum;

    if (reserveQueryInfo == NULL) {
        return (XRM_ERROR_INVALID);
    }
    if (cuPoolRes == NULL) {
        return (XRM_ERROR_INVALID);
    }
    if (reserveQueryInfo->poolId == 0) {
        return (XRM_ERROR_INVALID);
    }

    cuNum = 0;
    memset(cuPoolRes, 0, sizeof(cuPoolResource));
    /* Check all the devices */
    for (devId = 0; devId < m_numDevice; devId++) {
        dev = &m_devList[devId];
        /* Check all the cu on this device */
        for (cuId = 0; cuId < dev->xclbinInfo.numCu; cuId++) {
            cu = &dev->xclbinInfo.cuList[cuId];
            /* kernel name is presented, compare it; otherwise no need to compare */
            kernelNameEqual = true;
            if (reserveQueryInfo->kernelName[0] != '\0') {
                if (cu->kernelName.compare(reserveQueryInfo->kernelName)) kernelNameEqual = false;
            }
            /* kernel alias is presented, compare it; otherwise no need to compare */
            kernelAliasEqual = true;
            if (reserveQueryInfo->kernelAlias[0] != '\0') {
                if (cu->kernelAlias.compare(reserveQueryInfo->kernelAlias)) kernelAliasEqual = false;
            }
            if (!(kernelNameEqual && kernelAliasEqual)) continue;
            /* Check all the reservations on this cu */
            for (reserveIdx = 0; reserveIdx < cu->numReserve; reserveIdx++) {
                reserve = &cu->reserves[reserveIdx];
                if (reserve->reservePoolId == reserveQueryInfo->poolId) {
                    /* out of cu pool limitation */
                    if (cuNum >= XRM_MAX_POOL_CU_NUM_V2) {
                        break;
                        /* to just return max number of cu, instead of return error.
                         * // memset(cuPoolRes, 0, sizeof(cuPoolResource));
                         * // return (XRM_ERROR);
                         */
                    }
                    cuRes = &cuPoolRes->cuResources[cuNum];
                    strncpy(cuRes->xclbinFileName, dev->xclbinName.c_str(), XRM_MAX_NAME_LEN - 1);
                    strncpy(cuRes->uuidStr, dev->xclbinInfo.uuidStr.c_str(), XRM_MAX_NAME_LEN - 1);
                    strncpy(cuRes->kernelPluginFileName, cu->kernelPluginFileName.c_str(), XRM_MAX_NAME_LEN - 1);
                    strncpy(cuRes->kernelName, cu->kernelName.c_str(), XRM_MAX_NAME_LEN - 1);
                    strncpy(cuRes->instanceName, cu->instanceName.c_str(), XRM_MAX_NAME_LEN - 1);
                    strncpy(cuRes->kernelAlias, cu->kernelAlias.c_str(), XRM_MAX_NAME_LEN - 1);
                    strncpy(cuRes->cuName, cu->cuName.c_str(), XRM_MAX_NAME_LEN - 1);
                    cuRes->cuType = cu->cuType;
                    cuRes->baseAddr = cu->baseAddr;
                    cuRes->membankId = cu->membankId;
                    cuRes->membankType = cu->membankType;
                    cuRes->membankSize = cu->membankSize;
                    cuRes->membankBaseAddr = cu->membankBaseAddr;
                    cuRes->deviceId = devId;
                    cuRes->cuId = cuId;
                    cuRes->poolId = reserveQueryInfo->poolId;
                    cuNum++;
                }
            }
        }
    }
    cuPoolRes->cuNum = cuNum;
    return (XRM_SUCCESS);
}

/*
 * flush the information data of xrm plugin
 */
void xrm::system::flushXrmPluginInfo(uint32_t xrmPluginId) {
    m_xrmPluginList[xrmPluginId].xrmPluginFileName = "";
    m_xrmPluginList[xrmPluginId].xrmPluginName = "";
    memset(&m_xrmPluginList[xrmPluginId].pluginData, 0, sizeof(xrmPluginData));
}

/*
 * Load one xrm plugin
 *
 * Return:
 *   XRM_SUCCESS: successed to load one xrm plugin
 *   XRM_ERROR: failed to load one xrm plugin
 *
 */
int32_t xrm::system::loadOneXrmPlugin(std::string& xrmPluginFileName, std::string& xrmPluginName, std::string& errmsg) {
    int32_t ret = XRM_ERROR;
    char* errorMsg;
    uint32_t pluginVersion = 0;

    /* check whether plugin loading reaches limitation */
    if (m_numXrmPlugin >= XRM_MAX_PLUGIN_NUM) {
        logMsg(XRM_LOG_ERROR, "%s: Plugin loading has reached the limitation (%d)\n", __func__, XRM_MAX_PLUGIN_NUM);
        errmsg += "Plugin loading has reached the limitation (" + std::to_string(XRM_MAX_PLUGIN_NUM) + ")";
        return (ret);
    }

    /* check whether plugin is loaded */
    for (uint32_t pluginId = 0; pluginId < m_numXrmPlugin; pluginId++) {
        if (m_xrmPluginList[pluginId].xrmPluginName.compare(xrmPluginName.c_str()) == 0) {
            pluginVersion = m_xrmPluginList[pluginId].pluginData.pluginVersion();
            logMsg(XRM_LOG_ERROR, "%s: plugin %s (version %d) is already loaded\n", __func__, xrmPluginName.c_str(),
                   pluginVersion);
            errmsg += "plugin " + xrmPluginName + " (version " + std::to_string(pluginVersion) + ") is already loaded";
            return (ret);
        }
    }

    /* load plugin library file */
    void* pluginHandle = dlopen(xrmPluginFileName.c_str(), (RTLD_GLOBAL | RTLD_NOW));
    if (!pluginHandle) {
        errorMsg = dlerror();
        logMsg(XRM_LOG_ERROR, "%s: Failed to open plugin %s\n Error msg: %s\n", __func__, xrmPluginFileName.c_str(),
               errorMsg);
        errmsg += errorMsg;
        return (ret);
    }
    xrmPluginData* plugin = (xrmPluginData*)dlsym(pluginHandle, xrmPluginName.c_str());
    if ((errorMsg = dlerror()) != NULL) {
        logMsg(XRM_LOG_ERROR, "%s: Failed to get plugin named %s\n Error msg: %s\n", __func__, xrmPluginName.c_str(),
               errorMsg);
        errmsg += errorMsg;
        return (ret);
    }

    /* check the api version */
    if (plugin->apiVersion == NULL) {
        logMsg(XRM_LOG_ERROR, "%s: Plugin library must provide api version function\n", __func__);
        errmsg += "There is No API version function in plugin";
        return (ret);
    }
    int32_t apiVersion = plugin->apiVersion();
    if (apiVersion != XRM_API_VERSION_1) {
        logMsg(XRM_LOG_ERROR, "%s: Plugin api version is not matching: %d\n", __func__, apiVersion);
        errmsg += "Plugin API version not match";
        return (ret);
    }

    /* check the plugin version */
    if (plugin->pluginVersion == NULL) {
        logMsg(XRM_LOG_ERROR, "%s: Plugin library must provide version function\n", __func__);
        errmsg += "There is No plugin version function in plugin";
        return (ret);
    }
    m_xrmPluginList[m_numXrmPlugin].xrmPluginFileName = xrmPluginFileName;
    m_xrmPluginList[m_numXrmPlugin].xrmPluginName = xrmPluginName;
    memcpy(&m_xrmPluginList[m_numXrmPlugin].pluginData, plugin, sizeof(xrmPluginData));
    m_numXrmPlugin++;
    ret = XRM_SUCCESS;
    return (ret);
}

/*
 * Load xrm plugins - loaded plugin recored in m_xrmPluginList[], starting from [0], no hole
 *
 * Return:
 *   XRM_SUCCESS: successed to load all xrm plugins
 *   XRM_ERROR: failed to load all xrm plugins
 *
 * the load process need to be protected by lock
 */
int32_t xrm::system::loadXrmPlugins(pt::ptree& loadPluginsTree, std::string& errmsg) {
    int32_t ret = XRM_ERROR;
    std::string xrmPluginFileName, xrmPluginName;
    std::string loadErrmsg;

    for (auto it : loadPluginsTree) {
        auto kv = it.second;
        xrmPluginFileName = kv.get<std::string>("xrmPluginFileName", "");
        if (xrmPluginFileName.c_str()[0] == '\0') {
            errmsg += "request parameters xrm plugin file name is not provided";
            ret = XRM_ERROR_INVALID;
            break;
        }

        xrmPluginName = kv.get<std::string>("xrmPluginName", "");
        if (xrmPluginName.c_str()[0] == '\0') {
            errmsg += "request parameters xrm plugin name is not provided";
            ret = XRM_ERROR_INVALID;
            break;
        }

        ret = loadOneXrmPlugin(xrmPluginFileName, xrmPluginName, loadErrmsg);
        if (ret != XRM_SUCCESS) {
            errmsg += loadErrmsg;
            break;
        }
    }
    return (ret);
}

/*
 * Unload one xrm plugin
 *
 * Return:
 *   XRM_SUCCESS: successed to unload one plugin
 *   XRM_ERROR: failed to unload one plugin
 *
 */
int32_t xrm::system::unloadOneXrmPlugin(std::string& xrmPluginName, std::string& errmsg) {
    int32_t ret = XRM_ERROR;
    bool foundPlugin = false;
    uint32_t pluginId = 0;

    /* find the plugin */
    for (pluginId = 0; pluginId < m_numXrmPlugin; pluginId++) {
        if (m_xrmPluginList[pluginId].xrmPluginName.compare(xrmPluginName.c_str()) == 0) {
            foundPlugin = true;
            break;
        }
    }
    if (!foundPlugin) {
        errmsg += "XRM Plugin not loaded";
        return (ret);
    }

    /* move the slot forward */
    for (; pluginId < (m_numXrmPlugin - 1); pluginId++) {
        memcpy(&m_xrmPluginList[pluginId].pluginData, &m_xrmPluginList[pluginId + 1].pluginData, sizeof(xrmPluginData));
        m_xrmPluginList[pluginId].xrmPluginFileName = m_xrmPluginList[pluginId + 1].xrmPluginFileName;
        m_xrmPluginList[pluginId].xrmPluginName = m_xrmPluginList[pluginId + 1].xrmPluginName;
    }

    /* clear the last slot */
    flushXrmPluginInfo(pluginId);
    m_numXrmPlugin--;
    ret = XRM_SUCCESS;
    return (ret);
}

/*
 * Unload xrm plugins - unloaded xrm plugin removed from m_xrmPluginList[], no hole
 *
 * Return:
 *   XRM_SUCCESS: successed to unload all xrm plugins
 *   XRM_ERROR: failed to unload all xrm plugins
 *
 * the unload process need to be protected by lock
 */
int32_t xrm::system::unloadXrmPlugins(pt::ptree& unloadPluginsTree, std::string& errmsg) {
    int32_t ret = XRM_ERROR;
    std::string xrmPluginName;
    std::string unloadErrmsg;

    for (auto it : unloadPluginsTree) {
        auto kv = it.second;
        xrmPluginName = kv.get<std::string>("xrmPluginName", "");
        if (xrmPluginName.c_str()[0] == '\0') {
            errmsg += "request parameters xrm plugin name is not provided";
            ret = XRM_ERROR_INVALID;
            break;
        }

        ret = unloadOneXrmPlugin(xrmPluginName, unloadErrmsg);
        if (ret != XRM_SUCCESS) {
            errmsg += unloadErrmsg;
            break;
        }
    }
    return (ret);
}

void* xrm::system::listXrmPlugin(int32_t pluginId) {
    return ((void*)&m_xrmPluginList[pluginId]);
}

int32_t xrm::system::getXrmPluginNumber() {
    return (m_numXrmPlugin);
}

/*
 * Execute the xrm plugin function
 *
 * Return:
 *   XRM_SUCCESS: successed to execute the xrm plugin function
 *   XRM_ERROR: failed to execute the xrm plugin function
 *
 */
int32_t xrm::system::execXrmPluginFunc(const char* xrmPluginName,
                                       uint32_t funcId,
                                       xrmPluginFuncParam* param,
                                       std::string& errmsg) {
    int32_t ret = XRM_ERROR_INVALID;
    uint32_t pluginId = 0;

    if (xrmPluginName[0] == '\0') {
        logMsg(XRM_LOG_ERROR, "%s: XRM Plugin name is not set\n", __func__);
        errmsg += "XRM Plugin name is not set";
        return (ret);
    }
    if (funcId > XRM_MAX_PLUGIN_FUNC_ID) {
        logMsg(XRM_LOG_ERROR, "%s: Plugin function id is out of range: %d\n", __func__, funcId);
        errmsg += "Plugin function id out of range";
        return (ret);
    }

    bool foundPlugin = false;
    for (pluginId = 0; pluginId < m_numXrmPlugin; pluginId++) {
        if (m_xrmPluginList[pluginId].xrmPluginName.compare(xrmPluginName) == 0) {
            foundPlugin = true;
            break;
        }
    }
    if (!foundPlugin) {
        logMsg(XRM_LOG_ERROR, "%s: Plugin %s is not loaded\n", __func__, xrmPluginName);
        errmsg += "Plugin is not loaded";
        return (ret);
    }
    if (m_xrmPluginList[pluginId].pluginData.pluginFunc[funcId] == NULL) {
        logMsg(XRM_LOG_ERROR, "%s: Plugin Function %d is not implemented\n", __func__, funcId);
        errmsg += "Function is not implemented";
        return (ret);
    }
    ret = m_xrmPluginList[pluginId].pluginData.pluginFunc[funcId](param);

    return (ret);
}

/*
 * Function wrapper
 * older version:
 *     NULL
 * 2019.2 and 2019.2_PU1
 *     int xclCuName2Index(xclDeviceHandle handle, const char *cu_name, uint32_t *cu_index)
 * 2019.2_PU2:
 *     int xclIPName2Index(xclDeviceHandle handle, const char *ipName, uint32_t *ipIndex);
 * 2020.1:
 *     int xclIPName2Index(xclDeviceHandle handle, const char *ipName);
 */
int32_t xrm::system::wrapIPName2Index(xclDeviceHandle handle, const char* ipName) {
    if (m_libVersionDepFuncs.libVersionDepFuncXclCuName2Index != NULL) {
        // xrt 2019.2 and 2019.2_PU1 API
        uint32_t cuId;
        int32_t ret;
        ret = m_libVersionDepFuncs.libVersionDepFuncXclCuName2Index(handle, ipName, &cuId);
        if (ret == 0)
            return ((int32_t)cuId);
        else
            return (ret);
    } else if (m_libVersionDepFuncs.libVersionDepFuncXclIPName2Index25 != NULL) {
        // xrt 2019.2_PU2 API
        uint32_t cuId;
        int32_t ret;
        ret = m_libVersionDepFuncs.libVersionDepFuncXclIPName2Index25(handle, ipName, &cuId);
        if (ret == 0)
            return ((int32_t)cuId);
        else
            return (ret);
    } else if (m_libVersionDepFuncs.libVersionDepFuncXclIPName2Index26 != NULL) {
        // xrt 2020.1 API
        return (m_libVersionDepFuncs.libVersionDepFuncXclIPName2Index26(handle, ipName));
    } else {
        // older version, no API
        return (XRM_ERROR_INVALID);
    }
}

/*
 * Function wrapper
 * 2020.1 and older version
 * int xclLockDevice(xclDeviceHandle handle);
 */
int32_t xrm::system::wrapLockDevice(xclDeviceHandle handle) {
    if (m_libVersionDepFuncs.libVersionDepFuncXclLockDevice != NULL) {
        // xrt 2020.1 and older version, API implemented
        int32_t ret;
        ret = m_libVersionDepFuncs.libVersionDepFuncXclLockDevice(handle);
        return (ret);
    } else {
        // newer version, not support this API
        return (0);
    }
}

/*
 * Function wrapper
 * 2020.1 and older version
 * int xclUnlockDevice(xclDeviceHandle handle);
 */
int32_t xrm::system::wrapUnlockDevice(xclDeviceHandle handle) {
    if (m_libVersionDepFuncs.libVersionDepFuncXclUnlockDevice != NULL) {
        // xrt 2020.1 and older version, API implemented
        int32_t ret;
        ret = m_libVersionDepFuncs.libVersionDepFuncXclUnlockDevice(handle);
        return (ret);
    } else {
        // newer version, not support this API
        return (0);
    }
}

/*
 * Alloc one cu (compute unit) based on the request property version 2.
 * e.g for dev most used first policy, it tries to find cu by the order
 * of most used dev load id to least used dev load id
 * XRM_SUCCESS: allocated resouce is recorded by cuRes
 * Otherwise: failed to allocate the cu resource
 *
 * Lock: should enter lock during the cu allocation
 */
int32_t xrm::system::resAllocCuByDevLoad(cuPropertyV2* cuPropV2, cuResource* cuRes, bool updateId) {
    deviceData* dev;
    cuData* cu;
    cuData* preCu = NULL;
    int32_t ret = 0;
    bool kernelNameEqual, kernelAliasEqual, cuNameEqual;
    uint64_t clientId = cuPropV2->clientId;
    int32_t devId, cuId;
    int32_t preDevId = -1, preCuId = -1;
    bool cuAcquired = false;
    /* First pass will look for kernels already in-use by proc */
    cuProperty tmpProp;
    cuProperty* cuProp = &tmpProp;
    // no need to check cuPropV2 or cuRes as it should be checked already
    cuPropertyCopyFromV2(cuProp, cuPropV2);
    std::vector<xrm::deviceLoadInfo> devLoadArray(m_numDevice);
    for (int i = 0; i < m_numDevice; i++) {
        devLoadArray[i] = m_devList[i].devLoadInfo;
    }
    std::sort(devLoadArray.begin(), devLoadArray.end());

    for (int i = m_numDevice - 1; i >= 0 && !cuAcquired; i--) {
        devId = devLoadArray[i].deviceId;
        if (cuPropV2->policyInfo == XRM_POLICY_INFO_CONSTRAINT_TYPE_DEV_LEAST_USED_FIRST)
            devId = devLoadArray[m_numDevice - 1 - i].deviceId;
        ret = allocClientFromDev(devId, cuProp);
        if (ret < 0) {
            continue;
        }
        dev = &m_devList[devId];

        /*
         * Now check whether matching cu is on allocated device
         * if not, free device, increment dev count, re-loop
         */
        for (cuId = 0; cuId < XRM_MAX_XILINX_KERNELS && cuId < dev->xclbinInfo.numCu && !cuAcquired; cuId++) {
            cu = &dev->xclbinInfo.cuList[cuId];

            kernelNameEqual = true;
            if (cuProp->kernelName[0] != '\0') {
                if (cu->kernelName.compare(cuProp->kernelName)) kernelNameEqual = false;
            }
            /* kernel alias is presented, compare it; otherwise no need to compare */
            kernelAliasEqual = true;
            if (cuProp->kernelAlias[0] != '\0') {
                if (cu->kernelAlias.compare(cuProp->kernelAlias)) kernelAliasEqual = false;
            }
            /* cu name is presented, compare it; otherwise no need to compare */
            cuNameEqual = true;
            if (cuProp->cuName[0] != '\0') {
                if (cu->cuName.compare(cuProp->cuName)) cuNameEqual = false;
            }
            if (!(kernelNameEqual && kernelAliasEqual && cuNameEqual)) continue;
            cuData* tmpCu = preCu;
            /* alloc channel and register client id */
            ret = allocChanClientFromCu(cu, cuProp, cuRes, cuPropV2->policyInfo, &preCu);
            if (ret != XRM_SUCCESS) {
                if (tmpCu != preCu) {
                    preDevId = devId;
                    preCuId = cuId;
                }
                continue;
            }

            preCu = NULL;
            cuRes->deviceId = devId;
            cuRes->cuId = cuId;
            strncpy(cuRes->xclbinFileName, dev->xclbinName.c_str(), XRM_MAX_NAME_LEN - 1);
            strncpy(cuRes->uuidStr, dev->xclbinInfo.uuidStr.c_str(), XRM_MAX_NAME_LEN - 1);
            cuAcquired = true;
        } // cu loop

        if (preCu) {
            dev = &m_devList[preDevId];
            cu = &dev->xclbinInfo.cuList[preCuId];
            ret = allocChanClientFromCu(cu, cuProp, cuRes);
            cuRes->deviceId = preDevId;
            cuRes->cuId = preCuId;
            strncpy(cuRes->xclbinFileName, dev->xclbinName.c_str(), XRM_MAX_NAME_LEN - 1);
            strncpy(cuRes->uuidStr, dev->xclbinInfo.uuidStr.c_str(), XRM_MAX_NAME_LEN - 1);
            cuAcquired = true;
        }
        if (cuAcquired) {
            if (updateId) updateAllocServiceId();
            return (XRM_SUCCESS);
        }

        if (!cuAcquired) {
            releaseClientOnDev(devId, clientId);
        }
    }
    return (XRM_ERROR_NO_KERNEL);
}

/*
 * Alloc one cu (compute unit) based on the request property version 2.
 * e.g for cu load most used first policy, it tries to find cu by the order
 * of most used cu load to least used cu load
 * XRM_SUCCESS: allocated resouce is recorded by cuRes
 * Otherwise: failed to allocate the cu resource
 *
 * Lock: should enter lock during the cu allocation
 */

int32_t xrm::system::resAllocCuByCuLoad(cuPropertyV2* cuPropV2, cuResource* cuRes, bool updateId) {
    deviceData* dev;
    cuData* cu;
    cuData* preCu = NULL;
    int32_t ret = 0;
    bool kernelNameEqual, kernelAliasEqual, cuNameEqual;
    uint64_t clientId = cuPropV2->clientId;
    int32_t devId, cuId;
    int32_t preDevId = -1, preCuId = -1;
    bool cuAcquired = false;
    /* First pass will look for kernels already in-use by proc */
    bool cuAffinityPass = true;
    cuProperty tmpProp;
    cuProperty* cuProp = &tmpProp;
    // no need to check cuPropV2 or cuRes as it should be checked already
    cuPropertyCopyFromV2(cuProp, cuPropV2);
cu_alloc_loop:
    for (devId = -1; !cuAcquired && (devId < m_numDevice);) {
        devId = allocDevForClient(&devId, cuProp);
        if (devId < 0) {
            break;
        }

        ret = allocClientFromDev(devId, cuProp);
        if (ret < 0) {
            continue;
        }
        dev = &m_devList[devId];
        /*
         * Now check whether matching cu is on allocated device
         * if not, free device, increment dev count, re-loop
         */
        for (cuId = 0; cuId < XRM_MAX_XILINX_KERNELS && cuId < dev->xclbinInfo.numCu && !cuAcquired; cuId++) {
            cu = &dev->xclbinInfo.cuList[cuId];

            /* first attempt to re-use existing kernels; else, use a new kernel */
            if ((cuAffinityPass && cu->numClient == 0) || (!cuAffinityPass && cu->numClient > 0)) continue;
            /*
             * compare, 0: equal
             *
             * kernel name is presented, compare it; otherwise no need to compare
             */
            kernelNameEqual = true;
            if (cuProp->kernelName[0] != '\0') {
                if (cu->kernelName.compare(cuProp->kernelName)) kernelNameEqual = false;
            }
            /* kernel alias is presented, compare it; otherwise no need to compare */
            kernelAliasEqual = true;
            if (cuProp->kernelAlias[0] != '\0') {
                if (cu->kernelAlias.compare(cuProp->kernelAlias)) kernelAliasEqual = false;
            }
            /* cu name is presented, compare it; otherwise no need to compare */
            cuNameEqual = true;
            if (cuProp->cuName[0] != '\0') {
                if (cu->cuName.compare(cuProp->cuName)) cuNameEqual = false;
            }
            if (!(kernelNameEqual && kernelAliasEqual && cuNameEqual)) continue;
            cuData* tmpCu = preCu;
            /* alloc channel and register client id */
            ret = allocChanClientFromCu(cu, cuProp, cuRes, cuPropV2->policyInfo, &preCu);
            if (ret != XRM_SUCCESS) {
                if (tmpCu != preCu) {
                    preDevId = devId;
                    preCuId = cuId;
                }
                continue;
            }

            preCu = NULL;
            cuRes->deviceId = devId;
            cuRes->cuId = cuId;
            strncpy(cuRes->xclbinFileName, dev->xclbinName.c_str(), XRM_MAX_NAME_LEN - 1);
            strncpy(cuRes->uuidStr, dev->xclbinInfo.uuidStr.c_str(), XRM_MAX_NAME_LEN - 1);
            cuAcquired = true;
        }

        if (!cuAcquired) {
            releaseClientOnDev(devId, clientId);
        }
    }

    if (!cuAcquired && cuAffinityPass) {
        cuAffinityPass = false; /* open up search to all kernels */
        goto cu_alloc_loop;
    }
    if (preCu) {
        dev = &m_devList[preDevId];
        cu = &dev->xclbinInfo.cuList[preCuId];
        ret = allocChanClientFromCu(cu, cuProp, cuRes);
        cuRes->deviceId = preDevId;
        cuRes->cuId = preCuId;
        strncpy(cuRes->xclbinFileName, dev->xclbinName.c_str(), XRM_MAX_NAME_LEN - 1);
        strncpy(cuRes->uuidStr, dev->xclbinInfo.uuidStr.c_str(), XRM_MAX_NAME_LEN - 1);
        cuAcquired = true;
    }
    if (cuAcquired) {
        if (updateId) updateAllocServiceId();
        return (XRM_SUCCESS);
    }

    return (XRM_ERROR_NO_KERNEL);
}

int32_t xrm::system::resAllocCuFromDevByCuLoad(int32_t deviceId,
                                               cuPropertyV2* cuPropV2,
                                               cuResource* cuRes,
                                               bool updateId) {
    deviceData* dev;
    cuData* cu;
    cuData* preCu = NULL;
    int32_t ret = 0;
    bool kernelNameEqual, kernelAliasEqual, cuNameEqual;
    uint64_t clientId = cuPropV2->clientId;
    int32_t cuId;
    int32_t preCuId = -1;
    bool cuAcquired = false;
    // * First pass will look for kernels already in-use by proc
    bool cuAffinityPass = true;
    cuProperty tmpProp;
    cuProperty* cuProp = &tmpProp;
    // no need to check cuPropV2 or cuRes as it should be checked already
    cuPropertyCopyFromV2(cuProp, cuPropV2);

    if ((deviceId < 0) || (deviceId > (m_numDevice - 1))) {
        logMsg(XRM_LOG_ERROR, "deviceId (%d) is out of range\n", deviceId);
        return (XRM_ERROR_INVALID);
    }
    if ((cuProp == NULL) || (cuRes == NULL)) {
        logMsg(XRM_LOG_ERROR, "cuProp or cuRes is NULL\n");
        return (XRM_ERROR_INVALID);
    }
    if ((cuProp->kernelName[0] == '\0') && (cuProp->kernelAlias[0] == '\0') && (cuProp->cuName[0] == '\0')) {
        logMsg(XRM_LOG_ERROR, "None of kernel name, kernel alias and cu name are presented\n");
        return (XRM_ERROR_INVALID);
    }

    dev = &m_devList[deviceId];
    if (!dev->isLoaded) {
        return (XRM_ERROR_NO_DEV);
    }
    if ((dev->isExcl) && (dev->clientProcs[0].clientId != clientId)) {
        //* the device is used by other client exclusively
        return (XRM_ERROR_NO_DEV);
    }
    ret = allocClientFromDev(deviceId, cuProp);
    if (ret < 0) {
        return (XRM_ERROR_NO_DEV);
    }

cu_alloc_from_dev_loop:
    //*
    // * Now check whether matching cu is on allocated device
    // * and try to allocate requested cu.
    //
    for (cuId = 0; cuId < XRM_MAX_XILINX_KERNELS && cuId < dev->xclbinInfo.numCu && !cuAcquired; cuId++) {
        cu = &dev->xclbinInfo.cuList[cuId];

        /* first attempt to re-use existing kernels; else, use a new kernel */
        if ((cuAffinityPass && cu->numClient == 0) || (!cuAffinityPass && cu->numClient > 0)) continue;
        /*
         * compare, 0: equal
         *
         * kernel name is presented, compare it; otherwise no need to compare
         */
        kernelNameEqual = true;
        if (cuProp->kernelName[0] != '\0') {
            if (cu->kernelName.compare(cuProp->kernelName)) kernelNameEqual = false;
        }
        /* kernel alias is presented, compare it; otherwise no need to compare */
        kernelAliasEqual = true;
        if (cuProp->kernelAlias[0] != '\0') {
            if (cu->kernelAlias.compare(cuProp->kernelAlias)) kernelAliasEqual = false;
        }
        /* cu name is presented, compare it; otherwise no need to compare */
        cuNameEqual = true;
        if (cuProp->cuName[0] != '\0') {
            if (cu->cuName.compare(cuProp->cuName)) cuNameEqual = false;
        }
        if (!(kernelNameEqual && kernelAliasEqual && cuNameEqual)) continue;
        cuData* tmpCu = preCu;
        /* alloc channel and register client id */
        ret = allocChanClientFromCu(cu, cuProp, cuRes, cuPropV2->policyInfo, &preCu);
        if (ret != XRM_SUCCESS) {
            if (tmpCu != preCu) {
                preCuId = cuId;
            }
            continue;
        }

        preCu = NULL;
        cuRes->deviceId = deviceId;
        cuRes->cuId = cuId;
        strncpy(cuRes->xclbinFileName, dev->xclbinName.c_str(), XRM_MAX_NAME_LEN - 1);
        strncpy(cuRes->uuidStr, dev->xclbinInfo.uuidStr.c_str(), XRM_MAX_NAME_LEN - 1);
        cuAcquired = true;
    }

    if (!cuAcquired && cuAffinityPass) {
        cuAffinityPass = false; /* open up search to all kernels */
        goto cu_alloc_from_dev_loop;
    }
    if (preCu) {
        cu = &dev->xclbinInfo.cuList[preCuId];
        ret = allocChanClientFromCu(cu, cuProp, cuRes);
        cuRes->deviceId = deviceId;
        cuRes->cuId = preCuId;
        strncpy(cuRes->xclbinFileName, dev->xclbinName.c_str(), XRM_MAX_NAME_LEN - 1);
        strncpy(cuRes->uuidStr, dev->xclbinInfo.uuidStr.c_str(), XRM_MAX_NAME_LEN - 1);
        cuAcquired = true;
    }

    if (cuAcquired) {
        if (updateId) updateAllocServiceId();
        return (XRM_SUCCESS);
    } else {
        releaseClientOnDev(deviceId, clientId);
        return (XRM_ERROR_NO_KERNEL);
    }
}
