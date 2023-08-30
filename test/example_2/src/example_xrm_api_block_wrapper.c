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

#include "example_xrm_api_block_wrapper.h"

int32_t wrapperXrmCuAlloc(xrmContext* context, xrmCuProperty* cuProp, xrmCuResource* cuRes, bool blockAlloc) {
    int32_t ret;
    int64_t i = 0;
    if (blockAlloc) {
        ret = xrmCuAlloc(context, cuProp, cuRes);
        while ((ret != XRM_SUCCESS) && (ret != XRM_ERROR_CONNECT_FAIL)) {
            sleep(XRM_WAIT_SECONDS); // wait 10 seconds
            i++;
            printf("try to alloc cu list again: %lu\n", i);
            ret = xrmCuAlloc(context, cuProp, cuRes);
        }
        return (ret);
    } else {
        ret = xrmCuAlloc(context, cuProp, cuRes);
        return (ret);
    }
}

int32_t wrapperXrmCuListAlloc(xrmContext* context,
                              xrmCuListProperty* cuListProp,
                              xrmCuListResource* cuListRes,
                              bool blockAlloc) {
    int32_t ret;
    int64_t i = 0;
    if (blockAlloc) {
        ret = xrmCuListAlloc(context, cuListProp, cuListRes);
        while ((ret != XRM_SUCCESS) && (ret != XRM_ERROR_CONNECT_FAIL)) {
            sleep(XRM_WAIT_SECONDS); // wait 10 seconds
            i++;
            printf("try to alloc cu list again: %lu\n", i);
            ret = xrmCuListAlloc(context, cuListProp, cuListRes);
        }
        return (ret);
    } else {
        ret = xrmCuListAlloc(context, cuListProp, cuListRes);
        return (ret);
    }
}

