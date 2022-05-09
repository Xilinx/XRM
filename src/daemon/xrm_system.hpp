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

#ifndef _XRM_SYSTEM_HPP_
#define _XRM_SYSTEM_HPP_

#include <vector>
#include <map>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <fstream>
#include <iostream>
#include <dlfcn.h>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#define BOOST_SPIRIT_THREADSAFE // must before json_parser.h and ptree.hpp
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <syslog.h>
#include <signal.h>
#include <xrt.h>

#include "xrm_limits.h"
#include "xrm_error.h"
#include "xrm.h"

namespace pt = boost::property_tree;
// XRM_FURTHER_CHECK is used when it can't decide if current cu is best candidate.
#define XRM_FURTHER_CHECK (-1)

namespace xrm {

/* cu status */
typedef struct cuStatus {
    bool isBusy;
    int32_t usedLoadUnified;  // used load, granularity of 1,000,000
    int32_t usedLoadOriginal; /* used load of this cu, only one type granularity at one time.
                               * bit[31 - 28] reserved
                               * bit[27 -  8] granularity of 1000000 (0 - 1000000)
                               * bit[ 7 -  0] granularity of 100 (0 - 100)
                               */
    uint8_t extData[64];      // for future extension
} cuStatus;

/* request compute unit resource property */
typedef struct cuProperty {
    char kernelName[XRM_MAX_NAME_LEN];  // unique kernel name, not instance name
    char kernelAlias[XRM_MAX_NAME_LEN]; // unique alias of kernel name
    char cuName[XRM_MAX_NAME_LEN];      // unique cu name (kernelName:instanceName)
    bool devExcl;                       // request exclusive device usage for this client
    int32_t requestLoadUnified;         // request load, granularity of 1,000,000
    int32_t requestLoadOriginal;        /* request load, only one type granularity at one time.
                                         * bit[31 - 28] reserved
                                         * bit[27 -  8] granularity of 1000000 (0 - 1000000)
                                         * bit[ 7 -  0] granularity of 100 (0 - 100)
                                         */
    uint64_t clientId;
    pid_t clientProcessId;
    uint64_t poolId;     /* request to allocate resource from specified resource pool,
                          * the system default resource pool id is 0.
                          */
    uint8_t extData[64]; // for future extension

    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        std::ignore = version;
        ar& kernelName& kernelAlias& cuName& devExcl& requestLoadUnified& requestLoadOriginal& clientId&
            clientProcessId& poolId;
    }
} cuProperty;

/* list of request compute unit resource property */
typedef struct cuListProperty {
    cuProperty cuProps[XRM_MAX_LIST_CU_NUM];
    int32_t cuNum;
    bool sameDevice;
    uint8_t extData[64]; // for future extension

    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        std::ignore = version;
        ar& cuProps& cuNum& sameDevice;
    }
} cuListProperty;

/* user defined compute unit resource group information */
typedef struct udfCuGroupInformation {
    std::string udfCuGroupName; // name of user defined cu group
    cuListProperty optionUdfCuListProps[XRM_MAX_UDF_CU_GROUP_OPTION_LIST_NUM];
    int32_t optionUdfCuListNum;
    uint8_t extData[64]; // for future extension

    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        std::ignore = version;
        ar& udfCuGroupName& optionUdfCuListProps& optionUdfCuListNum;
    }
} udfCuGroupInformation;

/* compute resource group property */
typedef struct cuGroupProperty {
    std::string udfCuGroupName; // name of user defined cu group
    uint64_t clientId;
    pid_t clientProcessId;
    uint64_t poolId;     /* request to allocate resource from specified resource pool,
                          * 0 means to allocate resource from system default resource pool.
                          */
    uint8_t extData[64]; // for future extension
} cuGroupProperty;

/* pool of request compute unit resource property */
typedef struct cuPoolProperty {
    cuListProperty cuListProp;
    int32_t cuListNum; // number of such cu list
    uuid_t xclbinUuid;
    int32_t xclbinNum;     // number of such xclbin
    uint64_t clientId;     // client ID
    pid_t clientProcessId; // client process ID
    uint8_t extData[64];   // for future extension
} cuPoolProperty;

/* cu type */
typedef enum cuTypes {
    CU_NULL = 0,
    CU_IPKERNEL = 1,   // IP kernel
    CU_SOFTKERNEL = 2, // soft kernel
} cuTypes;

