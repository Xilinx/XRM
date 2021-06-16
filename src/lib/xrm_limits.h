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

#ifndef _XRM_LIMITS_H_
#define _XRM_LIMITS_H_

#define XRM_MAX_DDR_MAP 64
#define XRM_MAX_XILINX_DEVICES 16
#define XRM_MAX_XILINX_KERNELS 144
#define XRM_MAX_KERNEL_CHANNELS 1000
#define XRM_MAX_KERNEL_RESERVES 1000
#define XRM_MAX_NAME_LEN 256
#define XRM_MAX_PATH_NAME_LEN 256
#define XRM_MAX_PLUGIN_NAME XRM_MAX_NAME_LEN
#define XRM_MAX_KERNEL_NAME XRM_MAX_NAME_LEN
#define XRM_MAX_DSA_NAME XRM_MAX_NAME_LEN
#define XRM_MAX_CU_LOAD_GRANULARITY_100 100           // percentage (granularity 100): 0 - 100
#define XRM_MAX_CHAN_LOAD_GRANULARITY_100 100         // percentage (granularity 100): 0 - 100
#define XRM_MAX_CU_LOAD_GRANULARITY_1000000 1000000   // granularity 1,000,000: 0 - 1,000,000
#define XRM_MAX_CHAN_LOAD_GRANULARITY_1000000 1000000 // granularity 1,000,000: 0 - 1,000,000
#define XRM_MAX_LIST_CU_NUM 16
#define XRM_MAX_UDF_CU_GROUP_NUM 32
#define XRM_MAX_UDF_CU_GROUP_OPTION_LIST_NUM 8
#define XRM_MAX_GROUP_CU_NUM 16
#define XRM_MAX_POOL_CU_NUM 128
#define XRM_MAX_DEV_CLIENTS (XRM_MAX_XILINX_KERNELS * 8)
#define XRM_MAX_REGS_PER_IP 1
#define XRM_MAX_CONNECTION_ENTRIES (XRM_MAX_DDR_MAP * XRM_MAX_XILINX_KERNELS * XRM_MAX_REGS_PER_IP)
#define XRM_MAX_PLUGIN_NUM 32
#define XRM_MAX_PLUGIN_FUNC_ID 7 // 0 - 7
#define XRM_MAX_PLUGIN_FUNC_PARAM_LEN 16384

#define XRM_DEFAULT_INTERVAL_US 100000 // default interval (useconds), 100 ms

#define XRM_MIN_LOG_LEVEL XRM_LOG_EMERGENCY // min log level
#define XRM_MAX_LOG_LEVEL XRM_LOG_DEBUG     // max log level
#define XRM_DEFAULT_LOG_LEVEL XRM_LOG_ERROR // default log level

#define XRM_MAX_LIMIT_CONCURRENT_CLIENT 1000000   // max limit concurrent client
#define XRM_DEFAULT_LIMIT_CONCURRENT_CLIENT 40000 // default limit concurrent client

#endif // _XRM_LIMITS_H_
