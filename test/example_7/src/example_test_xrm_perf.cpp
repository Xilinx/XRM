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

#include "example_test_xrm_perf.hpp"

/*
 * This example will show how to test the cu allocation and release performance.
 */

using namespace std;

uint64_t getTimeCost(struct timeval tvStart, struct timeval tvEnd) {
    uint64_t usecondsStart, usecondsEnd, usecondsUsed;
    usecondsStart = tvStart.tv_sec * (uint64_t)1000000 + tvStart.tv_usec;
    usecondsEnd = tvEnd.tv_sec * (uint64_t)1000000 + tvEnd.tv_usec;
    usecondsUsed = usecondsEnd - usecondsStart;
    return (usecondsUsed);
}

uint64_t xrmCuAllocReleasePerfTest(xrmContext* ctx, string cuNameStr, int cuNum, int times) {
    int32_t ret;

    xrmCuProperty cuProp;
    xrmCuResource cuRes[MAX_CU_NUM];

    int32_t testTimes, testCuNum, allocatedCuNum;
    memset(&cuProp, 0, sizeof(xrmCuProperty));
    strcpy(cuProp.kernelName, cuNameStr.c_str());
    strcpy(cuProp.kernelAlias, "");
    cuProp.devExcl = false;
    cuProp.requestLoad = 100;
    cuProp.poolId = 0;

    struct timeval tvStart, tvEnd;
    gettimeofday(&tvStart, NULL);

    for (testTimes = 0; testTimes < times; testTimes++) {
        allocatedCuNum = 0;
        for (testCuNum = 0; testCuNum < cuNum; testCuNum++) {
            ret = xrmCuAlloc(ctx, &cuProp, &cuRes[testCuNum]);
            if (ret != 0) {
                printf("xrmCuAlloc: fail to alloc cu[%d]\n", testCuNum);
                break;
            }
            allocatedCuNum++;
        }
        for (testCuNum = 0; testCuNum < allocatedCuNum; testCuNum++) {
            if (!xrmCuRelease(ctx, &cuRes[testCuNum])) printf("fail to release cu[%d]\n", testCuNum);
        }
    }

    gettimeofday(&tvEnd, NULL);

    uint64_t usecondsUsed = getTimeCost(tvStart, tvEnd);
    printf("Allocate and Release: %d kernel %s %d times\n", cuNum, cuNameStr.c_str(), times);
    printf("Time Cost: %lu useconds (10^-6 seconds)\n\n", usecondsUsed);
    return (usecondsUsed);
}

void xrmCreateContextTest(int times) {
    int32_t testTimes;

    struct timeval tvStart, tvEnd;
    xrmContext* ctx;
    uint64_t usecondsUsed;

    printf("<<<<<<<==  Start the xrm create/destory context performance test ===>>>>>>>>\n\n");
    for (testTimes = 0; testTimes < times; testTimes++) {
        gettimeofday(&tvStart, NULL);
        ctx = (xrmContext*)xrmCreateContext(XRM_API_VERSION_1);
        if (ctx == NULL) {
            printf("Test 0-1: create context failed\n");
            break;
        }
        gettimeofday(&tvEnd, NULL);
        usecondsUsed = getTimeCost(tvStart, tvEnd);
        printf("create context\n");
        printf("Time Cost: %lu useconds (10^-6 seconds)\n\n", usecondsUsed);

        gettimeofday(&tvStart, NULL);
        if (xrmDestroyContext(ctx) != XRM_SUCCESS) {
            printf("Test 0-2: destroy context failed\n");
            break;
        }
        gettimeofday(&tvEnd, NULL);
        usecondsUsed = getTimeCost(tvStart, tvEnd);
        printf("destroy context\n");
        printf("Time Cost: %lu useconds (10^-6 seconds)\n\n", usecondsUsed);
    }
    printf("<<<<<<<==  End the xrm create/destory context performance test ===>>>>>>>>\n\n");
}

void testXrmPerformance(string cuNameStr, int cuNum, int times) {
    printf("<<<<<<<==  Start the xrm performance test ===>>>>>>>>\n\n");
    xrmContext* ctx = (xrmContext*)xrmCreateContext(XRM_API_VERSION_1);
    if (ctx == NULL) {
        printf("Test 1-1: create context failed\n");
        return;
    }
    printf("Test 1-1: create context success\n");

    int32_t round;
    uint64_t totalUsecondsUsed = 0;
    printf("Test 2-1: start performance test: %d cu, %d times, %d round\n\n", cuNum, times, MAX_ROUND_NUM);
    for (round = 0; round < MAX_ROUND_NUM; round++) {
        printf("Round %d :\n", (round + 1));
        totalUsecondsUsed += xrmCuAllocReleasePerfTest(ctx, cuNameStr, cuNum, times);
    }
    uint64_t avarageUsecondsUsed = totalUsecondsUsed / round;
    printf("%d Rounds Avarage Time Cost: %lu useconds (10^-6 seconds)\n\n", round, avarageUsecondsUsed);
    printf("Test 2-2: end performance test\n");

    printf("Test 3-1: destroy context\n");
    if (xrmDestroyContext(ctx) != XRM_SUCCESS)
        printf("Test 3-2: destroy context failed\n");
    else
        printf("Test 3-2: destroy context success\n");

    printf("<<<<<<<==  End the xrm performance test ===>>>>>>>>\n\n");
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        printf("How to run the test:\n");
        printf("./example_test_xrm_perf cuName cuNum times\n");
        return 0;
    }

    string cuNameStr = argv[1];

    int cuNum;
    string cuNumStr = argv[2];
    cuNum = std::stoi(cuNumStr, NULL, 0);
    if (cuNum <= 0 || cuNum > MAX_CU_NUM) {
        printf("invalid cuNum: %d, out of range: 1 - %d\n", cuNum, MAX_CU_NUM);
        return 0;
    }

    int times;
    string timesStr = argv[3];
    times = std::stoi(timesStr, NULL, 0);
    if (times <= 0 || times > MAX_TIMES_NUM) {
        printf("invalid times: %d, out of range: 1 - %d\n", times, MAX_TIMES_NUM);
        return 0;
    }

    xrmCreateContextTest(times);
    testXrmPerformance(cuNameStr, cuNum, times);

    return 0;
}
