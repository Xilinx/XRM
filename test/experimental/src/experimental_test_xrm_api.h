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

#ifndef _EXPERIMENTAL_TEST_XRM_API_H_
#define _EXPERIMENTAL_TEST_XRM_API_H_

#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <libgen.h>
#include <stdint.h>
#include <uuid/uuid.h>

#include "experimental/xrm_experimental.h"

#ifdef __cplusplus
extern "C" {
#endif

void testXrmFunctions(void);
void xrmCuAllocLeastUsedWithLoadTest(xrmContext* ctx);
void xrmCuAllocLeastUsedFromDevTest(xrmContext* ctx);

#ifdef __cplusplus
}
#endif

#endif // _EXPERIMENTAL_TEST_XRM_API_H_
