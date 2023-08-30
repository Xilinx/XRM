/*
 * Copyright (C) 2019-2021, Xilinx Inc - All rights reserved
 *
 * Copyright (C) 2023, Advanced Micro Devices, Inc. All rights reserved.
 *
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

#include "experimental_test_xrm_api.h"

/*
 * This example will show how to create context for resouce allocation,
 * and will show how to alloc resource from XRM, how to release the resource.
 * The API for application is defined in xrm.h Please see the header file
 * for parameter description.
 */

void xrmCuAllocLeastUsedWithLoadTest(xrmContext* ctx) {
    int32_t ret;
    printf("<<<<<<<==  start the xrm cu alloc least used with load test ===>>>>>>>>\n");
    if (ctx == NULL) {
        printf("ctx is null, fail to alloc cu list\n");
        return;
    }

    // alloc scaler cu
    xrmCuProperty scalerCuProp;
    xrmCuResource scalerCuRes;

    memset(&scalerCuProp, 0, sizeof(xrmCuProperty));
    memset(&scalerCuRes, 0, sizeof(xrmCuResource));
    strcpy(scalerCuProp.kernelName, "dpdpuv3_wrapper");
    strcpy(scalerCuProp.kernelAlias, "");
    scalerCuProp.devExcl = false;
    scalerCuProp.requestLoad = 25;
    scalerCuProp.poolId = 0;

    printf("Test 1-1: alloc scaler least used\n");
    printf("    scaler cu prop: kernelName is %s\n", scalerCuProp.kernelName);
    printf("    scaler cu prop: kernelAlias is %s\n", scalerCuProp.kernelAlias);

    char xclbinFileName[XRM_MAX_PATH_NAME_LEN];
    strcpy(xclbinFileName, "/tmp/xclbins/test_xrm.xclbin");

    const int niterations = 17;
    int i;
    for (i = 0; i < niterations; i++) {

    printf("Test 1-2: Alloc scaler cu\n");
    ret = xrmCuAllocLeastUsedWithLoad(ctx, &scalerCuProp, xclbinFileName, &scalerCuRes);
    if (ret != 0) {
        printf("xrmCuAllocLeastUsedWithLoad: fail to alloc scaler cu\n");
    } else {
            printf("Allocated scaler cu least used: \n");
            printf("   xclbinFileName is:  %s\n", scalerCuRes.xclbinFileName);
            printf("   kernelPluginFileName is:  %s\n", scalerCuRes.kernelPluginFileName);
            printf("   kernelName is:  %s\n", scalerCuRes.kernelName);
            printf("   instanceName is:  %s\n", scalerCuRes.instanceName);
            printf("   cuName is:  %s\n", scalerCuRes.cuName);
            printf("   kernelAlias is:  %s\n", scalerCuRes.kernelAlias);
            printf("   deviceId is:  %d\n", scalerCuRes.deviceId);
            printf("   cuId is:  %d\n", scalerCuRes.cuId);
            printf("   channelId is:  %d\n", scalerCuRes.channelId);
            printf("   cuType is:  %d\n", scalerCuRes.cuType);
            printf("   baseAddr is:  0x%lx\n", scalerCuRes.baseAddr);
            printf("   membankId is:  %d\n", scalerCuRes.membankId);
            printf("   membankType is:  %d\n", scalerCuRes.membankType);
            printf("   membankSize is:  0x%lx\n", scalerCuRes.membankSize);
            printf("   membankBaseAddr is:  0x%lx\n", scalerCuRes.membankBaseAddr);
            printf("   allocServiceId is:  %lu\n", scalerCuRes.allocServiceId);
            printf("   poolId is:  %lu\n", scalerCuRes.poolId);
            printf("   channelLoad is:  %d\n", scalerCuRes.channelLoad);
        }
    }

    printf("Test 1-3:  release scaler cu\n");
    if (xrmCuRelease(ctx, &scalerCuRes))
        printf("success to  release scaler cu\n");
    else
        printf("fail to release scaler cu\n");

    printf("<<<<<<<==  end the xrm allocation least used test ===>>>>>>>>\n");
}

