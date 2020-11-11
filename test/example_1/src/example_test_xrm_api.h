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

#ifndef _EXAMPLE_TEST_XRM_API_H_
#define _EXAMPLE_TEST_XRM_API_H_

#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <libgen.h>
#include <stdint.h>
#include <uuid/uuid.h>
#include <xrm.h>

#ifdef __cplusplus
extern "C" {
#endif

void xrmConcurrentContextTest(int32_t numContext);
void xrmCuAllocReleaseTest(xrmContext* ctx);
void xrmCuListAllocReleaseTest(xrmContext* ctx);
void xrmSoftCuAllocReleaseTest(xrmContext* ctx);
void xrmCuListAllocReleaseFromSameDevTest(xrmContext* ctx);
void xrmCuAllocReleaseUsingAliasTest(xrmContext* ctx);
void xrmCuAllocReleaseUsingKernelNameAndAliasTest(xrmContext* ctx);
void xrmCuAllocQueryReleaseUsingAliasTest(xrmContext* ctx);
void xrmCheckCuListAvailableNumUsingAliasTest(xrmContext* ctx);
void xrmCuPoolReserveAllocReleaseRelinquishTest(xrmContext* ctx);
void xrmLoadUnloadXclbinTest(xrmContext* ctx);
void xrmCheckDaemonTest(xrmContext* ctx);
void xrmPluginTest(xrmContext* ctx);
void xrmUdfCuGroupTest(xrmContext* ctx);
void xrmTestHexstrToBin(const char* inStr, int32_t insz, char* out);
void testXrmFunctions(void);
void xrmCuAllocWithLoadTest(xrmContext* ctx);
void xrmLoadAndAllCuAllocTest(xrmContext* ctx);
void xrmCuBlockingAllocReleaseTest(xrmContext* ctx);
void xrmCuListBlockingAllocReleaseTest(xrmContext* ctx);
void xrmCuGroupBlockingAllocReleaseTest(xrmContext* ctx);

#ifdef __cplusplus
}
#endif

#endif // _EXAMPLE_TEST_XRM_API_H_