/* allocated/released compute unit resource */
typedef struct cuResource {
    char xclbinFileName[XRM_MAX_PATH_NAME_LEN];       // include path and name
    char kernelPluginFileName[XRM_MAX_PATH_NAME_LEN]; // just the name
    char kernelName[XRM_MAX_NAME_LEN];                // kernel name, not instance name
    char kernelAlias[XRM_MAX_NAME_LEN];               // unique alias of kernel name
    char instanceName[XRM_MAX_NAME_LEN];              // instance name
    char cuName[XRM_MAX_NAME_LEN];                    // cu name
    char uuidStr[XRM_MAX_NAME_LEN];
    int32_t deviceId;
    int32_t cuId;
    int32_t channelId;
    cuTypes cuType;
    uint64_t baseAddr;        // base address of the CU
    uint32_t membankId;       // connected memory bank id
    uint32_t membankType;     // connected memory bank type
    uint64_t membankSize;     // connected memory bank size
    uint64_t membankBaseAddr; // connected memory bank base address
    uint64_t allocServiceId;  // unique service id of the allocation
    uint64_t clientId;
    int32_t channelLoadUnified;  // channel load, granularity of 1,000,000
    int32_t channelLoadOriginal; /* channel load, only one type granularity at one time.
                                  * bit[31 - 28] reserved
                                  * bit[27 -  8] granularity of 1000000 (0 - 1000000)
                                  * bit[ 7 -  0] granularity of 100 (0 - 100)
                                  */
    uint64_t poolId;             // id of the cu pool this cu comes from, default pool id is 0
    uint8_t extData[64];         // for future extension
} cuResource;

/*
 * allocated/released compute unit resource list
 */
typedef struct cuListResource {
    cuResource cuResources[XRM_MAX_LIST_CU_NUM];
    int32_t cuNum;
    uint8_t extData[64]; // for future extension
} cuListResource;

/*
 * allocated/released compute unit resource group
 */
typedef struct cuGroupResource {
    cuResource cuResources[XRM_MAX_GROUP_CU_NUM];
    int32_t cuNum;
    uint8_t extData[64]; // for future extension
} cuGroupResource;

/*
 * allocated/released compute unit resource pool
 */
typedef struct cuPoolResource {
    cuResource cuResources[XRM_MAX_POOL_CU_NUM];
    int32_t cuNum;
    uint8_t extData[64]; // for future extension
} cuPoolResource;

/* Alloction information for querying */
typedef struct allocationQueryInfo {
    uint64_t allocServiceId;            // the service id returned from allocation
    char kernelName[XRM_MAX_NAME_LEN];  // kernel name, not instance name
    char kernelAlias[XRM_MAX_NAME_LEN]; // unique alias of kernel name
    uint8_t extData[64];                // for future extension
} allocationQueryInfo;

/* request compute unit resource property version 2 */
typedef struct cuPropertyV2 {
    char kernelName[XRM_MAX_NAME_LEN];  // unique kernel name, not instance name
    char kernelAlias[XRM_MAX_NAME_LEN]; // unique alias of kernel name
    char cuName[XRM_MAX_NAME_LEN];      // unique cu name (kernelName:instanceName)
    bool devExcl;                       // request exclusive device usage for this client
    uint64_t deviceInfo;                /* resource allocation device constaint information
                                         * bit[63 - 40] reserved
                                         * bit[39 - 32] constraintType
                                         *              0 : no specified device constraint
                                         *              1 : hardware device index. It's used to identify hardware
                                         *                  device index is used as the constraint.
                                         *              2 : virtual device index. it's used to descript multiple cu
                                         *                  from same device if device index is same. It does not
                                         *                  means from specified hardware device. This type is only
                                         *                  valid in property of cu list. It's not valid for single cu.
                                         *              others : reserved
                                         * bit[31 -  0] deviceIndex
                                         */
    uint64_t memoryInfo;                /* resource allocation memory constaint information
                                         * bit[63 - 40] reserved
                                         * bit[39 - 32] constraintType
                                         *              0 : no specified memory constraint
                                         *              1 : hardware memory bank. It's used to identify hardware
                                         *                  memory bank is used as the constraint.
                                         *              others : reserved
                                         * bit[31 -  0] memoryBankIndex
                                         */
    uint64_t policyInfo;                /* resource allocation policy information
                                         * bit[63 -  8] reserved
                                         * bit[ 7 -  0] policyType
                                         *              0 : no specified policy
                                         *              1 : most used first
                                         *              2 : least used first
                                         *              others : reserved
                                         */
    int32_t requestLoadUnified;         // request load, granularity of 1,000,000
    int32_t requestLoadOriginal;        /* request load, only one type granularity at one time.
                                         * bit[31 - 28] reserved
                                         * bit[27 -  8] granularity of 1000000 (0 - 1000000)
                                         * bit[ 7 -  0] granularity of 100 (0 - 100)
                                         */
    uint64_t clientId;
    pid_t clientProcessId;
    uint64_t poolId;     /* request to allocate resource from specified resource pool,
                          * the system default resource pool id is 0.
                          */
    uint8_t extData[64]; // for future extension

    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        std::ignore = version;
        ar& kernelName& kernelAlias& cuName& deviceInfo& memoryInfo& policyInfo& devExcl& requestLoadUnified&
            requestLoadOriginal& clientId& clientProcessId& poolId;
    }
} cuPropertyV2;

/* item flags  */
typedef struct itemFlag {
    bool handled;   // the item is handled
    bool allocated; // the item is allocated
} itemFlag;