void xrmCuBlockAllocReleaseTest(xrmContext* ctx) {
    int32_t ret;
    bool blockAlloc;
    printf("<<<<<<<==  start the xrm block allocation test ===>>>>>>>>\n");
    if (ctx == NULL) {
        printf("ctx is null, fail to alloc cu list\n");
        return;
    }

    // alloc scaler cu
    xrmCuProperty scalerCuProp;
    xrmCuResource scalerCuRes;

    memset(&scalerCuProp, 0, sizeof(xrmCuProperty));
    memset(&scalerCuRes, 0, sizeof(xrmCuResource));
    strcpy(scalerCuProp.kernelName, "v_abrscaler_top");
    strcpy(scalerCuProp.kernelAlias, "");
    scalerCuProp.devExcl = false;
    scalerCuProp.requestLoad = 85;
    scalerCuProp.poolId = 0;
    blockAlloc = true;

    printf("Test 1-1: Using blocking way to alloc scaler cu\n");
    printf("          scaler cu prop: kernelName is %s\n", scalerCuProp.kernelName);
    printf("          scaler cu prop: kernelAlias is %s\n", scalerCuProp.kernelAlias);
    printf("NOTE: The test will be blocked here if requested cu is NOT available on system.\n");
    printf("      You can safely stop the test with Ctrl+c\n");

    ret = wrapperXrmCuAlloc(ctx, &scalerCuProp, &scalerCuRes, blockAlloc);
    if (ret != 0) {
        printf("xrmCuAlloc: fail to alloc scaler cu\n");
    } else {
        printf("Allocated scaler cu: \n");
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

    // alloc enc cu
    xrmCuProperty encCuProp;
    xrmCuResource encCuRes;

    memset(&encCuProp, 0, sizeof(xrmCuProperty));
    memset(&encCuRes, 0, sizeof(xrmCuResource));
    strcpy(encCuProp.kernelName, "krnl_ngcodec_pistachio_enc");
    strcpy(encCuProp.kernelAlias, "");
    encCuProp.devExcl = false;
    encCuProp.requestLoad = 90;
    encCuProp.poolId = 0;

    printf("Test 1-2: Using blocking way to alloc encoder cu\n");
    printf("          Alloc cu, kernelName is %s\n", encCuProp.kernelName);
    printf("NOTE: The test will be blocked here if requested cu is NOT available on system.\n");
    printf("      You can safely stop the test with Ctrl+c\n");

    ret = wrapperXrmCuAlloc(ctx, &encCuProp, &encCuRes, blockAlloc);
    if (ret != 0) {
        printf("xrmCuAlloc: fail to alloc encoder cu\n");
    } else {
        printf("Allocated enc cu: \n");
        printf("   xclbinFileName is:  %s\n", encCuRes.xclbinFileName);
        printf("   kernelPluginFileName is:  %s\n", encCuRes.kernelPluginFileName);
        printf("   kernelName is:  %s\n", encCuRes.kernelName);
        printf("   instanceName is:  %s\n", encCuRes.instanceName);
        printf("   cuName is:  %s\n", encCuRes.cuName);
        printf("   kernelAlias is:  %s\n", encCuRes.kernelAlias);
        printf("   deviceId is:  %d\n", encCuRes.deviceId);
        printf("   cuId is:  %d\n", encCuRes.cuId);
        printf("   channelId is:  %d\n", encCuRes.channelId);
        printf("   cuType is:  %d\n", encCuRes.cuType);
        printf("   baseAddr is:  0x%lx\n", encCuRes.baseAddr);
        printf("   membankId is:  %d\n", encCuRes.membankId);
        printf("   membankType is:  %d\n", encCuRes.membankType);
        printf("   membankSize is:  0x%lx\n", encCuRes.membankSize);
        printf("   membankBaseAddr is:  0x%lx\n", encCuRes.membankBaseAddr);
        printf("   allocServiceId is:  %lu\n", encCuRes.allocServiceId);
        printf("   poolId is:  %lu\n", encCuRes.poolId);
        printf("   channelLoad is:  %d\n", encCuRes.channelLoad);
    }

    printf("<<<<<<<==  end the xrm block allocation test ===>>>>>>>>\n");

    printf("Test 1-3:  release scaler cu\n");
    if (xrmCuRelease(ctx, &scalerCuRes))
        printf("success to  release scaler cu\n");
    else
        printf("fail to release scaler cu\n");

    printf("Test 1-4:   release encoder cu\n");
    if (xrmCuRelease(ctx, &encCuRes))
        printf("success to release encoder cu\n");
    else
        printf("fail to release encoder cu\n");
}

void xrmCuListBlockAllocReleaseTest(xrmContext* ctx) {
    int i;
    int32_t ret;
    bool blockAlloc;
    printf("<<<<<<<==  start the xrm cu list block allocation test ===>>>>>>>>\n");

    printf("Test 2-1: Using blocking way to alloc scaler cu list\n");
    printf("NOTE: The test will be blocked here if requested cu list is NOT available on system.\n");
    printf("      You can safely stop the test with Ctrl+c\n");
    if (ctx == NULL) {
        printf("ctx is null, fail to alloc cu list\n");
        return;
    }
    // alloc scaler cu
    xrmCuListProperty scalerCuListProp;
    xrmCuListResource scalerCuListRes;

    memset(&scalerCuListProp, 0, sizeof(scalerCuListProp));
    memset(&scalerCuListRes, 0, sizeof(scalerCuListRes));

    scalerCuListProp.cuNum = 4;
    for (i = 0; i < scalerCuListProp.cuNum; i++) {
        strcpy(scalerCuListProp.cuProps[i].kernelName, "v_abrscaler_top");
        strcpy(scalerCuListProp.cuProps[i].kernelAlias, "");
        scalerCuListProp.cuProps[i].devExcl = false;
        scalerCuListProp.cuProps[i].requestLoad = 70;
        scalerCuListProp.cuProps[i].poolId = 0;
    }
    blockAlloc = true;

    ret = wrapperXrmCuListAlloc(ctx, &scalerCuListProp, &scalerCuListRes, blockAlloc);
    if (ret != 0) {
        printf("wrapperXrmCuListAlloc: fail to alloc scaler cu list\n");
    } else {
        for (i = 0; i < scalerCuListRes.cuNum; i++) {
            printf("Allocated scaler cu list: cu %d\n", i);
            printf("   xclbinFileName is:  %s\n", scalerCuListRes.cuResources[i].xclbinFileName);
            printf("   kernelPluginFileName is:  %s\n", scalerCuListRes.cuResources[i].kernelPluginFileName);
            printf("   kernelName is:  %s\n", scalerCuListRes.cuResources[i].kernelName);
            printf("   instanceName is:  %s\n", scalerCuListRes.cuResources[i].instanceName);
            printf("   cuName is:  %s\n", scalerCuListRes.cuResources[i].cuName);
            printf("   kernelAlias is:  %s\n", scalerCuListRes.cuResources[i].kernelAlias);
            printf("   deviceId is:  %d\n", scalerCuListRes.cuResources[i].deviceId);
            printf("   cuId is:  %d\n", scalerCuListRes.cuResources[i].cuId);
            printf("   channelId is:  %d\n", scalerCuListRes.cuResources[i].channelId);
            printf("   cuType is:  %d\n", scalerCuListRes.cuResources[i].cuType);
            printf("   baseAddr is:  0x%lx\n", scalerCuListRes.cuResources[i].baseAddr);
            printf("   membankId is:  %d\n", scalerCuListRes.cuResources[i].membankId);
            printf("   membankType is:  %d\n", scalerCuListRes.cuResources[i].membankType);
            printf("   membankSize is:  0x%lx\n", scalerCuListRes.cuResources[i].membankSize);
            printf("   membankBaseAddr is:  0x%lx\n", scalerCuListRes.cuResources[i].membankBaseAddr);
            printf("   allocServiceId is:  %lu\n", scalerCuListRes.cuResources[i].allocServiceId);
            printf("   poolId is:  %lu\n", scalerCuListRes.cuResources[i].poolId);
            printf("   channelLoad is:  %d\n", scalerCuListRes.cuResources[i].channelLoad);
        }
    }

    printf("Test 2-2: Using blocking way to alloc encoder cu list\n");
    printf("NOTE: The test will be blocked here if requested cu list is NOT available on system.\n");
    printf("      You can safely stop the test with Ctrl+c\n");
    // alloc enc cu
    xrmCuListProperty encCuListProp;
    xrmCuListResource encCuListRes;
    memset(&encCuListProp, 0, sizeof(encCuListProp));
    memset(&encCuListRes, 0, sizeof(encCuListRes));
    encCuListProp.cuNum = 4;
    for (i = 0; i < encCuListProp.cuNum; i++) {
        strcpy(encCuListProp.cuProps[i].kernelName, "krnl_ngcodec_pistachio_enc");
        strcpy(encCuListProp.cuProps[i].kernelAlias, "");
        encCuListProp.cuProps[i].devExcl = false;
        encCuListProp.cuProps[i].requestLoad = 70;
        encCuListProp.cuProps[i].poolId = 0;
    }
    blockAlloc = true;

    ret = wrapperXrmCuListAlloc(ctx, &encCuListProp, &encCuListRes, blockAlloc);
    if (ret != 0) {
        printf("xrmCuListAlloc: fail to alloc encoder cu list\n");
    } else {
        for (i = 0; i < encCuListRes.cuNum; i++) {
            printf("Allocated ENCODER cu list: cu %d\n", i);
            printf("   xclbinFileName is:  %s\n", encCuListRes.cuResources[i].xclbinFileName);
            printf("   kernelPluginFileName is:  %s\n", encCuListRes.cuResources[i].kernelPluginFileName);
            printf("   kernelName is:  %s\n", encCuListRes.cuResources[i].kernelName);
            printf("   instanceName is:  %s\n", encCuListRes.cuResources[i].instanceName);
            printf("   cuName is:  %s\n", encCuListRes.cuResources[i].cuName);
            printf("   kernelAlias is:  %s\n", encCuListRes.cuResources[i].kernelAlias);
            printf("   deviceId is:  %d\n", encCuListRes.cuResources[i].deviceId);
            printf("   cuId is:  %d\n", encCuListRes.cuResources[i].cuId);
            printf("   channelId is:  %d\n", encCuListRes.cuResources[i].channelId);
            printf("   cuType is:  %d\n", encCuListRes.cuResources[i].cuType);
            printf("   baseAddr is:  0x%lx\n", encCuListRes.cuResources[i].baseAddr);
            printf("   membankId is:  %d\n", encCuListRes.cuResources[i].membankId);
            printf("   membankType is:  %d\n", encCuListRes.cuResources[i].membankType);
            printf("   membankSize is:  0x%lx\n", encCuListRes.cuResources[i].membankSize);
            printf("   membankBaseAddr is:  0x%lx\n", encCuListRes.cuResources[i].membankBaseAddr);
            printf("   allocServiceId is:  %lu\n", encCuListRes.cuResources[i].allocServiceId);
            printf("   poolId is:  %lu\n", encCuListRes.cuResources[i].poolId);
            printf("   channelLoad is:  %d\n", encCuListRes.cuResources[i].channelLoad);
        }
    }

    printf("Test 2-3:   release scaler cu list\n");
    if (xrmCuListRelease(ctx, &scalerCuListRes))
        printf("success to release scaler cu list\n");
    else
        printf("fail to release scaler cu list\n");

    printf("Test 2-4:  release scaler cu list\n");
    if (xrmCuListRelease(ctx, &encCuListRes))
        printf("success to release encoder cu list\n");
    else
        printf("fail to release encoder cu list\n");
}

void testXrmFunction(void) {
    printf("<<<<<<<==  Start the xrm function test ===>>>>>>>>\n\n");
    xrmContext* ctx = (xrmContext*)xrmCreateContext(XRM_API_VERSION_1);
    if (ctx == NULL) {
        printf("Test 0-1: create context failed\n");
        return;
    }
    printf("Test 0-1: create context success\n");

    xrmCuBlockAllocReleaseTest(ctx);
    xrmCuListBlockAllocReleaseTest(ctx);

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
    testXrmFunction();

    return 0;
}
