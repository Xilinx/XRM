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
    int32_t usedLoad;    // Percentage: 0 - 100
    uint8_t extData[64]; // for future extension
} xrmCuStat;

/* request compute resource property */
typedef struct xrmCuProperty {
    char kernelName[XRM_MAX_NAME_LEN];  // unique kernel name, not instance name
    char kernelAlias[XRM_MAX_NAME_LEN]; // unique alias of kernel name
    bool devExcl;                       // request exclusive device usage for this client
    int32_t requestLoad;                // request load percentage of the CU: 1 - 100
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
    int32_t requestLoad;           // request load percentage of the CU: 1 - 100
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
    int32_t channelLoad;      // load percentage of the CU for this channel: 1 - 100
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
 * xrmCreateContext() - establishes a connection with the XRM daemon
 *
 * @xrmApiVersion: the XRM API version number
 * @return: xrmContext, pointer to created context or NULL on fail
 **/
xrmContext xrmCreateContext(uint32_t xrmApiVersion);

/**
 * xrmDestroyContext() - disconnects an existing connection with the XRM daemon
 *
 * @context: the context created through xrmCreateContext()
 * @return: int32_t, 0 on success or appropriate error number
 **/
int32_t xrmDestroyContext(xrmContext context);

/**
 * xrmIsDaemonRunning() - check whether the daemon is running
 *
 * @context: the context created through xrmCreateContext()
 * @return value: bool, true on running or false on NOT running
 **/
bool xrmIsDaemonRunning(xrmContext context);

/**
 * xrmLoadOneDevice() - loads xclbin to one device
 *
 * @context: the context created through xrmCreateContext()
 * @deviceId: the device id to load the xclbin file, -1 means to any available device
 * @xclbinFileName: xclbin file (full path and name)
 * @return: int32_t, device id (>= 0) loaded with xclbin or appropriate error number (< 0) on fail
 **/
int32_t xrmLoadOneDevice(xrmContext context, int32_t deviceId, char* xclbinFileName);

/**
 * xrmUnloadOneDevice() - unloads xclbin from one device
 *
 * @context: the context created through xrmCreateContext()
 * @deviceId: the device id to unload the xclbin file
 * @return: int32_t, 0 on success or appropriate error number
 **/
int32_t xrmUnloadOneDevice(xrmContext context, int32_t deviceId);

/**
 * xrmCuAlloc() - allocates compute unit with a device, cu, and channel given a
 * kernel name or alias or both and request load (1 - 100). This function also
 * provides the xclbin and kernel plugin loaded on the device.
 *
 * @context: the context created through xrmCreateContext()
 * @cuProp:
 *      kernelName: the kernel name requested
 *      kernelAlias: the alias of kernel name requested
 *      devExcl: request exclusive device usage for this client
 *      requestLoad: request load (1 - 100)
 *      poolId: request to allocate cu from specified resource pool
 * @cuRes:
 *      xclbinFileName: xclbin (path and name) attached to this device
 *      kernelPluginFileName: kernel plugin (only name) attached to this device
 *      kernelName: the kernel name of allocated cu
 *      kernelAlias: the name alias of allocated cu
 *      instanceName: the instance name of allocated cu
 *      cuName: the name of allocated cu (kernelName:instanceName)
 *      uuid: uuid of the loaded xclbin file
 *      deviceId: device id of this cu
 *      cuId: cu id of this cu
 *      channelId: channel id of this cu
 *      cuType: type of cu, hardware kernel or soft kernel
 *      allocServiceId: service id for this cu allocation
 *      channelLoad: allocated load of this cu (1 - 100)
 *      poolId: id of the cu pool this cu comes from, the default pool id is 0
 * @return: int32_t, 0 on success or appropriate error number
 **/
int32_t xrmCuAlloc(xrmContext context, xrmCuProperty* cuProp, xrmCuResource* cuRes);

/**
 * xrmCuListAlloc() - allocates a list of compute unit resource given a list of
 * kernels's property with kernel name or alias or both and request load (1 - 100).
 *
 * @context: the context created through xrmCreateContext()
 * @cuListProp:
 *      cuProps: cu prop list to fill kernelName, devExcl and requestLoad, starting from cuProps[0], no hole.
 *      cuNum: request number of cu in this list
 *      sameDevice: request this list of cu from same device
 * @cuListRes:
 *      cuResources: cu resource list to fill the allocated cus infor, starting from cuResources[0], no hole.
 *      cuNum: allocated cu number in this list
 * @return: int32_t, 0 on success or appropriate error number
 **/
int32_t xrmCuListAlloc(xrmContext context, xrmCuListProperty* cuListProp, xrmCuListResource* cuListRes);

/**
 * xrmCuRelease() - releases a previously allocated resource
 *
 * @context: the context created through xrmCreateContext()
 * @cuRes:
 *      xclbinFileName: xclbin (path and name) attached to this device
 *      kernelPluginFileName: kernel plugin (only name) attached to this device
 *      kernelName: the kernel name of allocated cu
 *      kernelAlias: the name alias of allocated cu
 *      instanceName: the instance name of allocated cu
 *      cuName: the name of allocated cu (kernelName:instanceName)
 *      uuid: uuid of the loaded xclbin file
 *      deviceId: device id of this cu
 *      cuId: cu id of this cu
 *      channelId: channel id of this cu
 *      cuType: type of cu, hardware kernel or soft kernel
 *      allocServiceId: service id for this cu allocation
 *      channelLoad: allocated load of this cu (1 - 100)
 *      poolId: id of the cu pool this cu comes from, the system default pool id is 0
 * @return: bool, true on success or false on fail
 **/
bool xrmCuRelease(xrmContext context, xrmCuResource* cuRes);

/**
 * xrmCuListRelease() - releases a previously allocated list of resources
 *
 * @context: the context created through xrmCreateContext()
 * @cuListRes:
 *      cuResources: cu resource list to be released, starting from cuResources[0], no hole.
 *      cuNum: number of cu in this list
 * @return: bool, true on success or false on fail
 **/
bool xrmCuListRelease(xrmContext context, xrmCuListResource* cuListRes);

/**
 * xrmUdfCuGroupDeclare() - declare user defined cu group type given the specified
 * kernels's property with cu name (kernelName:instanceName) and request load (1 - 100).
 *
 * @context: the context created through xrmCreateContext()
 * @udfCuGroupProp:
 *      optionUdfCuListProps[]: option user defined cu list property array starting from optionCuListProps[0], no hole.
 *      optionUdfCuListNum: number of option user defined cu list
 * @udfCuGroupName: unique user defined cu group name for the new group type declaration
 * @return: int32_t, 0 on success or appropriate error number
 **/
int32_t xrmUdfCuGroupDeclare(xrmContext context, xrmUdfCuGroupProperty* udfCuGroupProp, char* udfCuGroupName);

/**
 * xrmUdfCuGroupUndeclare() - undeclare user defined cu group type given the specified
 * group name.
 *
 * @context: the context created through xrmCreateContext()
 * @udfCuGroupName: user defined cu group name for the group type undeclaration
 * @return: int32_t, 0 on success or appropriate error number
 **/
int32_t xrmUdfCuGroupUndeclare(xrmContext context, char* udfCuGroupName);

/**
 * xrmCuGroupAlloc() - allocates a group of compute unit resource given a user defined group of
 * kernels's property with cu name (kernelName:instanceName) and request load (1 - 100).
 *
 * @context: the context created through xrmCreateContext()
 * @cuGroupProp:
 *      udfCuGroupName: user defined cu group type name
 *      poolId: id of the cu pool this group CUs come from, the system default pool id is 0
 * @cuGroupRes:
 *      cuResources: cu resource group to fill the allocated cus infor, starting from cuResources[0], no hole.
 *      cuNum: allocated cu number in this group
 * @return: int32_t, 0 on success or appropriate error number
 **/
int32_t xrmCuGroupAlloc(xrmContext context, xrmCuGroupProperty* cuGroupProp, xrmCuGroupResource* cuGroupRes);

/**
 * xrmCuGroupRelease() - releases a previously allocated group of resources
 *
 * @context: the context created through xrmCreateContext()
 * @cuGroupRes:
 *      cuResources: cu resource group to be released, starting from cuResources[0], no hole.
 *      cuNum: number of cu in this group
 * @return: bool, true on success or false on fail
 **/
bool xrmCuGroupRelease(xrmContext context, xrmCuGroupResource* cuGroupRes);

/**
 * xrmCuGetMaxCapacity() retrieves the maximum capacity associated with a resource
 *
 * @context: the context created through xrmCreateContext()
 * @cuProp:
 *      kernelName: the kernel name requested
 *      kernelAlias: the alias of kernel name requested
 * @return: uint64_t, the max capacity of the cu (> 0) or 0 if cu is not existing in system or max capacity
 *          is not described.
 **/
uint64_t xrmCuGetMaxCapacity(xrmContext context, xrmCuProperty* cuProp);

/**
 * xrmCuCheckStatus() - returns whether or not a specified cu resource is busy
 *
 * @context: the context created through xrmCreateContext()
 * @cuRes:
 *      deviceId: device id of this cu
 *      cuId: cu id of this cu
 *      channelId: channel id of this cu
 *      cuType: type of cu, hardware kernel or soft kernel
 *      allocServiceId: service id for this cu allocation
 * @cuStat:
 *      isBusy: the cu is busy or not
 *      usedLoad: allocated load on this cu
 * @return: int32_t, 0 on success or appropriate error number
 **/
int32_t xrmCuCheckStatus(xrmContext context, xrmCuResource* cuRes, xrmCuStat* cuStat);

/**
 * xrmAllocationQuery() - query the compute unit resource given the allocation service id.
 *
 * @context: the context created through xrmCreateContext()
 * @allocQuery:
 *      allocServiceId: the service id returned from allocation
 *      kernelName: the kernel name requested
 *      kernelAlias: the alias of kernel name requested
 * @cuListRes:
 *      cuListRes: cu resource list to fill the allocated cus infor, starting from cuResources[0], no hole.
 *      cuNum: cu number in this list
 * @return: int32_t, 0 on success or appropriate error number
 **/
int32_t xrmAllocationQuery(xrmContext context, xrmAllocationQueryInfo* allocQuery, xrmCuListResource* cuListRes);

/**
 * xrmCheckCuAvailableNum() - check the available cu num on the system given
 * the kernels's property with kernel name or alias or both and request
 * load (1 - 100).
 *
 * @context: the context created through xrmCreateContext()
 * @cuProp:
 *      kernelName: the kernel name requested
 *      kernelAlias: the alias of kernel name requested
 *      devExcl: request exclusive device usage for this client
 *      requestLoad: request load (1 - 100)
 *      poolId: request to allocate cu from specified resource pool
 * @return: int32_t, available cu num (>= 0) on success or appropriate error number (< 0), if available
 *          cu number is >= XRM_MAX_AVAILABLE_CU_NUM, will only return XRM_MAX_AVAILABLE_CU_NUM.
 **/
int32_t xrmCheckCuAvailableNum(xrmContext context, xrmCuProperty* cuProp);

/**
 * xrmCheckCuListAvailableNum() - check the available cu list num on the system given
 * a list of kernels's property with kernel name or alias or both and request
 * load (1 - 100).
 *
 * @context: the context created through xrmCreateContext()
 * @cuListProp:
 *      cuProps: cu prop list to fill kernelName, devExcl and requestLoad, starting from cuProps[0], no hole
 *      cuNum: request number of cu in this list
 *      sameDevice: request this list of cu from same device
 * @return: int32_t, available cu list num (>= 0) on success or appropriate error number (< 0), if available
 *          cu list number is >= XRM_MAX_AVAILABLE_LIST_NUM, will only return XRM_MAX_AVAILABLE_LIST_NUM.
 **/
int32_t xrmCheckCuListAvailableNum(xrmContext context, xrmCuListProperty* cuListProp);

/**
 * xrmCheckCuGroupAvailableNum() - check the available group number of compute unit resource given a user
 * defined group of kernels's property with cu name (kernelName:instanceName) and request load (1 - 100).
 *
 * @context: the context created through xrmCreateContext()
 * @cuGroupProp:
 *      udfCuGroupName: user defined cu group type name
 *      poolId: id of the cu pool this group CUs come from, the system default pool id is 0
 * @return: int32_t, 0 on success or appropriate error number
 **/
int32_t xrmCheckCuGroupAvailableNum(xrmContext context, xrmCuGroupProperty* cuGroupProp);

/**
 * xrmCheckCuPoolAvailableNum() - check the available cu pool num on the system given
 * a pool of kernels's property with kernel name or alias or both and request
 * load (1 - 100).
 *
 * @context: the context created through xrmCreateContext()
 * @cuPoolProp:
 *      cuListProp: cu list property
 *      cuListNum: number of cu list in this pool
 *      xclbinUuid: uuid of xclbin
 *      xclbinNum: number of xclbin in this pool
 * @return: int32_t, available cu pool num (>= 0) on success or appropriate error number (< 0), if available
 *          cu pool number is >= XRM_MAX_AVAILABLE_POOL_NUM, will only return XRM_MAX_AVAILABLE_POOL_NUM.
 **/
int32_t xrmCheckCuPoolAvailableNum(xrmContext context, xrmCuPoolProperty* cuPoolProp);

/**
 * xrmCuPoolReserve() - reserves a pool of compute unit resource given a pool of
 * kernels's property with kernel name or alias or both and request load (1 - 100).
 *
 * @context: the context created through xrmCreateContext()
 * @cuPoolProp:
 *      cuListProp: cu prop list to fill kernelName, devExcl and requestLoad etc. information
 *      cuListNum: request number of such cu list for this pool
 *      xclbinUuid: request all resource in the xclbin
 *      xclbinNum: request number of such xclbin for this pool
 * @return: uint64_t, reserve pool id (> 0) or 0 on fail
 **/
uint64_t xrmCuPoolReserve(xrmContext context, xrmCuPoolProperty* cuPoolProp);

/**
 * xrmCuPoolRelinquish() - relinquish a previously reserved pool of resources
 *
 * @context: the context created through xrmCreateContext()
 * @poolId: the reserve pool id
 * @return: bool, true on success or false on fail
 **/
bool xrmCuPoolRelinquish(xrmContext context, uint64_t poolId);

/**
 * xrmReservationQuery() - query the compute unit resource given the reservation id.
 *
 * @context: the context created through xrmCreateContext()
 * @poolId: the reserve pool id
 * @cuPoolRes:
 *      cuPoolRes: cu resource pool to fill the allocated cus infor
 *                 starting from cuPoolRes[0], no hole
 *      cuNum: cu number in this pool
 * NOTE: The allocServiceId, channelId and channelLoad are NOT valid in the cuPoolRes
 * @return value: int32_t, 0 on success or appropriate error number
 **/
int32_t xrmReservationQuery(xrmContext context, uint64_t poolId, xrmCuPoolResource* cuPoolRes);

/**
 * xrmExecPluginFunc() - execuate the function of one specified xrm plugin.
 *
 * @context: the context created through xrmCreateContext()
 * @xrmPluginName: name of the xrm plugin
 * @funcId: the function id within xrm plugin
 * @param: the parameter struct for xrm plugin function
 * @return value: int32_t, 0 on success or appropriate error number
 **/
int32_t xrmExecPluginFunc(xrmContext context, char* xrmPluginName, uint32_t funcId, xrmPluginFuncParam* param);

/**
 * Following APIs are NOT stable interface and may be changed in coming future.
 **/

/**
 * xrmCuAllocWithLoad() - allocates compute unit with a device, cu, and channel given a
 * kernel name or alias or both and request load (1 - 100). This function also
 * provides the xclbin and kernel plugin loaded on the device. If required CU is not
 * available, this function will try to load the xclbin to one device and do the
 * allocation again.
 *
 * @context: the context created through xrmCreateContext()
 * @cuProp:
 *      kernelName: the kernel name requested
 *      kernelAlias: the alias of kernel name requested
 *      devExcl: request exclusive device usage for this client
 *      requestLoad: request load (1 - 100)
 *      poolId: poolId will be ignored, this request will only allocate cu from default pool
 * @xclbinFileName: xclbin file (full path and name)
 * @cuRes:
 *      xclbinFileName: xclbin (path and name) attached to this device
 *      kernelPluginFileName: kernel plugin (only name) attached to this device
 *      kernelName: the kernel name of allocated cu
 *      kernelAlias: the name alias of allocated cu
 *      instanceName: the instance name of allocated cu
 *      cuName: the name of allocated cu (kernelName:instanceName)
 *      uuid: uuid of the loaded xclbin file
 *      deviceId: device id of this cu
 *      cuId: cu id of this cu
 *      channelId: channel id of this cu
 *      cuType: type of cu, hardware kernel or soft kernel
 *      allocServiceId: service id for this cu allocation
 *      channelLoad: allocated load of this cu (1 - 100)
 *      poolId: id of the cu pool this cu comes from, this resource should only come from default pool (id is 0)
 * @return: int32_t, 0 on success or appropriate error number
 **/
int32_t xrmCuAllocWithLoad(xrmContext context, xrmCuProperty* cuProp, char* xclbinFileName, xrmCuResource* cuRes);

/**
 * xrmLoadAndAllCuAlloc() - load xclbin to one device, then allocate all CUs from this device.
 *
 * @context: the context created through xrmCreateContext()
 * @xclbinFileName: xclbin file (full path and name)
 * @cuListRes:
 *      cuResources: cu resource list to fill the allocated cus infor, starting from cuResources[0], no hole.
 *      cuNum: allocated cu number in this list
 * @return: int32_t, 0 on success or appropriate error number
 **/
int32_t xrmLoadAndAllCuAlloc(xrmContext context, char* xclbinFileName, xrmCuListResource* cuListRes);

/**
 * xrmCuBlockingAlloc() - blocking function of xrmCuAlloc(), this function will try to do cu allocation
 * until sucess.
 *
 * @context: the context created through xrmCreateContext()
 * @cuProp:
 *      kernelName: the kernel name requested
 *      kernelAlias: the alias of kernel name requested
 *      devExcl: request exclusive device usage for this client
 *      requestLoad: request load (1 - 100)
 *      poolId: request to allocate cu from specified resource pool
 * @interval: the interval time (useconds) before re-trying, To set it as 0 to use XRM default interval
 * @cuRes:
 *      xclbinFileName: xclbin (path and name) attached to this device
 *      kernelPluginFileName: kernel plugin (only name) attached to this device
 *      kernelName: the kernel name of allocated cu
 *      kernelAlias: the name alias of allocated cu
 *      instanceName: the instance name of allocated cu
 *      cuName: the name of allocated cu (kernelName:instanceName)
 *      uuid: uuid of the loaded xclbin file
 *      deviceId: device id of this cu
 *      cuId: cu id of this cu
 *      channelId: channel id of this cu
 *      cuType: type of cu, hardware kernel or soft kernel
 *      allocServiceId: service id for this cu allocation
 *      channelLoad: allocated load of this cu (1 - 100)
 *      poolId: id of the cu pool this cu comes from, the default pool id is 0
 * @return: int32_t, 0 on success or appropriate error number
 **/
int32_t xrmCuBlockingAlloc(xrmContext context, xrmCuProperty* cuProp, uint64_t interval, xrmCuResource* cuRes);

/**
 * xrmCuListAlloc() - blocking function of xrmCuListAlloc(), this function will try to do cu list allocation
 * until sucess.
 *
 * @context: the context created through xrmCreateContext()
 * @cuListProp:
 *      cuProps: cu prop list to fill kernelName, devExcl and requestLoad, starting from cuProps[0], no hole.
 *      cuNum: request number of cu in this list
 *      sameDevice: request this list of cu from same device
 * @interval: the interval time (useconds) before re-trying, To set it as 0 to use XRM default interval
 * @cuListRes:
 *      cuResources: cu resource list to fill the allocated cus infor, starting from cuResources[0], no hole.
 *      cuNum: allocated cu number in this list
 * @return: int32_t, 0 on success or appropriate error number
 **/
int32_t xrmCuListBlockingAlloc(xrmContext context,
                               xrmCuListProperty* cuListProp,
                               uint64_t interval,
                               xrmCuListResource* cuListRes);

/**
 * xrmCuGroupBlockingAlloc() - blocking function of xrmCuGroupAlloc(), this function will try to do cu group
 * allocation until sucess.
 *
 * @context: the context created through xrmCreateContext()
 * @cuGroupProp:
 *      udfCuGroupName: user defined cu group type name
 *      poolId: id of the cu pool this group CUs come from, the system default pool id is 0
 * @interval: the interval time (useconds) before re-trying, To set it as 0 to use XRM default interval
 * @cuGroupRes:
 *      cuResources: cu resource group to fill the allocated cus infor, starting from cuResources[0], no hole.
 *      cuNum: allocated cu number in this list
 * @return: int32_t, 0 on success or appropriate error number
 **/
int32_t xrmCuGroupBlockingAlloc(xrmContext context,
                                xrmCuGroupProperty* cuGroupProp,
                                uint64_t interval,
                                xrmCuGroupResource* cuGroupRes);

#ifdef __cplusplus
}
#endif

#endif // _XRM_H_