/* list of request compute unit resource property version 2 */
typedef struct cuListPropertyV2 {
    cuPropertyV2 cuProps[XRM_MAX_LIST_CU_NUM_V2];
    int32_t cuNum;
    uint8_t extData[64]; // for future extension

    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        std::ignore = version;
        ar& cuProps& cuNum;
    }
} cuListPropertyV2;

/* sub list of request compute unit resource property version 2 */
typedef struct cuSubListPropertyV2 {
    cuProperty cuProps[XRM_MAX_LIST_CU_NUM_V2];
    int32_t indexInOriList[XRM_MAX_LIST_CU_NUM_V2];
    int32_t cuNum;
    uint8_t extData[64]; // for future extension

    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        std::ignore = version;
        ar& cuProps& indexInOriList& cuNum;
    }
} cuSubListPropertyV2;

/* user defined compute unit resource group information version 2 */
typedef struct udfCuGroupInformationV2 {
    std::string udfCuGroupName; // name of user defined cu group
    cuListPropertyV2 optionUdfCuListProps[XRM_MAX_UDF_CU_GROUP_OPTION_LIST_NUM_V2];
    int32_t optionUdfCuListNum;
    uint8_t extData[64]; // for future extension

    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        std::ignore = version;
        ar& udfCuGroupName& optionUdfCuListProps& optionUdfCuListNum;
    }
} udfCuGroupInformationV2;

/* compute resource group property version 2 */
typedef struct cuGroupPropertyV2 {
    std::string udfCuGroupName; // name of user defined cu group
    uint64_t clientId;
    pid_t clientProcessId;
    uint64_t poolId;     /* request to allocate resource from specified resource pool,
                          * 0 means to allocate resource from system default resource pool.
                          */
    uint8_t extData[64]; // for future extension
} cuGroupPropertyV2;

/* device list */
typedef struct deviceIdListPropertyV2 {
    uint64_t deviceIds[XRM_MAX_XILINX_DEVICES]; // device id in list is unique
    int32_t deviceNum;
    uint8_t extData[64]; // for future extension
} deviceIdListPropertyV2;

/* pool of request compute unit resource property version 2 */
typedef struct cuPoolPropertyV2 {
    cuListPropertyV2 cuListProp;
    int32_t cuListNum; // number of such cu list
    uuid_t xclbinUuid;
    int32_t xclbinNum; // number of such xclbin
    deviceIdListPropertyV2 deviceIdListProp;
    uint64_t clientId;     // client ID
    pid_t clientProcessId; // client process ID
    uint8_t extData[64];   // for future extension
} cuPoolPropertyV2;

/*
 * allocated/released compute unit resource list version 2
 */
typedef struct cuListResourceV2 {
    cuResource cuResources[XRM_MAX_LIST_CU_NUM_V2];
    int32_t cuNum;
    uint8_t extData[64]; // for future extension
} cuListResourceV2;

/*
 * allocated/released sub compute unit resource list version 2
 */
typedef struct cuSubListResourceV2 {
    cuResource cuResources[XRM_MAX_LIST_CU_NUM_V2];
    int32_t cuNum;
    uint8_t extData[64]; // for future extension
} cuSubListResourceV2;

/*
 * allocated/released compute unit resource group version 2
 */
typedef struct cuGroupResourceV2 {
    cuResource cuResources[XRM_MAX_GROUP_CU_NUM_V2];
    int32_t cuNum;
    uint8_t extData[64]; // for future extension
} cuGroupResourceV2;

/*
 * allocated/released compute unit resource pool version 2
 */
typedef struct cuPoolResourceV2 {
    cuResource cuResources[XRM_MAX_POOL_CU_NUM_V2];
    int32_t cuNum;
    uint8_t extData[64]; // for future extension
} cuPoolResourceV2;

/* cu resource information version 2 */
typedef struct cuResInforV2 {
    uint64_t deviceId;
    uint8_t extData[64]; // for future extension
} cuResInforV2;

/* cu list resource information version 2 */
typedef struct cuListResInforV2 {
    cuResInforV2 cuResInfor[XRM_MAX_LIST_CU_NUM_V2];
    int32_t cuNum;
    uint8_t extData[64]; // for future extension
} cuListResInforV2;

/*
 * cu pool resource information version 2
 */
typedef struct cuPoolResInforV2 {
    cuListResInforV2 cuListResInfor[XRM_MAX_POOL_CU_LIST_NUM_V2];
    int32_t cuListNum; // number of such cu list
    cuResInforV2 xclbinResInfor[XRM_MAX_XILINX_DEVICES];
    int32_t xclbinNum;   // number of xclbin
    uint8_t extData[64]; // for future extension
} cuPoolResInforV2;

/* Alloction information for querying version 2 */
typedef struct allocationQueryInfoV2 {
    uint64_t allocServiceId;            // the service id returned from allocation
    char kernelName[XRM_MAX_NAME_LEN];  // kernel name, not instance name
    char kernelAlias[XRM_MAX_NAME_LEN]; // unique alias of kernel name
    uint8_t extData[64];                // for future extension
} allocationQueryInfoV2;

