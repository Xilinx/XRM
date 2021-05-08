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

/**
 * @file xrm_experimental.h
 * @brief Header for Experimental APIs of XRM.
 * Please note these APIs may change or be removed without warning.
 */

#ifndef _XRM_EXPERIMENTAL_H_
#define _XRM_EXPERIMENTAL_H_

#include "xrm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Allocates compute unit with a device, cu, and channel given a
 * kernel name or alias or both and request load. This function also
 * provides the xclbin and kernel plugin loaded on the device. If required CU is not
 * available, this function will try to load the xclbin to one device and do the
 * allocation again. If there are no free CUs remaining, the CU with the
 * minimum load which supports the requested capacity will be acquired.
 * Additionally, it strictly enforces that the allocated CU comes
 * from a device that was loaded with the user specified xclbinFileName.
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
int32_t xrmCuAllocLeastUsedWithLoad(xrmContext context,
                                    xrmCuProperty* cuProp,
                                    char* xclbinFileName,
                                    xrmCuResource* cuRes);
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
int32_t xrmCuAllocLeastUsedFromDev(xrmContext context, int32_t deviceId, xrmCuProperty* cuProp, xrmCuResource* cuRes);

#ifdef __cplusplus
}
#endif

#endif // _XRM_EXPERIMENTAL_H_
