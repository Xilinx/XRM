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

#ifndef _EXAMPLE_XRM_API_BLOCK_WRAPPER_H_
#define _EXAMPLE_XRM_API_BLOCK_WRAPPER_H_

#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <libgen.h>
#include <stdbool.h>
#include <xrm.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define XRM_WAIT_SECONDS 10

int32_t wrapperXrmCuAlloc(xrmContext* context, xrmCuProperty* cuProp, xrmCuResource* cuRes, bool blockAlloc);
int32_t wrapperXrmCuListAlloc(xrmContext* context,
                              xrmCuListProperty* cuListProp,
                              xrmCuListResource* cuListRes,
                              bool blockAlloc);
void xrmCuBlockAllocReleaseTest(xrmContext* ctx);
void xrmCuListBlockAllocReleaseTest(xrmContext* ctx);
void testXrmFunction(void);

#ifdef __cplusplus
}
#endif

#endif // _EXAMPLE_XRM_API_BLOCK_WRAPPER_H_