/* Reservation information for querying version 2 */
typedef struct reservationQueryInfoV2 {
    uint64_t poolId;                    // the pool id returned from reservation
    char kernelName[XRM_MAX_NAME_LEN];  // kernel name, not instance name
    char kernelAlias[XRM_MAX_NAME_LEN]; // unique alias of kernel name
    uint8_t extData[64];                // for future extension
} reservationQueryInfoV2;

/* channel data */
typedef struct channelData {
    int32_t channelId;
    uint64_t allocServiceId;
    uint64_t poolId;
    uint64_t clientId;
    pid_t clientProcessId;
    int32_t channelLoadUnified;  // channel load, granularity of 1,000,000
    int32_t channelLoadOriginal; /* channel load, only one type granularity at one time.
                                  * bit[31 - 28] reserved
                                  * bit[27 -  8] granularity of 1000000 (0 - 1000000)
                                  * bit[ 7 -  0] granularity of 100 (0 - 100)
                                  */

    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        std::ignore = version;
        ar& channelId& allocServiceId& poolId& clientId& clientProcessId& channelLoadUnified& channelLoadOriginal;
    }
} channelData;

/* reserve related data */
typedef struct reserveData {
    uint64_t reservePoolId;
    int32_t reserveLoadUnified;     // load, granularity of 1,000,000
    int32_t reserveUsedLoadUnified; // load, granularity of 1,000,000
    bool clientIsActive;
    uint64_t clientId;
    pid_t clientProcessId;

    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        std::ignore = version;
        ar& reservePoolId& reserveLoadUnified& reserveUsedLoadUnified& clientIsActive& clientId& clientProcessId;
    }
} reserveData;

struct deviceData;
typedef struct deviceLoadInfo {
    int32_t devLoadRate; // devCurrentLoad * 1000/ totalLoad
    int32_t deviceId;
    int64_t devCurrentLoad; // current device load = sum of all cu load
    deviceData* devData;
    void init(int32_t devId) {
        deviceId = devId;
        devLoadRate = 0;
        devCurrentLoad = 0;
    }
    deviceLoadInfo() {}
    bool operator<(const deviceLoadInfo& b) { return devLoadRate < b.devLoadRate || deviceId < b.deviceId; }
} deviceLoadInfo;

/* compute unit data */
typedef struct cuData {
    int32_t cuId;          // index on one device, start from 0
    int32_t ipLayoutIndex; // static index of m_ip_data in xclbin file
    cuTypes cuType;
    std::string kernelName;   // kernel name, not instance name
    std::string kernelAlias;  // alias of kernel name
    std::string cuName;       // kernel_name:instance_name (for IP kernel)
    std::string instanceName; // instance name
    std::string kernelPluginFileName;
    uint64_t maxCapacity;
    uint64_t baseAddr;        // the base address of the CU
    uint32_t membankId;       // connected memory bank id
    uint32_t membankType;     // connected memory bank type
    uint64_t membankSize;     // connected memory bank size
    uint64_t membankBaseAddr; // connected memory bank base address
    channelData channels[XRM_MAX_KERNEL_CHANNELS];
    int32_t numChanInuse;
    uint64_t clients[XRM_MAX_KERNEL_CHANNELS]; // client id attached to cu
    int32_t numClient;                         // current number of processes attached to cu
    reserveData reserves[XRM_MAX_KERNEL_RESERVES];
    int32_t numReserve;                   // number of reserves on this cu
    int32_t totalUsedLoadUnified;         // granularity of 1,000,000, allocated load in default pool + reserved load
    int32_t totalReservedLoadUnified;     // granularity of 1,000,000, reserved load
    int32_t totalReservedUsedLoadUnified; // granularity of 1,000,000, used load in reserved load

    int32_t deviceId;

    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        std::ignore = version;
        ar& cuId& cuType& kernelName& kernelAlias& cuName& instanceName& kernelPluginFileName& maxCapacity& baseAddr&
            baseAddr& channels& numChanInuse& clients& numClient& reserves& numReserve& totalUsedLoadUnified&
                totalReservedLoadUnified& totalReservedUsedLoadUnified;
    }
} cuData;

/* memory topology information */
typedef struct memTopologyData {
    uint8_t memType;
    uint8_t memUsed;
    uint64_t memSize;
    uint64_t memBaseAddress;
    unsigned char memTag[16];

    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        std::ignore = version;
        ar& memType& memUsed& memSize& memBaseAddress& memTag;
    }
} memTopologyData;

/* connectivity information */
typedef struct connectData {
    int32_t argIndex;
    int32_t ipLayoutIndex;
    int32_t memDataIndex;

    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        std::ignore = version;
        ar& argIndex& ipLayoutIndex& memDataIndex;
    }
} connectData;

