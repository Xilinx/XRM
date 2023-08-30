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

#ifndef _EXAMPLE_TEST_XRM_PERF_HPP_
#define _EXAMPLE_TEST_XRM_PERF_HPP_

#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fstream>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdint.h>
#include <xrm.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_CU_NUM 64
#define MAX_TIMES_NUM 1000000
#define MAX_ROUND_NUM 20

using namespace std;

uint64_t xrmCuAllocReleasePerfTest(xrmContext* ctx, unsigned int times);
void testXrmPerformance(string cuNameStr, int cuNum, int times);
uint64_t getTimeCost(struct timeval tvStart, struct timeval tvEnd);

#ifdef __cplusplus
}
#endif

#endif // _EXAMPLE_TEST_XRM_PERF_HPP_
