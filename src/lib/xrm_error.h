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

#ifndef _XRM_ERROR_H_
#define _XRM_ERROR_H_
#include <stdio.h>

/**
 * DOC:
 *  @def @XRM_SUCCESS - Normal return
 *  @def @XRM_ERROR - Unspecified error has occured. Check log files for more info.
 *  @def @XRM_ERROR_INVALID - Invalid or malformed data has been passed to function.
 *  @def @XRM_ERROR_NO_KERNEL - No kernel resource exists or is available.
 *  @def @XRM_ERROR_NO_DEV - No free device.
 *  @def @XRM_ERROR_NO_CHAN - No channels remain to be allocated on the kernel.
 *  @def @XRM_ERROR_CONNECT_FAIL - Connect to xrm daemon fail.
 *  @def @XRM_ERROR_DEVICE_IS_NOT_LOADED - Device is not loaded with xclbin file.
 *  @def @XRM_ERROR_DEVICE_IS_BUSY - Device is busy.
 *  @def @XRM_ERROR_DEVICE_IS_LOCKED - Device is locked.
 **/

#define XRM_SUCCESS (0)

#define XRM_ERROR (-1)
#define XRM_ERROR_INVALID (-2)
#define XRM_ERROR_NO_DEV (-3)
#define XRM_ERROR_NO_KERNEL (-4)
#define XRM_ERROR_NO_CHAN (-5)
#define XRM_ERROR_CONNECT_FAIL (-21)
#define XRM_ERROR_DEVICE_IS_NOT_LOADED (-31)
#define XRM_ERROR_DEVICE_IS_BUSY (-32)
#define XRM_ERROR_DEVICE_IS_LOCKED (-33)

#endif // _XRM_ERROR_H_