/* xclbin information */
typedef struct xclbinInformation {
    uuid_t uuid;
    std::string uuidStr;
    int32_t numCu;
    int32_t numHardwareCu;
    int32_t numSoftwareCu;
    cuData cuList[XRM_MAX_XILINX_KERNELS];
    int32_t numMemBank;
    int32_t numConnect;
    memTopologyData memTopologyList[XRM_MAX_DDR_MAP];
    connectData connectList[XRM_MAX_CONNECTION_ENTRIES];

    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        std::ignore = version;
        ar& uuid& uuidStr& numCu& numHardwareCu& numSoftwareCu& cuList& numMemBank& numConnect& memTopologyList&
            connectList;
    }
} xclbinInformation;

typedef struct clientData {
    uint64_t clientId;
    pid_t clientProcessId;
    int32_t ref;

    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        std::ignore = version;
        ar& clientId& clientProcessId& ref;
    }
} clientData;

typedef struct deviceData {
    std::string dsaName;
    std::string xclbinName;
    int32_t devId;
    bool isDisabled; // device is disabled or not
    bool isLoaded;   // xclbin loaded or not
    bool isExcl;     // exclusive access to the device
    char* memBuffer;
    xclDeviceHandle deviceHandle;
    xclDeviceInfo2 deviceInfo;
    xclbinInformation xclbinInfo;
    clientData clientProcs[XRM_MAX_DEV_CLIENTS]; // processes using device (allocation but NOT reservation)

    deviceLoadInfo devLoadInfo;
    void updateDeviceLoad(int64_t loadChangeVal, int64_t setCurLoadVal) {
        if (setCurLoadVal >= 0) {
            devLoadInfo.devCurrentLoad = setCurLoadVal;
        } else {
            devLoadInfo.devCurrentLoad += loadChangeVal;
        }
        devLoadInfo.devLoadRate =
            devLoadInfo.devCurrentLoad * 1000 / (xclbinInfo.numCu * XRM_MAX_CHAN_LOAD_GRANULARITY_1000000);
        return;
    }

    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        std::ignore = version;
        ar& dsaName& xclbinName& devId& isDisabled& isLoaded& isExcl& xclbinInfo& clientProcs;
    }
} deviceData;

typedef struct pluginInformation {
    std::string xrmPluginName;
    std::string xrmPluginFileName;
    xrmPluginData pluginData;
} pluginInformation;

/* version depended functions from Lib */

/**
 * In xrt 2019.2 (xrt 2.3.xxx)
 * XRT_VERSION_RELEASE 201920
 * XRT_VERSION_MAJOR 2
 * XRT_VERSION_MINOR 3
 *
 * and
 *
 * In xrt 2019.2_pu1 (xrt 2.4.xxx)
 * XRT_VERSION_RELEASE 201920_PU1
 * XRT_VERSION_MAJOR 2
 * XRT_VERSION_MINOR 4
 *
 * int xclCuName2Index(xclDeviceHandle handle, const char *cu_name, uint32_t *cu_index);
 */
typedef int32_t (*libVersionDepFuncXclCuName2IndexType)(xclDeviceHandle handle,
                                                        const char* cu_name,
                                                        uint32_t* cu_index);

/**
 * In xrt 2019.2_pu2 (xrt 2.5.xxx)
 * XRT_VERSION_RELEASE 201920_PU2
 * XRT_VERSION_MAJOR 2
 * XRT_VERSION_MINOR 5
 *
 * int xclIPName2Index(xclDeviceHandle handle, const char *ipName, uint32_t *ipIndex);
 */
typedef int32_t (*libVersionDepFuncXclIPName2IndexType25)(xclDeviceHandle handle,
                                                          const char* ipName,
                                                          uint32_t* ipIndex);

/**
 * In xrt 2020.1 and newer version (xrt >= 2.6.xxx)
 * XRT_VERSION_RELEASE 202010
 * XRT_VERSION_MAJOR 2
 * XRT_VERSION_MINOR 6
 *
 * int xclIPName2Index(xclDeviceHandle handle, const char *ipName);
 */
typedef int32_t (*libVersionDepFuncXclIPName2IndexType26)(xclDeviceHandle handle, const char* ipName);

/**
 * In xrt 2020.1 and older version (xrt <= 2.6.xxx)
 * XRT_VERSION_RELEASE 202010
 * XRT_VERSION_MAJOR 2
 * XRT_VERSION_MINOR 6
 *
 * int xclLockDevice(xclDeviceHandle handle);
 */
typedef int32_t (*libVersionDepFuncXclLockDeviceType)(xclDeviceHandle handle);

/**
 * In xrt 2020.1 and older version (xrt <= 2.6.xxx)
 * XRT_VERSION_RELEASE 202010
 * XRT_VERSION_MAJOR 2
 * XRT_VERSION_MINOR 6
 *
 * int xclUnlockDevice(xclDeviceHandle handle);
 */
typedef int32_t (*libVersionDepFuncXclUnlockDeviceType)(xclDeviceHandle handle);

