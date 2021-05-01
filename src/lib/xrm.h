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

/**
 * @file xrm.h
 * @brief Header for Public APIs of XRM.
 */

#ifndef _XRM_H_
#define _XRM_H_

#include <stdint.h>
#include <stdbool.h>
#include <uuid/uuid.h>

#include "xrm_limits.h"
#include "xrm_error.h"

#ifdef __cplusplus
extern "C" {
#endif

#define XRM_API_VERSION_1 1

/* cu stat */
typedef struct xrmCuStat {
    bool isBusy;
    int32_t usedLoad;    /* used load, only one type granularity at one time.
                          * bit[31 - 28] reserved
                          * bit[27 -  8] granularity of 1000000 (0 - 1000000)
                          * bit[ 7 -  0] granularity of 100 (0 - 100)
                          */
    uint8_t extData[64]; // for future extension
} xrmCuStat;

/* request compute resource property */
typedef struct xrmCuProperty {
    char kernelName[XRM_MAX_NAME_LEN];  // unique kernel name, not instance name
    char kernelAlias[XRM_MAX_NAME_LEN]; // unique alias of kernel name
    bool devExcl;                       // request exclusive device usage for this client
    int32_t requestLoad;                /* request load of the CU, only one type granularity at one time.
                                         * bit[31 - 28] reserved
                                         * bit[27 -  8] granularity of 1000000 (0 - 1000000)
                                         * bit[ 7 -  0] granularity of 100 (0 - 100)
                                         */
    uint64_t poolId;                    /* request to allocate resource from specified resource pool,
                                         * 0 means to allocate resource from system default resource pool.
                                         */
    uint8_t extData[64];                // for future extension
} xrmCuProperty;

/* list of request compute resource property */
typedef struct xrmCuListProperty {
    xrmCuProperty cuProps[XRM_MAX_LIST_CU_NUM];
    int32_t cuNum; // total number of requested cu in the list
    bool sameDevice;
    uint8_t extData[64]; // for future extension
} xrmCuListProperty;

/* compute unit resource pool property */
typedef struct xrmCuPoolProperty {
    xrmCuListProperty cuListProp;
    int32_t cuListNum; // number of such cu list
    uuid_t xclbinUuid;
    int32_t xclbinNum;   // number of such xclbin
    uint8_t extData[64]; // for future extension
} xrmCuPoolProperty;

/* user defined compute resource property */
typedef struct xrmUdfCuProperty {
    char cuName[XRM_MAX_NAME_LEN]; // unique cu name (kernelName:instanceName)
    bool devExcl;                  // request exclusive device usage for this client
    int32_t requestLoad;           /* request load of the CU, only one type granularity at one time.
                                    * bit[31 - 28] reserved
                                    * bit[27 -  8] granularity of 1000000 (0 - 1000000)
                                    * bit[ 7 -  0] granularity of 100 (0 - 100)
                                    */
    uint8_t extData[64];           // for future extension
} xrmUdfCuProperty;

/* list of user defined compute resource property */
typedef struct xrmUdfCuListProperty {
    xrmUdfCuProperty udfCuProps[XRM_MAX_LIST_CU_NUM];
    int32_t cuNum; // total number of user defined cu in the list
    bool sameDevice;
    uint8_t extData[64]; // for future extension
} xrmUdfCuListProperty;

/* user defined compute resource group property */
typedef struct xrmUdfCuGroupProperty {
    xrmUdfCuListProperty optionUdfCuListProps[XRM_MAX_UDF_CU_GROUP_OPTION_LIST_NUM];
    int32_t optionUdfCuListNum; // total number of option user defined cu list in the group
    uint8_t extData[64];        // for future extension
} xrmUdfCuGroupProperty;

/* compute resource group property */
typedef struct xrmCuGroupProperty {
    char udfCuGroupName[XRM_MAX_NAME_LEN]; // name of user defined cu group
    uint64_t poolId;                       /* request to allocate resource from specified resource pool,
                                            * 0 means to allocate resource from system default resource pool.
                                            */
    uint8_t extData[64];                   // for future extension
} xrmCuGroupProperty;

/* cu type */
typedef enum xrmCuType {
    XRM_CU_NULL = 0,
    XRM_CU_IPKERNEL = 1,   // IP kernel
    XRM_CU_SOFTKERNEL = 2, // soft kernel
} xrmCuType;

/* allocated/released compute resource */
typedef struct xrmCuResource {
    char xclbinFileName[XRM_MAX_PATH_NAME_LEN];       // include path and name
    char kernelPluginFileName[XRM_MAX_PATH_NAME_LEN]; // just the name
    char kernelName[XRM_MAX_NAME_LEN];                // kernel name
    char kernelAlias[XRM_MAX_NAME_LEN];               // unique alias of kernel name
    char instanceName[XRM_MAX_NAME_LEN];              // instance name
    char cuName[XRM_MAX_NAME_LEN];                    // cu name (kernelName:instanceName)
    uuid_t uuid;
    int32_t deviceId;
    int32_t cuId;
    int32_t channelId;
    xrmCuType cuType;
    uint64_t baseAddr;        // base address of the cu
    uint32_t membankId;       // connected memory bank id
    uint32_t membankType;     // connected memory bank type
    uint64_t membankSize;     // connected memory bank size
    uint64_t membankBaseAddr; // connected memory bank base address
    uint64_t allocServiceId;  // unique service id of the allocation
    int32_t channelLoad;      /* load of the CU for this channel, only one type granularity at one time.
                               * bit[31 - 28] reserved
                               * bit[27 -  8] granularity of 1000000 (0 - 1000000)
                               * bit[ 7 -  0] granularity of 100 (0 - 100)
                               */
    uint64_t poolId;          // id of the cu pool this cu comes from, default pool id is 0
    uint8_t extData[64];      // for future extension
} xrmCuResource;

/*
 * allocated/released compute resource list
 */
typedef struct xrmCuListResource {
    xrmCuResource cuResources[XRM_MAX_LIST_CU_NUM];
    int32_t cuNum;
    uint8_t extData[64]; // for future extension
} xrmCuListResource;

/*
 * allocated/released compute resource group
 */
typedef struct xrmCuGroupResource {
    xrmCuResource cuResources[XRM_MAX_GROUP_CU_NUM];
    int32_t cuNum;
    uint8_t extData[64]; // for future extension
} xrmCuGroupResource;

/*
 * reserved/relinquished compute resource pool
 */
typedef struct xrmCuPoolResource {
    xrmCuResource cuResources[XRM_MAX_POOL_CU_NUM];
    int32_t cuNum;
    uint8_t extData[64]; // for future extension
} xrmCuPoolResource;

/* Alloction information for querying */
typedef struct xrmAllocationQueryInfo {
    uint64_t allocServiceId;            // the service id returned from allocation
    char kernelName[XRM_MAX_NAME_LEN];  // kernel name, not instance name
    char kernelAlias[XRM_MAX_NAME_LEN]; // unique alias of kernel name
    uint8_t extData[64];                // for future extension
} xrmAllocationQueryInfo;

/*
 * plugin related data struct
 */
typedef struct xrmPluginFuncParam {
    char input[XRM_MAX_PLUGIN_FUNC_PARAM_LEN];
    char output[XRM_MAX_PLUGIN_FUNC_PARAM_LEN];
} xrmPluginFuncParam;

typedef int32_t (*xrmPluginVersionType)();
typedef int32_t (*xrmApiVersionType)();
typedef int32_t (*xrmPluginFuncType)(xrmPluginFuncParam*);

typedef struct xrmPluginData {
    xrmPluginVersionType pluginVersion;
    xrmApiVersionType apiVersion;
    xrmPluginFuncType pluginFunc[XRM_MAX_PLUGIN_FUNC_ID + 1];
    uint32_t extData[4]; // for future extension
} xrmPluginData;

typedef enum xrmLogLevelType {
    XRM_LOG_EMERGENCY = 0,
    XRM_LOG_ALERT = 1,
    XRM_LOG_CRITICAL = 2,
    XRM_LOG_ERROR = 3,
    XRM_LOG_WARNING = 4,
    XRM_LOG_NOTICE = 5,
    XRM_LOG_INFO = 6,
    XRM_LOG_DEBUG = 7
} xrmLogLevelType;

typedef void* xrmContext;

/**
 * \brief Establishes a connection with the XRM daemon
 *
 * @param xrmApiVersion the XRM API version number
 * @return xrmContext, pointer to created context or NULL on fail
 */
xrmContext xrmCreateContext(uint32_t xrmApiVersion);

/**
 * \brief Disconnects an existing connection with the XRM daemon
 *
 * @param context the context created through xrmCreateContext()
 * @return int32_t, 0 on success or appropriate error number
 */
int32_t xrmDestroyContext(xrmContext context);

/**
 * \brief To check whether the daemon is running
 *
 * @param context the context created through xrmCreateContext()
 * @return bool, true on running or false on NOT running
 */
bool xrmIsDaemonRunning(xrmContext context);

/**
 * \brief Enable one device
 *
 * @param context the context created through xrmCreateContext()
 * @param deviceId the device id to enable
 * @return 0 on success or appropriate error number
 */
int32_t xrmEnableOneDevice(xrmContext context, int32_t deviceId);

/**
 * \brief Disable one device
 *
 * @param context the context created through xrmCreateContext()
 * @param deviceId the device id to disable
 * @return int32_t, 0 on success or appropriate error number
 */
int32_t xrmDisableOneDevice(xrmContext context, int32_t deviceId);

/**
 * \brief Loads xclbin to one device
 *
 * @param context the context created through xrmCreateContext()
 * @param deviceId the device id to load the xclbin file, -1 means to any available device
 * @param xclbinFileName xclbin file (full path and name)
 * @return int32_t, device id (>= 0) loaded with xclbin or appropriate error number (< 0) on fail
 */
int32_t xrmLoadOneDevice(xrmContext context, int32_t deviceId, char* xclbinFileName);

/**
 * \brief Unloads xclbin from one device
 *
 * @param context the context created through xrmCreateContext()
 * @param deviceId the device id to unload the xclbin file
 * @return int32_t, 0 on success or appropriate error number
 */
int32_t xrmUnloadOneDevice(xrmContext context, int32_t deviceId);

/**
 * \brief Allocates compute unit with a device, cu, and channel given a
 * kernel name or alias or both and request load. This function also
 * provides the xclbin and kernel plugin loaded on the device.
 *
 * @param context the context created through xrmCreateContext()
 * @param cuProp the property of requested cu.
 *             kernelName: the kernel name requested.
 *             kernelAlias: the alias of kernel name requested.
 *             devExcl: request exclusive device usage for this client.
 *             requestLoad: request load, only one type granularity at one time.
 *                          bit[31 - 28] reserved
 *                          bit[27 -  8] granularity of 1000000 (0 - 1000000)
 *                          bit[ 7 -  0] granularity of 100 (0 - 100)
 *             poolId: request to allocate cu from specified resource pool
 * @param cuRes the cu resource.
 *             xclbinFileName: xclbin (path and name) attached to this device.
 *             kernelPluginFileName: kernel plugin (only name) attached to this device.
 *             kernelName: the kernel name of allocated cu.
 *             kernelAlias: the name alias of allocated cu.
 *             instanceName: the instance name of allocated cu.
 *             cuName: the name of allocated cu (kernelName:instanceName).
 *             uuid: uuid of the loaded xclbin file.
 *             deviceId: device id of this cu.
 *             cuId: cu id of this cu.
 *             channelId: channel id of this cu.
 *             cuType: type of cu, hardware kernel or soft kernel.
 *             allocServiceId: service id for this cu allocation.
 *             channelLoad: allocated load of this cu, only one type granularity at one time.
 *                          bit[31 - 28] reserved
 *                          bit[27 -  8] granularity of 1000000 (0 - 1000000)
 *                          bit[ 7 -  0] granularity of 100 (0 - 100)
 *             poolId: id of the cu pool this cu comes from, the system default pool id is 0.
 * @return int32_t, 0 on success or appropriate error number
 */
int32_t xrmCuAlloc(xrmContext context, xrmCuProperty* cuProp, xrmCuResource* cuRes);

/**
 * \brief Allocates compute unit from specified device given a
 * kernel name or alias or both and request load. This function also
 * provides the xclbin and kernel plugin loaded on the device.
 *
 * @param context the context created through xrmCreateContext()
 * @param deviceId the id of target device to allocate the cu.
 * @param cuProp the property of requested cu.
 *             kernelName: the kernel name requested.
 *             kernelAlias: the alias of kernel name requested.
 *             devExcl: request exclusive device usage for this client.
 *             requestLoad: request load, only one type granularity at one time.
 *                          bit[31 - 28] reserved
 *                          bit[27 -  8] granularity of 1000000 (0 - 1000000)
 *                          bit[ 7 -  0] granularity of 100 (0 - 100)
 *             poolId: request to allocate cu from specified resource pool
 * @param cuRes the cu resource.
 *             xclbinFileName: xclbin (path and name) attached to this device.
 *             kernelPluginFileName: kernel plugin (only name) attached to this device.
 *             kernelName: the kernel name of allocated cu.
 *             kernelAlias: the name alias of allocated cu.
 *             instanceName: the instance name of allocated cu.
 *             cuName: the name of allocated cu (kernelName:instanceName).
 *             uuid: uuid of the loaded xclbin file.
 *             deviceId: device id of this cu.
 *             cuId: cu id of this cu.
 *             channelId: channel id of this cu.
 *             cuType: type of cu, hardware kernel or soft kernel.
 *             allocServiceId: service id for this cu allocation.
 *             channelLoad: allocated load of this cu, only one type granularity at one time.
 *                          bit[31 - 28] reserved
 *                          bit[27 -  8] granularity of 1000000 (0 - 1000000)
 *                          bit[ 7 -  0] granularity of 100 (0 - 100)
 *             poolId: id of the cu pool this cu comes from, the system default pool id is 0.
 * @return int32_t, 0 on success or appropriate error number
 */
int32_t xrmCuAllocFromDev(xrmContext context, int32_t deviceId, xrmCuProperty* cuProp, xrmCuResource* cuRes);

/**
 * \brief Allocates a list of compute unit resource given a list of
 * kernels's property with kernel name or alias or both and request load.
 *
 * @param context the context created through xrmCreateContext()
 * @param cuListProp the property of cu list.
 *             cuProps: cu prop list to fill kernelName, devExcl and requestLoad, starting from cuProps[0], no hole.
 *             cuNum: request number of cu in this list.
 *             sameDevice: request this list of cu from same device.
 * @param cuListRes the cu list resource.
 *             cuResources: cu resource list to fill the allocated cus infor, starting from cuResources[0], no hole.
 *             cuNum: allocated cu number in this list.
 * @return int32_t, 0 on success or appropriate error number
 */
int32_t xrmCuListAlloc(xrmContext context, xrmCuListProperty* cuListProp, xrmCuListResource* cuListRes);

/**
 * \brief Releases a previously allocated resource
 *
 * @param context the context created through xrmCreateContext()
 * @param cuRes the cu resource.
 *             xclbinFileName: xclbin (path and name) attached to this device.
 *             kernelPluginFileName: kernel plugin (only name) attached to this device.
 *             kernelName: the kernel name of allocated cu.
 *             kernelAlias: the name alias of allocated cu.
 *             instanceName: the instance name of allocated cu.
 *             cuName: the name of allocated cu (kernelName:instanceName).
 *             uuid: uuid of the loaded xclbin file.
 *             deviceId: device id of this cu.
 *             cuId: cu id of this cu.
 *             channelId: channel id of this cu.
 *             cuType: type of cu, hardware kernel or soft kernel.
 *             allocServiceId: service id for this cu allocation.
 *             channelLoad: allocated load of this cu, only one type granularity at one time.
 *                          bit[31 - 28] reserved
 *                          bit[27 -  8] granularity of 1000000 (0 - 1000000)
 *                          bit[ 7 -  0] granularity of 100 (0 - 100)
 *             poolId: id of the cu pool this cu comes from, the system default pool id is 0.
 * @return bool, true on success or false on fail
 */
bool xrmCuRelease(xrmContext context, xrmCuResource* cuRes);

/**
 * \brief Releases a previously allocated list of resources
 *
 * @param context the context created through xrmCreateContext()
 * @param cuListRes the cu list resource.
 *             cuResources: cu resource list to be released, starting from cuResources[0], no hole.
 *             cuNum: number of cu in this list.
 * @return bool, true on success or false on fail
 */
bool xrmCuListRelease(xrmContext context, xrmCuListResource* cuListRes);

/**
 * \brief Declares user defined cu group type given the specified
 * kernels's property with cu name (kernelName:instanceName) and request load.
 *
 * @param context the context created through xrmCreateContext()
 * @param udfCuGroupProp the property of user defined cu group.
 *             optionUdfCuListProps[]: option cu list property array starting from optionCuListProps[0], no hole.
 *             optionUdfCuListNum: number of option user defined cu list.
 * @param udfCuGroupName unique user defined cu group name for the new group type declaration
 * @return int32_t, 0 on success or appropriate error number
 */
int32_t xrmUdfCuGroupDeclare(xrmContext context, xrmUdfCuGroupProperty* udfCuGroupProp, char* udfCuGroupName);

/**
 * \brief Undeclares user defined cu group type given the specified
 * group name.
 *
 * @param context the context created through xrmCreateContext()
 * @param udfCuGroupName user defined cu group name for the group type undeclaration
 * @return int32_t, 0 on success or appropriate error number
 */
int32_t xrmUdfCuGroupUndeclare(xrmContext context, char* udfCuGroupName);

/**
 * \brief Allocates a group of compute unit resource given a user defined group of
 * kernels's property with cu name (kernelName:instanceName) and request load.
 *
 * @param context the context created through xrmCreateContext()
 * @param cuGroupProp the property of cu group.
 *             udfCuGroupName: user defined cu group type name.
 *             poolId: id of the cu pool this group CUs come from, the system default pool id is 0.
 * @param cuGroupRes the cu group resource.
 *             cuResources: cu resource group to fill the allocated cus infor, starting from cuResources[0], no hole.
 *             cuNum: allocated cu number in this group.
 * @return int32_t, 0 on success or appropriate error number
 */
int32_t xrmCuGroupAlloc(xrmContext context, xrmCuGroupProperty* cuGroupProp, xrmCuGroupResource* cuGroupRes);

/**
 * \brief Releases a previously allocated group of resources
 *
 * @param context the context created through xrmCreateContext()
 * @param cuGroupRes cu group resource.
 *             cuResources: cu resource group to be released, starting from cuResources[0], no hole.
 *             cuNum: number of cu in this group.
 * @return bool, true on success or false on fail
 */
bool xrmCuGroupRelease(xrmContext context, xrmCuGroupResource* cuGroupRes);

/**
 * \brief Retrieves the maximum capacity associated with a resource
 *
 * @param context the context created through xrmCreateContext()
 * @param cuProp the property of cu.
 *             kernelName: the kernel name requested.
 *             kernelAlias: the alias of kernel name requested.
 * @return uint64_t, the max capacity of the cu (> 0) or 0 if cu is not existing in system or max capacity
 *          is not described.
 */
uint64_t xrmCuGetMaxCapacity(xrmContext context, xrmCuProperty* cuProp);

/**
 * \brief To check the status of specified cu resource
 *
 * @param context the context created through xrmCreateContext()
 * @param cuRes the cu resource.
 *             deviceId: device id of this cu.
 *             cuId: cu id of this cu.
 *             channelId: channel id of this cu.
 *             cuType: type of cu, hardware kernel or soft kernel.
 *             allocServiceId: service id for this cu allocation.
 * @param cuStat the status of cu.
 *             isBusy: the cu is busy or not.
 *             usedLoad: allocated load on this cu, only one type granularity at one time.
 *                       bit[31 - 28] reserved
 *                       bit[27 -  8] granularity of 1000000 (0 - 1000000)
 *                       bit[ 7 -  0] granularity of 100 (0 - 100)
 * @return int32_t, 0 on success or appropriate error number
 */
int32_t xrmCuCheckStatus(xrmContext context, xrmCuResource* cuRes, xrmCuStat* cuStat);

/**
 * \brief Querys the compute unit resource given the allocation service id.
 *
 * @param context the context created through xrmCreateContext()
 * @param allocQuery the allocate query information.
 *             allocServiceId: the service id returned from allocation.
 *             kernelName: the kernel name requested.
 *             kernelAlias: the alias of kernel name requested.
 * @param cuListRes cu list resource.
 *             cuListRes: cu resource list to fill the allocated cus infor, starting from cuResources[0], no hole.
 *             cuNum: cu number in this list.
 * @return int32_t, 0 on success or appropriate error number
 */
int32_t xrmAllocationQuery(xrmContext context, xrmAllocationQueryInfo* allocQuery, xrmCuListResource* cuListRes);

/**
 * \brief To check the available cu num on the system given
 * the kernels's property with kernel name or alias or both and request
 * load.
 *
 * @param context the context created through xrmCreateContext()
 * @param cuProp the property of cu.
 *             kernelName: the kernel name requested.
 *             kernelAlias: the alias of kernel name requested.
 *             devExcl: request exclusive device usage for this client.
 *             requestLoad: request load, only one type granularity at one time.
 *                          bit[31 - 28] reserved
 *                          bit[27 -  8] granularity of 1000000 (0 - 1000000)
 *                          bit[ 7 -  0] granularity of 100 (0 - 100)
 *             poolId: request to allocate cu from specified resource pool.
 * @return int32_t, available cu num (>= 0) on success or appropriate error number (< 0).
 */
int32_t xrmCheckCuAvailableNum(xrmContext context, xrmCuProperty* cuProp);

/**
 * \brief To check the available cu list num on the system given
 * a list of kernels's property with kernel name or alias or both and request
 * load.
 *
 * @param context the context created through xrmCreateContext()
 * @param cuListProp the property of cu list.
 *             cuProps: cu prop list to fill kernelName, devExcl and requestLoad, starting from cuProps[0], no hole
 *             cuNum: request number of cu in this list.
 *             sameDevice: request this list of cu from same device.
 * @return int32_t, available cu list num (>= 0) on success or appropriate error number (< 0).
 */
int32_t xrmCheckCuListAvailableNum(xrmContext context, xrmCuListProperty* cuListProp);

/**
 * \brief To check the available group number of compute unit resource given a user
 * defined group of kernels's property with cu name (kernelName:instanceName) and request load.
 *
 * @param context the context created through xrmCreateContext()
 * @param cuGroupProp the property of cu group.
 *             udfCuGroupName: user defined cu group type name.
 *             poolId: id of the cu pool this group CUs come from, the system default pool id is 0.
 * @return int32_t, available cu group num (>= 0) on success or appropriate error number (< 0).
 */
int32_t xrmCheckCuGroupAvailableNum(xrmContext context, xrmCuGroupProperty* cuGroupProp);

/**
 * \brief To check the available cu pool num on the system given
 * a pool of kernels's property with kernel name or alias or both and request
 * load.
 *
 * @param context the context created through xrmCreateContext()
 * @param cuPoolProp the property of cu pool.
 *             cuListProp: cu list property.
 *             cuListNum: number of cu list in this pool.
 *             xclbinUuid: uuid of xclbin.
 *             xclbinNum: number of xclbin in this pool.
 * @return int32_t, available cu pool num (>= 0) on success or appropriate error number (< 0).
 */
int32_t xrmCheckCuPoolAvailableNum(xrmContext context, xrmCuPoolProperty* cuPoolProp);

/**
 * \brief Reserves a pool of compute unit resource given a pool of
 * kernels's property with kernel name or alias or both and request load.
 *
 * @param context the context created through xrmCreateContext()
 * @param cuPoolProp the property of cu pool.
 *             cuListProp: cu prop list to fill kernelName, devExcl and requestLoad etc. information.
 *             cuListNum: request number of such cu list for this pool.
 *             xclbinUuid: request all resource in the xclbin.
 *             xclbinNum: request number of such xclbin for this pool.
 * @return uint64_t, reserve pool id (> 0) or 0 on fail
 */
uint64_t xrmCuPoolReserve(xrmContext context, xrmCuPoolProperty* cuPoolProp);

/**
 * \brief Relinquishes a previously reserved pool of resources
 *
 * @param context the context created through xrmCreateContext()
 * @param poolId the reserve pool id
 * @return bool, true on success or false on fail
 */
bool xrmCuPoolRelinquish(xrmContext context, uint64_t poolId);

/**
 * \brief Querys the compute unit resource given the reservation id.
 * NOTE: The allocServiceId, channelId and channelLoad are NOT valid in the cuPoolRes
 *
 * @param context the context created through xrmCreateContext()
 * @param poolId the reserve pool id
 * @param cuPoolRes the cu pool resource.
 *             cuPoolRes: cu resource pool to fill the allocated cus infor, starting from cuPoolRes[0], no hole.
 *             cuNum: cu number in this pool.
 * @return int32_t, 0 on success or appropriate error number
 */
int32_t xrmReservationQuery(xrmContext context, uint64_t poolId, xrmCuPoolResource* cuPoolRes);

/**
 * \brief Execuates the function of one specified xrm plugin.
 *
 * @param context the context created through xrmCreateContext()
 * @param xrmPluginName name of the xrm plugin
 * @param funcId the function id within xrm plugin
 * @param param the parameter struct for xrm plugin function
 * @return int32_t, 0 on success or appropriate error number
 */
int32_t xrmExecPluginFunc(xrmContext context, char* xrmPluginName, uint32_t funcId, xrmPluginFuncParam* param);

/**
 * Following APIs are NOT stable interface and may be changed in coming future.
 */

/**
 * \brief Allocates compute unit with a device, cu, and channel given a
 * kernel name or alias or both and request load. This function also
 * provides the xclbin and kernel plugin loaded on the device. If required CU is not
 * available, this function will try to load the xclbin to one device and do the
 * allocation again.
 *
 * @param context the context created through xrmCreateContext()
 * @param cuProp the property of cu.
 *             kernelName: the kernel name requested.
 *             kernelAlias: the alias of kernel name requested.
 *             devExcl: request exclusive device usage for this client.
 *             requestLoad: request load, only one type granularity at one time.
 *                          bit[31 - 28] reserved
 *                          bit[27 -  8] granularity of 1000000 (0 - 1000000)
 *                          bit[ 7 -  0] granularity of 100 (0 - 100)
 *             poolId: request to allocate cu from specified resource pool.
 * @param xclbinFileName xclbin file (full path and name)
 * @param cuRes cu resource.
 *             xclbinFileName: xclbin (path and name) attached to this device.
 *             kernelPluginFileName: kernel plugin (only name) attached to this device.
 *             kernelName: the kernel name of allocated cu.
 *             kernelAlias: the name alias of allocated cu.
 *             instanceName: the instance name of allocated cu.
 *             cuName: the name of allocated cu (kernelName:instanceName).
 *             uuid: uuid of the loaded xclbin file.
 *             deviceId: device id of this cu.
 *             cuId: cu id of this cu.
 *             channelId: channel id of this cu.
 *             cuType: type of cu, hardware kernel or soft kernel.
 *             allocServiceId: service id for this cu allocation.
 *             channelLoad: allocated load of this cu, only one type granularity at one time.
 *                          bit[31 - 28] reserved
 *                          bit[27 -  8] granularity of 1000000 (0 - 1000000)
 *                          bit[ 7 -  0] granularity of 100 (0 - 100)
 *             poolId: id of the cu pool this cu comes from, the default pool id is 0.
 * @return int32_t, 0 on success or appropriate error number
 */
int32_t xrmCuAllocWithLoad(xrmContext context, xrmCuProperty* cuProp, char* xclbinFileName, xrmCuResource* cuRes);

/**
 * \brief Loads xclbin to one device, then allocates all CUs from this device.
 *
 * @param context the context created through xrmCreateContext()
 * @param xclbinFileName xclbin file (full path and name)
 * @param cuListRes cu list resource.
 *             cuResources: cu resource list to fill the allocated cus infor, starting from cuResources[0], no hole.
 *             cuNum: allocated cu number in this list.
 * @return int32_t, 0 on success or appropriate error number
 */
int32_t xrmLoadAndAllCuAlloc(xrmContext context, char* xclbinFileName, xrmCuListResource* cuListRes);

/**
 * \brief Blocking function of xrmCuAlloc(), this function will try to do cu allocation
 * until success.
 *
 * @param context the context created through xrmCreateContext()
 * @param cuProp the property of cu.
 *             kernelName: the kernel name requested.
 *             kernelAlias: the alias of kernel name requested.
 *             devExcl: request exclusive device usage for this client.
 *             requestLoad: request load, only one type granularity at one time.
 *                          bit[31 - 28] reserved
 *                          bit[27 -  8] granularity of 1000000 (0 - 1000000)
 *                          bit[ 7 -  0] granularity of 100 (0 - 100)
 *             poolId: request to allocate cu from specified resource pool.
 * @param interval the interval time (useconds) before re-trying, [0 - 1000000], other value is invalid.
 * @param cuRes cu resource.
 *             xclbinFileName: xclbin (path and name) attached to this device.
 *             kernelPluginFileName: kernel plugin (only name) attached to this device.
 *             kernelName: the kernel name of allocated cu.
 *             kernelAlias: the name alias of allocated cu.
 *             instanceName: the instance name of allocated cu.
 *             cuName: the name of allocated cu (kernelName:instanceName).
 *             uuid: uuid of the loaded xclbin file.
 *             deviceId: device id of this cu.
 *             cuId: cu id of this cu.
 *             channelId: channel id of this cu.
 *             cuType: type of cu, hardware kernel or soft kernel.
 *             allocServiceId: service id for this cu allocation.
 *             channelLoad: allocated load of this cu, only one type granularity at one time.
 *                          bit[31 - 28] reserved
 *                          bit[27 -  8] granularity of 1000000 (0 - 1000000)
 *                          bit[ 7 -  0] granularity of 100 (0 - 100)
 *             poolId: id of the cu pool this cu comes from, the default pool id is 0.
 * @return int32_t, 0 on success or appropriate error number
 */
int32_t xrmCuBlockingAlloc(xrmContext context, xrmCuProperty* cuProp, uint64_t interval, xrmCuResource* cuRes);

/**
 * \brief Blocking function of xrmCuListAlloc(), this function will try to do cu list allocation
 * until success.
 *
 * @param context the context created through xrmCreateContext()
 * @param cuListProp the property of cu list.
 *             cuProps: cu prop list to fill kernelName, devExcl and requestLoad, starting from cuProps[0], no hole.
 *             cuNum: request number of cu in this list.
 *             sameDevice request this list of cu from same device.
 * @param interval the interval time (useconds) before re-trying, [0 - 1000000], other value is invalid.
 * @param cuListRes cu list resource.
 *             cuResources: cu resource list to fill the allocated cus infor, starting from cuResources[0], no hole.
 *             cuNum: allocated cu number in this list.
 * @return int32_t, 0 on success or appropriate error number
 */
int32_t xrmCuListBlockingAlloc(xrmContext context,
                               xrmCuListProperty* cuListProp,
                               uint64_t interval,
                               xrmCuListResource* cuListRes);

/**
 * \brief Blocking function of xrmCuGroupAlloc(), this function will try to do cu group
 * allocation until success.
 *
 * @param context the context created through xrmCreateContext()
 * @param cuGroupProp the property of cu group.
 *            udfCuGroupName: user defined cu group type name.
 *            poolId: id of the cu pool this group CUs come from, the system default pool id is 0.
 * @param interval the interval time (useconds) before re-trying, [0 - 1000000], other value is invalid.
 * @param cuGroupRes cu group resource.
 *            cuResources cu resource group to fill the allocated cus infor, starting from cuResources[0], no hole.
 *            cuNum allocated cu number in this list.
 * @return int32_t, 0 on success or appropriate error number
 */
int32_t xrmCuGroupBlockingAlloc(xrmContext context,
                                xrmCuGroupProperty* cuGroupProp,
                                uint64_t interval,
                                xrmCuGroupResource* cuGroupRes);

#ifdef __cplusplus
}
#endif

#endif // _XRM_H_