void xrmCuAllocLeastUsedFromDevTest(xrmContext* ctx) {
    int32_t ret;
    printf("<<<<<<<==  start the xrm cu alloc least used from device ===>>>>>>>>\n");
    if (ctx == NULL) {
        printf("ctx is null, fail to alloc cu list\n");
        return;
    }

    // alloc scaler cu
    xrmCuProperty scalerCuProp;
    xrmCuResource scalerCuRes;

    memset(&scalerCuProp, 0, sizeof(xrmCuProperty));
    memset(&scalerCuRes, 0, sizeof(xrmCuResource));
    strcpy(scalerCuProp.kernelName, "dpdpuv3_wrapper");
    strcpy(scalerCuProp.kernelAlias, "");
    scalerCuProp.devExcl = false;
    scalerCuProp.requestLoad = 25;
    scalerCuProp.poolId = 0;
    int32_t deviceId=0; 
    printf("Test 2-1: alloc scaler least used\n");
    printf("    scaler cu prop: kernelName is %s\n", scalerCuProp.kernelName);
    printf("    scaler cu prop: kernelAlias is %s\n", scalerCuProp.kernelAlias);

    const int niterations = 17;
    int i;
    for (i = 0; i < niterations; i++) {

    printf("Test 2-2: Alloc scaler cu\n");
    ret = xrmCuAllocLeastUsedFromDev(ctx,deviceId, &scalerCuProp, &scalerCuRes);
    if (ret != 0) {
        printf("xrmCuAllocLeastUsedFromDev: fail to alloc scaler cu\n");
    } else {
            printf("Allocated scaler cu least used: \n");
            printf("   xclbinFileName is:  %s\n", scalerCuRes.xclbinFileName);
            printf("   kernelPluginFileName is:  %s\n", scalerCuRes.kernelPluginFileName);
            printf("   kernelName is:  %s\n", scalerCuRes.kernelName);
            printf("   instanceName is:  %s\n", scalerCuRes.instanceName);
            printf("   cuName is:  %s\n", scalerCuRes.cuName);
            printf("   kernelAlias is:  %s\n", scalerCuRes.kernelAlias);
            printf("   deviceId is:  %d\n", scalerCuRes.deviceId);
            printf("   cuId is:  %d\n", scalerCuRes.cuId);
            printf("   channelId is:  %d\n", scalerCuRes.channelId);
            printf("   cuType is:  %d\n", scalerCuRes.cuType);
            printf("   baseAddr is:  0x%lx\n", scalerCuRes.baseAddr);
            printf("   membankId is:  %d\n", scalerCuRes.membankId);
            printf("   membankType is:  %d\n", scalerCuRes.membankType);
            printf("   membankSize is:  0x%lx\n", scalerCuRes.membankSize);
            printf("   membankBaseAddr is:  0x%lx\n", scalerCuRes.membankBaseAddr);
            printf("   allocServiceId is:  %lu\n", scalerCuRes.allocServiceId);
            printf("   poolId is:  %lu\n", scalerCuRes.poolId);
            printf("   channelLoad is:  %d\n", scalerCuRes.channelLoad);
        }
    }

    printf("Test 2-3:  release scaler cu\n");
    if (xrmCuRelease(ctx, &scalerCuRes))
        printf("success to  release scaler cu\n");
    else
        printf("fail to release scaler cu\n");

    printf("<<<<<<<==  end the xrm allocation least used test ===>>>>>>>>\n");
}
void testXrmFunctions(void) {
    printf("<<<<<<<==  Start the xrm function test ===>>>>>>>>\n\n");
    xrmContext* ctx = (xrmContext*)xrmCreateContext(XRM_API_VERSION_1);
    if (ctx == NULL) {
        printf("Test 0-1: create context failed\n");
        return;
    }
    printf("Test 0-1: create context success\n");

    xrmCuAllocLeastUsedWithLoadTest(ctx);
    xrmCuAllocLeastUsedFromDevTest(ctx);
    
    printf("Test 0-2: destroy context\n");
    if (xrmDestroyContext(ctx) != XRM_SUCCESS)
        printf("Test 0-2: destroy context failed\n");
    else
        printf("Test 0-2: destroy context success\n");

    printf("<<<<<<<==  End the xrm function test ===>>>>>>>>\n\n");
}

int main(int argc, char* argv[]) {
    /*
     * This example will show how to create context for resouce allocation,
     * and will show how to alloc resource from XRM, how to release the resource.
     * The API for application is defined in xrm.h Please see the header file
     * for parameter description.
     */
    testXrmFunctions();

    return 0;
}