typedef struct libVersionDepFunctionsData {
    libVersionDepFuncXclCuName2IndexType libVersionDepFuncXclCuName2Index;
    libVersionDepFuncXclIPName2IndexType25 libVersionDepFuncXclIPName2Index25;
    libVersionDepFuncXclIPName2IndexType26 libVersionDepFuncXclIPName2Index26;
    libVersionDepFuncXclLockDeviceType libVersionDepFuncXclLockDevice;
    libVersionDepFuncXclUnlockDeviceType libVersionDepFuncXclUnlockDevice;
} libVersionDepFunctionsData;

struct Version {
    int major = 0, minor = 0, revision = 0;
    Version(std::string version) { std::sscanf(version.c_str(), "%d.%d.%d", &major, &minor, &revision); }

    bool operator<(const Version& other) {
        if (major < other.major) return true;
        if (minor < other.minor) return true;
        if (revision < other.revision) return true;
        return false;
    }

    bool operator==(const Version& other) {
        return major == other.major && minor == other.minor && revision == other.revision;
    }

    friend std::ostream& operator<<(std::ostream& stream, const Version& ver) {
        stream << ver.major;
        stream << '.';
        stream << ver.minor;
        stream << '.';
        stream << ver.revision;
        return stream;
    }
};

struct DevLoad {
    uint32_t loadRate;
    int32_t devId;
    uint64_t curDevLoad;
};

class system {
   public:
    system() {}

    void initSystem();
    void initConfig();
    void initLibVersionDepFunctions();
    void initDevices();
    void initXrmPlugins();
    void initUdfCuGroups();
    int32_t closeDevice(int32_t devId);
    int32_t resetDevice(int32_t devId);
    int32_t isDeviceOffline(int32_t devId);
    int32_t enableOneDevice(const int32_t& devId, std::string& errmsg);
    int32_t disableOneDevice(const int32_t& devId, std::string& errmsg);
    int32_t loadDevices(pt::ptree& loadTree, std::string& errmsg);
    int32_t loadOneDevice(pt::ptree& loadTree, std::string& errmsg);
    int32_t unloadOneDevice(const int32_t& devId, std::string& errmsg);
    void* listDevice(int32_t devId);
    int32_t getDeviceNumber();
    int32_t getLogLevel();
    uint64_t getNewClientId();
    uint32_t getNumConcurrentClient();
    bool incNumConcurrentClient();
    bool decNumConcurrentClient();
    void* listXrmPlugin(int32_t pluginId);
    int32_t getXrmPluginNumber();

    void binToHexstr(unsigned char* in, int32_t insz, std::string& outStr);
    void hexstrToBin(std::string& inStr, int32_t insz, unsigned char* out);

    void initLock();
    void enterLock();
    void exitLock();
    void logMsg(xrmLogLevelType logLevel, const char* format, ...);

    void save();
    bool restore();

    void recycleResource(uint64_t clientId);

    uint64_t getCuMaxCapacity(cuProperty* cuProp);
    int32_t checkCuStat(cuResource* crRes, cuStatus* cuStat);

    /* functions to check whether cu/cu list / cu group is existing in current system */
    bool resIsCuExistingOnDev(int32_t devId, cuProperty* cuProp);
    bool resIsCuExisting(cuProperty* cuProp);
    bool resIsCuListExisting(cuListProperty* cuListProp);
    bool resIsCuGroupExisting(cuGroupProperty* cuGroupProp);

    /* following alloc and free function need to do lock protect */
    int32_t resAllocCuV2(cuPropertyV2* cuPropV2, cuResource* cuRes, bool updateId);
    int32_t resAllocCuByDevLoad(cuPropertyV2* cuPropV2, cuResource* cuRes, bool updateId);
    int32_t resAllocCuByCuLoad(cuPropertyV2* cuPropV2, cuResource* cuRes, bool updateId);
    int32_t resAllocCuSubListFromSameDevice(cuSubListPropertyV2* cuSubListPropV2,
                                            deviceIdListPropertyV2* usedDevIdList,
                                            cuSubListResourceV2* cuSubListResV2);
    int32_t resReleaseCuSubList(cuSubListResourceV2* cuSubListResV2);
    int32_t resAllocCuListV2(cuListPropertyV2* cuListPropV2, cuListResourceV2* cuListResV2);
    int32_t resAllocationQueryV2(allocationQueryInfoV2* allocQueryV2, cuListResourceV2* cuListResV2);
    int32_t resReleaseCuV2(cuResource* cuRes);
    int32_t resReleaseCuListV2(cuListResourceV2* cuListResV2);
    int32_t resUdfCuGroupDeclareV2(udfCuGroupInformationV2* udfCuGroupInfoV2);
    int32_t resUdfCuGroupUndeclareV2(udfCuGroupInformationV2* udfCuGroupInfoV2);
    int32_t resAllocCuGroupV2(cuGroupPropertyV2* cuGroupPropV2, cuGroupResourceV2* cuGroupResV2);
    int32_t resReleaseCuGroupV2(cuGroupResourceV2* cuGroupResV2);
    void flushUdfCuGroupInfoV2(uint32_t udfCuGroupIdx);
    int32_t resAllocCu(cuProperty* cuProp, cuResource* cuRes, bool updateId);
    int32_t resAllocCuFromDev(int32_t deviceId, cuProperty* cuProp, cuResource* cuRes, bool updateId);
    int32_t resAllocCuFromDevByCuLoad(int32_t deviceId, cuPropertyV2* cuProp, cuResource* cuRes, bool updateId);
    int32_t resAllocLeastUsedCuFromDev(int32_t deviceId, cuProperty* cuProp, cuResource* cuRes, bool updateId);
    int32_t resAllocCuList(cuListProperty* cuListProp, cuListResource* cuListRes);
    int32_t resAllocationQuery(allocationQueryInfo* allocQuery, cuListResource* cuListRes);
    int32_t resReleaseCu(cuResource* cuRes);
    int32_t resReleaseCuList(cuListResource* cuListRes);
    int32_t resUdfCuGroupDeclare(udfCuGroupInformation* udfCuGroupInfo);
    int32_t resUdfCuGroupUndeclare(udfCuGroupInformation* udfCuGroupInfo);
    int32_t resAllocCuGroup(cuGroupProperty* cuGroupProp, cuGroupResource* cuGroupRes);
    int32_t resReleaseCuGroup(cuGroupResource* cuGroupRes);
    void flushUdfCuGroupInfo(uint32_t udfCuGroupIdx);

    void cuPropertyCopyFromV2(cuProperty* desCuProp, cuPropertyV2* srcCuPropV2);
    void cuResourceCopy(cuResource* desCuRes, cuResource* srcCuRes);
    bool isInDeviceIdList(deviceIdListPropertyV2* devIdList, int32_t devId);
    uint32_t insertDeviceIdList(deviceIdListPropertyV2* devIdList, int32_t devId);

    int32_t resReserveCuSubListFromSameDevice(uint64_t reservePoolId,
                                              cuSubListPropertyV2* cuSubListProp,
                                              deviceIdListPropertyV2* usedDevIdList,
                                              uint64_t* fromDevId);
    int32_t resReserveCuListV2(uint64_t reservePoolId,
                               cuListPropertyV2* cuListPropV2,
                               cuListResInforV2* cuListResInforV2);
    int32_t resReserveDevice(uint64_t reservePoolId, uint64_t deviceId, uint64_t clientId, pid_t clientProcessId);
    int32_t resRelinquishCuListV2(uint64_t reservePoolId);
    uint64_t resReserveCuPoolV2(cuPoolPropertyV2* cuPoolProp, cuPoolResInforV2* cuPoolResInfor);
    int32_t resRelinquishCuPoolV2(uint64_t reservePoolId);
    int32_t resReservationQueryV2(reservationQueryInfoV2* reserveQueryInfo, cuPoolResourceV2* cuPoolRes);
    int32_t resReserveCu(uint64_t reservePoolId, cuProperty* cuProp, uint64_t* fromDevId);
    int32_t resReserveCuList(uint64_t reservePoolId, cuListProperty* cuListProp);
    int32_t resReserveCuOfXclbin(
        uint64_t reservePoolId, uuid_t uuid, uint64_t clientId, pid_t clientProcessId, uint64_t* fromDevId);
    int32_t resRelinquishCuList(uint64_t reservePoolId);
    uint64_t resReserveCuPool(cuPoolProperty* cuPoolProp);
    int32_t resRelinquishCuPool(uint64_t reservePoolId);
    int32_t resReservationQuery(uint64_t reservePoolId, cuPoolResource* cuPoolRes);

    int32_t resAllocCuWithLoad(cuProperty* cuProp, std::string xclbin, cuResource* cuRes, bool updateId);
    int32_t resAllocCuLeastUsedWithLoad(cuProperty* cuProp, std::string xclbin, cuResource* cuRes, bool updateId);
    int32_t resLoadAndAllocAllCu(std::string xclbin,
                                 uint64_t clientId,
                                 pid_t clientProcessId,
                                 cuListResource* cuListRes);

    /* xrm plugin related functions */
    void flushXrmPluginInfo(uint32_t xrmPluginId);
    int32_t loadOneXrmPlugin(std::string& xrmPluginFileName, std::string& xrmPluginName, std::string& errmsg);
    int32_t loadXrmPlugins(pt::ptree& loadPluginsTree, std::string& errmsg);
    int32_t unloadOneXrmPlugin(std::string& xrmPluginName, std::string& errmsg);
    int32_t unloadXrmPlugins(pt::ptree& loadPluginsTree, std::string& errmsg);
    int32_t execXrmPluginFunc(const char* xrmPluginName,
                              uint32_t funcId,
                              xrmPluginFuncParam* param,
                              std::string& errmsg);

    /* wrap function from lib */
    int32_t wrapIPName2Index(xclDeviceHandle handle, const char* ipName);
    int32_t wrapLockDevice(xclDeviceHandle handle);
    int32_t wrapUnlockDevice(xclDeviceHandle handle);
    void updateDeviceLoad(int32_t devId, int64_t loadIncreased, int64_t setLoadVal) {
        m_devList[devId].updateDeviceLoad(loadIncreased, setLoadVal);
    }

   private:
    int32_t xclbinLoadToDevice(int32_t devId, std::string& errmsg);
    int32_t openDevice(int32_t devId);
    int32_t xclbinFileReadUuid(std::string& name, std::string& uuidStr, std::string& errmsg);
    int32_t xclbinReadFile(int32_t devId, std::string& name, std::string& errmsg);
    int32_t deviceLoadXclbin(int32_t devId, std::string& xclbin, std::string& errmsg);
    int32_t loadAnyDevice(int32_t* devId, std::string& xclbin, std::string& errmsg);
    void flushDevData(int32_t devId);
    bool deviceFreeBuffer(int32_t devId);
    bool deviceClearInfo(int32_t devId);
    bool isDeviceLoaded(int32_t devId);
    bool isDeviceLoadedXclbinWithUuid(int32_t devId, uuid_t uuid);
    bool isDeviceBusy(int32_t devId);

    int32_t xclbinGetInfo(int32_t devId, std::string& errmsg);
    int32_t xclbinGetKeyvalues(int32_t devId, std::string& errmsg);
    int32_t xclbinGetUuid(int32_t devId, std::string& errmsg);
    int32_t xclbinGetLayout(int32_t devId, std::string& errmsg);
    int32_t xclbinGetConnectivity(int32_t devId, std::string& errmsg);
    int32_t xclbinGetMemTopology(int32_t devId, std::string& errmsg);

    void deviceDumpResource(int32_t devId);
    int32_t deviceLockXclbin(int32_t devId);
    int32_t deviceUnlockXclbin(int32_t devId);

    int32_t cuFindFreeChannelId(cuData* cu);
    void cuInitChannels(cuData* cu);

    int32_t allocDevForClient(int32_t* devId, cuProperty* cuProp);
    int32_t getNextFreeDevForClient(int32_t* devId, cuProperty* cuProp);
    int32_t verifyProcess(pid_t pid);
    int32_t allocClientFromDev(int32_t devId, cuProperty* cuProp);
    int32_t allocCuFromDev(int32_t devId, cuProperty* cuProp, cuResource* cuRes);
    // allocChanClientFromCu is compatible with previous version (no policy support for allocating cu) if default last
    // two parameters
    int32_t allocChanClientFromCu(
        cuData* cu, cuProperty* cuProp, cuResource* cuRes, uint64_t policyVal = 0, cuData** preCu = NULL);
    void addClientToCu(cuData* cu, uint64_t clientId);
    int isClientUsingCu(cuData* cu, uint64_t clientId);

    int32_t releaseClientOnDev(int32_t devId, uint64_t clientId);
    void releaseAllCuChanClientOnDev(deviceData* dev, uint64_t clientId);
    void removeClientOnCu(cuData* cu, uint64_t clientId);

    uint64_t getNextAllocServiceId();
    void updateAllocServiceId();
    uint64_t getNextReservePoolId();
    void updateReservePoolId();

    int32_t isReservePoolUsingCu(cuData* cu, uint64_t reservePoolId);
    int32_t reserveLoadFromCu(uint64_t reservePoolId, cuData* cu, cuProperty* cuProp);
    int32_t reserveCuFromDev(uint64_t reservePoolId, int32_t devId, cuProperty* cuProp);
    int32_t relinquishCuFromDev(uint64_t reservePoolId, int32_t devId, cuProperty* cuProp);

    deviceData m_devList[XRM_MAX_XILINX_DEVICES];
    uint32_t m_logLevel;
    uint64_t m_clientId;
    uint32_t m_numConcurrentClient;
    uint32_t m_limitConcurrentClient;
    int32_t m_numDevice;
    pluginInformation m_xrmPluginList[XRM_MAX_PLUGIN_NUM];
    uint32_t m_numXrmPlugin;
    udfCuGroupInformation m_udfCuGroups[XRM_MAX_UDF_CU_GROUP_NUM];
    uint32_t m_numUdfCuGroup;
    udfCuGroupInformationV2 m_udfCuGroupsV2[XRM_MAX_UDF_CU_GROUP_NUM_V2];
    uint32_t m_numUdfCuGroupV2;
    libVersionDepFunctionsData m_libVersionDepFuncs;
    std::string m_xrtVersionFileFullPathName;
    std::string m_libXrtCoreFileFullPathName;
    uint64_t m_allocServiceId;
    uint64_t m_reservePoolId;
    pthread_mutex_t m_lock;
    bool m_devicesInited;

    friend class boost::serialization::access;

    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        std::ignore = version;
        ar& m_logLevel& m_clientId& m_numConcurrentClient& m_devList& m_numDevice& m_udfCuGroups& m_udfCuGroupsV2&
            m_numUdfCuGroup& m_numUdfCuGroupV2& m_allocServiceId& m_reservePoolId;
    }
};
} // namespace xrm

#endif // _XRM_SYSTEM_HPP_
