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

#include "example_test_xrm_api.h"

/*
 * This example will show how to create context for resouce allocation,
 * and will show how to alloc resource from XRM, how to release the resource.
 * The API for application is defined in xrm.h Please see the header file
 * for parameter description.
 */

void xrmCuAllocReleaseTest(xrmContext* ctx) {
    int32_t ret;
    printf("<<<<<<<==  start the xrm allocation test ===>>>>>>>>\n");
    if (ctx == NULL) {
        printf("ctx is null, fail to do cu alloc test\n");
        return;
    }

    // alloc scaler cu
    xrmCuProperty scalerCuProp;
    xrmCuResource scalerCuRes;
    xrmCuStat scalerCuStat;
    uint64_t maxCapacity;

    memset(&scalerCuProp, 0, sizeof(xrmCuProperty));
    memset(&scalerCuRes, 0, sizeof(xrmCuResource));
    memset(&scalerCuStat, 0, sizeof(xrmCuStat));
    strcpy(scalerCuProp.kernelName, "scaler");
    strcpy(scalerCuProp.kernelAlias, "");
    scalerCuProp.devExcl = false;
    scalerCuProp.requestLoad = 45;
    scalerCuProp.poolId = 0;

    printf("Test 1-1: check scaler cu available number\n");
    printf("    scaler cu prop: kernelName is %s\n", scalerCuProp.kernelName);
    printf("    scaler cu prop: kernelAlias is %s\n", scalerCuProp.kernelAlias);

    ret = xrmCheckCuAvailableNum(ctx, &scalerCuProp);
    if (ret < 0) {
        printf("xrmCheckCuAvailableNum: fail to check scaler cu available number\n");
    } else {
        printf("xrmCheckCuAvailableNum: scaler cu available number = %d\n", ret);
    }

    printf("Test 1-2: Alloc scaler cu\n");
    ret = xrmCuAlloc(ctx, &scalerCuProp, &scalerCuRes);
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

    maxCapacity = xrmCuGetMaxCapacity(ctx, &scalerCuProp);
    printf("Test 1-3: Get Max Capacity: scaler cu max capacity is: %lu\n", maxCapacity);

    printf("Test 1-4: Check SCALER cu status: \n");
    ret = xrmCuCheckStatus(ctx, &scalerCuRes, &scalerCuStat);
    if (ret != 0) {
        printf("xrmCuCheckStatus: fail to check status of scaler cu\n");
    } else {
        if (scalerCuStat.isBusy)
            printf("   isBusy:  true\n");
        else
            printf("   isBusy:  false\n");
        printf("   usedLoad:  %d\n", scalerCuStat.usedLoad);
    }

    // alloc encoder cu
    xrmCuProperty encCuProp;
    xrmCuResource encCuRes;

    memset(&encCuProp, 0, sizeof(xrmCuProperty));
    memset(&encCuRes, 0, sizeof(xrmCuResource));
    strcpy(encCuProp.kernelName, "encoder");
    strcpy(encCuProp.kernelAlias, "");
    encCuProp.devExcl = false;
    encCuProp.requestLoad = 55;
    encCuProp.poolId = 0;

    maxCapacity = xrmCuGetMaxCapacity(ctx, &encCuProp);
    printf("Test 1-5: Get Max Capacity: encoder cu max capacity is: %lu\n", maxCapacity);

    printf("Test 1-6: Check encoder cu available number ...\n");
    printf("    kernelName is %s\n", encCuProp.kernelName);

    ret = xrmCheckCuAvailableNum(ctx, &encCuProp);
    if (ret < 0) {
        printf("xrmCheckCuAvailableNum: fail to check encoder cu available number\n");
    } else {
        printf("xrmCheckCuAvailableNum: encoder cu available number = %d\n", ret);
    }

    printf("Test 1-7: Alloc encoder cu\n");
    ret = xrmCuAlloc(ctx, &encCuProp, &encCuRes);
    if (ret != 0) {
        printf("xrmCuAlloc: fail to alloc encoder cu\n");
    } else {
        printf("Allocated encoder cu: \n");
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

    printf("<<<<<<<==  end the xrm allocation test ===>>>>>>>>\n");

    printf("Test 1-8:  release scaler cu\n");
    if (xrmCuRelease(ctx, &scalerCuRes))
        printf("success to  release scaler cu\n");
    else
        printf("fail to release scaler cu\n");

    printf("Test 1-9:   release encoder cu\n");
    if (xrmCuRelease(ctx, &encCuRes))
        printf("success to release encoder cu\n");
    else
        printf("fail to release encoder cu\n");
}

void xrmCuListAllocReleaseTest(xrmContext* ctx) {
    int i;
    int32_t ret;
    printf("<<<<<<<==  start the xrm cu list allocation test ===>>>>>>>>\n");

    printf("Test 2-1: alloc scaler cu list\n");
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
        strcpy(scalerCuListProp.cuProps[i].kernelName, "scaler");
        strcpy(scalerCuListProp.cuProps[i].kernelAlias, "");
        scalerCuListProp.cuProps[i].devExcl = false;
        scalerCuListProp.cuProps[i].requestLoad = 45;
        scalerCuListProp.cuProps[i].poolId = 0;
    }

    ret = xrmCuListAlloc(ctx, &scalerCuListProp, &scalerCuListRes);
    if (ret != 0) {
        printf("xrmCuListAlloc: fail to alloc scaler cu list\n");
    } else {
        for (i = 0; i < scalerCuListRes.cuNum; i++) {
            printf("Allocated scaler cu list: cu %d\n", i);
            printf("   xclbinFileName is:  %s\n", scalerCuListRes.cuResources[i].xclbinFileName);
            printf("   kernelPluginFileName is:  %s\n", scalerCuListRes.cuResources[i].kernelPluginFileName);
            printf("   kernelName is:  %s\n", scalerCuListRes.cuResources[i].kernelName);
            printf("   kernelAlias is:  %s\n", scalerCuListRes.cuResources[i].kernelAlias);
            printf("   instanceName is:  %s\n", scalerCuListRes.cuResources[i].instanceName);
            printf("   cuName is:  %s\n", scalerCuListRes.cuResources[i].cuName);
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

    printf("Test 2-2:  alloc encoder cu list\n");
    // alloc enc cu
    xrmCuListProperty encCuListProp;
    xrmCuListResource encCuListRes;
    memset(&encCuListProp, 0, sizeof(encCuListProp));
    memset(&encCuListRes, 0, sizeof(encCuListRes));
    encCuListProp.cuNum = 4;
    for (i = 0; i < encCuListProp.cuNum; i++) {
        strcpy(encCuListProp.cuProps[i].kernelName, "encoder");
        strcpy(encCuListProp.cuProps[i].kernelAlias, "");
        encCuListProp.cuProps[i].devExcl = false;
        encCuListProp.cuProps[i].requestLoad = 55;
        encCuListProp.cuProps[i].poolId = 0;
    }

    ret = xrmCuListAlloc(ctx, &encCuListProp, &encCuListRes);
    if (ret != 0) {
        printf("xrmCuListAlloc: fail to alloc encoder cu list\n");
    } else {
        for (i = 0; i < encCuListRes.cuNum; i++) {
            printf("Allocated ENCODER cu list: cu %d\n", i);
            printf("   xclbinFileName is:  %s\n", encCuListRes.cuResources[i].xclbinFileName);
            printf("   kernelPluginFileName is:  %s\n", encCuListRes.cuResources[i].kernelPluginFileName);
            printf("   kernelName is:  %s\n", encCuListRes.cuResources[i].kernelName);
            printf("   kernelAlias is:  %s\n", encCuListRes.cuResources[i].kernelAlias);
            printf("   instanceName is:  %s\n", encCuListRes.cuResources[i].instanceName);
            printf("   cuName is:  %s\n", encCuListRes.cuResources[i].cuName);
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

void xrmSoftCuAllocReleaseTest(xrmContext* ctx) {
    printf("<<<<<<<==  start the xrm soft cu allocation test ===>>>>>>>>\n");

    // alloc scaler cu
    xrmCuProperty scalerCuProp;
    xrmCuResource scalerCuRes;
    xrmCuStat scalerCuStat;
    uint64_t maxCapacity;

    memset(&scalerCuProp, 0, sizeof(xrmCuProperty));
    memset(&scalerCuRes, 0, sizeof(xrmCuResource));
    memset(&scalerCuStat, 0, sizeof(xrmCuStat));
    strcpy(scalerCuProp.kernelName, "scaler");
    strcpy(scalerCuProp.kernelAlias, "");
    scalerCuProp.devExcl = false;
    scalerCuProp.requestLoad = 45;
    scalerCuProp.poolId = 0;

    printf("Test 3-1: Alloc scaler cu, kernelName is %s\n", scalerCuProp.kernelName);
    printf("Test 3-1: Alloc scaler cu, kernelAlias is %s\n", scalerCuProp.kernelAlias);

    int32_t ret = xrmCuAlloc(ctx, &scalerCuProp, &scalerCuRes);
    if (ret != 0) {
        printf("Allocated scaler cu failed.\n");
    } else {
        printf("Allocated scaler cu: \n");
        printf("   xclbinFileName is:  %s\n", scalerCuRes.xclbinFileName);
        printf("   kernelPluginFileName is:  %s\n", scalerCuRes.kernelPluginFileName);
        printf("   kernelName is:  %s\n", scalerCuRes.kernelName);
        printf("   kernelAlias is:  %s\n", scalerCuRes.kernelAlias);
        printf("   instanceName is:  %s\n", scalerCuRes.instanceName);
        printf("   cuName is:  %s\n", scalerCuRes.cuName);
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

    maxCapacity = xrmCuGetMaxCapacity(ctx, &scalerCuProp);
    printf("Test 3-2: Get Max Capacity: scaler cu max capacity is: %lu\n", maxCapacity);

    printf("Test 3-3: Check SCALER cu status: \n");
    ret = (xrmCuCheckStatus(ctx, &scalerCuRes, &scalerCuStat));
    if (ret != 0) {
        printf("xrmCuCheckStatus: fail to check status of scaler cu\n");
    } else {
        if (scalerCuStat.isBusy)
            printf("   isBusy:  true\n");
        else
            printf("   isBusy:  false\n");
        printf("   usedLoad:  %d\n", scalerCuStat.usedLoad);
    }

    printf("<<<<<<<==  end the xrm soft cu allocation test ===>>>>>>>>\n");

    printf("Test 3-4:  release scaler cu\n");
    if (xrmCuRelease(ctx, &scalerCuRes))
        printf("success to release scaler cu\n");
    else
        printf("fail to release scaler cu\n");
}

void xrmCuListAllocReleaseFromSameDevTest(xrmContext* ctx) {
    int i;
    int32_t ret;
    printf("<<<<<<<==  start the xrm cu list from same dev allocation test ===>>>>>>>>\n");

    printf("Test 4-1: alloc scaler cu list\n");
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
    scalerCuListProp.sameDevice = true;
    for (i = 0; i < scalerCuListProp.cuNum; i++) {
        strcpy(scalerCuListProp.cuProps[i].kernelName, "scaler");
        strcpy(scalerCuListProp.cuProps[i].kernelAlias, "");
        scalerCuListProp.cuProps[i].devExcl = false;
        scalerCuListProp.cuProps[i].requestLoad = 20;
        scalerCuListProp.cuProps[i].poolId = 0;
    }

    ret = xrmCuListAlloc(ctx, &scalerCuListProp, &scalerCuListRes);
    if (ret != 0) {
        printf("xrmCuListAlloc: fail to alloc scaler cu list\n");
    } else {
        for (i = 0; i < scalerCuListRes.cuNum; i++) {
            printf("Allocated scaler cu list: cu %d\n", i);
            printf("   xclbinFileName is:  %s\n", scalerCuListRes.cuResources[i].xclbinFileName);
            printf("   kernelPluginFileName is:  %s\n", scalerCuListRes.cuResources[i].kernelPluginFileName);
            printf("   kernelName is:  %s\n", scalerCuListRes.cuResources[i].kernelName);
            printf("   kernelAlias is:  %s\n", scalerCuListRes.cuResources[i].kernelAlias);
            printf("   instanceName is:  %s\n", scalerCuListRes.cuResources[i].instanceName);
            printf("   cuName is:  %s\n", scalerCuListRes.cuResources[i].cuName);
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

    printf("Test 4-2:  alloc encoder cu list\n");
    // alloc enc cu
    xrmCuListProperty encCuListProp;
    xrmCuListResource encCuListRes;
    memset(&encCuListProp, 0, sizeof(encCuListProp));
    memset(&encCuListRes, 0, sizeof(encCuListRes));
    encCuListProp.cuNum = 4;
    encCuListProp.sameDevice = true;
    for (i = 0; i < encCuListProp.cuNum; i++) {
        strcpy(encCuListProp.cuProps[i].kernelName, "encoder");
        strcpy(encCuListProp.cuProps[i].kernelAlias, "");
        encCuListProp.cuProps[i].devExcl = false;
        encCuListProp.cuProps[i].requestLoad = 30;
        encCuListProp.cuProps[i].poolId = 0;
    }

    ret = xrmCuListAlloc(ctx, &encCuListProp, &encCuListRes);
    if (ret != 0) {
        printf("xrmCuListAlloc: fail to alloc encoder cu list\n");
    } else {
        for (i = 0; i < encCuListRes.cuNum; i++) {
            printf("Allocated ENCODER cu list: cu %d\n", i);
            printf("   xclbinFileName is:  %s\n", encCuListRes.cuResources[i].xclbinFileName);
            printf("   kernelPluginFileName is:  %s\n", encCuListRes.cuResources[i].kernelPluginFileName);
            printf("   kernelName is:  %s\n", encCuListRes.cuResources[i].kernelName);
            printf("   kernelAlias is:  %s\n", encCuListRes.cuResources[i].kernelAlias);
            printf("   instanceName is:  %s\n", encCuListRes.cuResources[i].instanceName);
            printf("   cuName is:  %s\n", encCuListRes.cuResources[i].cuName);
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

    printf("Test 4-3:   release scaler cu list\n");
    if (xrmCuListRelease(ctx, &scalerCuListRes))
        printf("success to release scaler cu list\n");
    else
        printf("fail to release scaler cu list\n");

    printf("Test 4-4:  release scaler cu list\n");
    if (xrmCuListRelease(ctx, &encCuListRes))
        printf("success to release encoder cu list\n");
    else
        printf("fail to release encoder cu list\n");
}

void xrmCuAllocReleaseUsingAliasTest(xrmContext* ctx) {
    printf("<<<<<<<==  start the xrm allocation with kernel alias test ===>>>>>>>>\n");

    // alloc scaler cu
    xrmCuProperty scalerCuProp;
    xrmCuResource scalerCuRes;
    xrmCuStat scalerCuStat;
    uint64_t maxCapacity;

    memset(&scalerCuProp, 0, sizeof(xrmCuProperty));
    memset(&scalerCuRes, 0, sizeof(xrmCuResource));
    memset(&scalerCuStat, 0, sizeof(xrmCuStat));
    strcpy(scalerCuProp.kernelName, "");
    strcpy(scalerCuProp.kernelAlias, "SCALER_MPSOC");
    scalerCuProp.devExcl = false;
    scalerCuProp.requestLoad = 45;
    scalerCuProp.poolId = 0;

    printf("Test 5-1: Alloc scaler cu, kernelName is %s\n", scalerCuProp.kernelName);
    printf("Test 5-1: Alloc scaler cu, kernelAlias is %s\n", scalerCuProp.kernelAlias);

    int32_t ret = xrmCuAlloc(ctx, &scalerCuProp, &scalerCuRes);
    if (ret != 0) {
        printf("Allocated scaler cu failed.\n");
    } else {
        printf("Allocated scaler cu: \n");
        printf("   xclbinFileName is:  %s\n", scalerCuRes.xclbinFileName);
        printf("   kernelPluginFileName is:  %s\n", scalerCuRes.kernelPluginFileName);
        printf("   kernelName is:  %s\n", scalerCuRes.kernelName);
        printf("   kernelAlias is:  %s\n", scalerCuRes.kernelAlias);
        printf("   instanceName is:  %s\n", scalerCuRes.instanceName);
        printf("   cuName is:  %s\n", scalerCuRes.cuName);
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

    maxCapacity = xrmCuGetMaxCapacity(ctx, &scalerCuProp);
    printf("Test 5-2: Get Max Capacity: scaler cu max capacity is: %lu\n", maxCapacity);

    printf("Test 5-3: Check SCALER cu status: \n");
    ret = (xrmCuCheckStatus(ctx, &scalerCuRes, &scalerCuStat));
    if (ret != 0) {
        printf("xrmCuCheckStatus: fail to check status of scaler cu\n");
    } else {
        if (scalerCuStat.isBusy)
            printf("   isBusy:  true\n");
        else
            printf("   isBusy:  false\n");
        printf("   usedLoad:  %d\n", scalerCuStat.usedLoad);
    }

    printf("<<<<<<<==  end the xrm allocation with kernel alias test ===>>>>>>>>\n");

    printf("Test 5-4:  release scaler cu\n");
    if (xrmCuRelease(ctx, &scalerCuRes))
        printf("success to release scaler cu\n");
    else
        printf("fail to release scaler cu\n");
}

void xrmCuAllocReleaseUsingKernelNameAndAliasTest(xrmContext* ctx) {
    printf("<<<<<<<==  start the xrm allocation with kernel name and alias test ===>>>>>>>>\n");

    // alloc scaler cu
    xrmCuProperty scalerCuProp;
    xrmCuResource scalerCuRes;
    xrmCuStat scalerCuStat;
    uint64_t maxCapacity;

    memset(&scalerCuProp, 0, sizeof(xrmCuProperty));
    memset(&scalerCuRes, 0, sizeof(xrmCuResource));
    memset(&scalerCuStat, 0, sizeof(xrmCuStat));
    strcpy(scalerCuProp.kernelName, "scaler");
    strcpy(scalerCuProp.kernelAlias, "SCALER_MPSOC");
    scalerCuProp.devExcl = false;
    scalerCuProp.requestLoad = 45;
    scalerCuProp.poolId = 0;

    printf("Test 6-1: Alloc scaler cu, kernelName is %s\n", scalerCuProp.kernelName);
    printf("Test 6-1: Alloc scaler cu, kernelAlias is %s\n", scalerCuProp.kernelAlias);

    int32_t ret = xrmCuAlloc(ctx, &scalerCuProp, &scalerCuRes);
    if (ret != 0) {
        printf("Allocated scaler cu failed.\n");
    } else {
        printf("Allocated scaler cu: \n");
        printf("   xclbinFileName is:  %s\n", scalerCuRes.xclbinFileName);
        printf("   kernelPluginFileName is:  %s\n", scalerCuRes.kernelPluginFileName);
        printf("   kernelName is:  %s\n", scalerCuRes.kernelName);
        printf("   kernelAlias is:  %s\n", scalerCuRes.kernelAlias);
        printf("   instanceName is:  %s\n", scalerCuRes.instanceName);
        printf("   cuName is:  %s\n", scalerCuRes.cuName);
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

    maxCapacity = xrmCuGetMaxCapacity(ctx, &scalerCuProp);
    printf("Test 6-2: Get Max Capacity: scaler cu max capacity is: %lu\n", maxCapacity);

    printf("Test 6-3: Check SCALER cu status: \n");
    ret = (xrmCuCheckStatus(ctx, &scalerCuRes, &scalerCuStat));
    if (ret != 0) {
        printf("xrmCuCheckStatus: fail to check status of scaler cu\n");
    } else {
        if (scalerCuStat.isBusy)
            printf("   isBusy:  true\n");
        else
            printf("   isBusy:  false\n");
        printf("   usedLoad:  %d\n", scalerCuStat.usedLoad);
    }

    printf("<<<<<<<==  end the xrm allocation with kernel name and alias test ===>>>>>>>>\n");

    printf("Test 6-4:  release scaler cu\n");
    if (xrmCuRelease(ctx, &scalerCuRes))
        printf("success to release scaler cu\n");
    else
        printf("fail to release scaler cu\n");
}

void xrmCuAllocQueryReleaseUsingAliasTest(xrmContext* ctx) {
    int i, ret;

    printf("<<<<<<<==  start the xrm allocation and query with kernel alias test ===>>>>>>>>\n");

    printf("Test 7-1: alloc scaler cu list\n");
    if (ctx == NULL) {
        printf("ctx is null, fail to alloc cu list\n");
        return;
    }

    xrmCuListProperty scalerCuListProp;
    xrmCuListResource scalerCuListRes;

    memset(&scalerCuListProp, 0, sizeof(scalerCuListProp));
    memset(&scalerCuListRes, 0, sizeof(scalerCuListRes));

    scalerCuListProp.cuNum = 4;
    scalerCuListProp.sameDevice = false;
    for (i = 0; i < scalerCuListProp.cuNum; i++) {
        strcpy(scalerCuListProp.cuProps[i].kernelName, "scaler");
        strcpy(scalerCuListProp.cuProps[i].kernelAlias, "");
        scalerCuListProp.cuProps[i].devExcl = false;
        scalerCuListProp.cuProps[i].requestLoad = 45;
        scalerCuListProp.cuProps[i].poolId = 0;
    }

    ret = xrmCuListAlloc(ctx, &scalerCuListProp, &scalerCuListRes);
    if (ret != 0) {
        printf("xrmCuListAlloc: fail to alloc scaler cu list\n");
    } else {
        for (i = 0; i < scalerCuListRes.cuNum; i++) {
            printf("Allocated scaler cu list: cu %d\n", i);
            printf("   xclbinFileName is:  %s\n", scalerCuListRes.cuResources[i].xclbinFileName);
            printf("   kernelPluginFileName is:  %s\n", scalerCuListRes.cuResources[i].kernelPluginFileName);
            printf("   kernelName is:  %s\n", scalerCuListRes.cuResources[i].kernelName);
            printf("   kernelAlias is:  %s\n", scalerCuListRes.cuResources[i].kernelAlias);
            printf("   instanceName is:  %s\n", scalerCuListRes.cuResources[i].instanceName);
            printf("   cuName is:  %s\n", scalerCuListRes.cuResources[i].cuName);
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

    printf("Test 7-2:  query scaler cu list with allocServiceId\n");
    xrmAllocationQueryInfo allocQuery;
    xrmCuListResource queryScalerCuListRes;

    memset(&allocQuery, 0, sizeof(xrmAllocationQueryInfo));
    memset(&queryScalerCuListRes, 0, sizeof(xrmAllocationQueryInfo));
    allocQuery.allocServiceId = scalerCuListRes.cuResources[0].allocServiceId;
    ret = xrmAllocationQuery(ctx, &allocQuery, &queryScalerCuListRes);
    if (ret != 0) {
        printf("xrmAllocationQuery: fail to query allocated scaler cu list with allocServiceId\n");
    } else {
        for (i = 0; i < queryScalerCuListRes.cuNum; i++) {
            printf("Query Allocated scaler cu list: cu %d\n", i);
            printf("   xclbinFileName is:  %s\n", queryScalerCuListRes.cuResources[i].xclbinFileName);
            printf("   kernelPluginFileName is:  %s\n", queryScalerCuListRes.cuResources[i].kernelPluginFileName);
            printf("   kernelName is:  %s\n", queryScalerCuListRes.cuResources[i].kernelName);
            printf("   kernelAlias is:  %s\n", queryScalerCuListRes.cuResources[i].kernelAlias);
            printf("   instanceName is:  %s\n", queryScalerCuListRes.cuResources[i].instanceName);
            printf("   cuName is:  %s\n", queryScalerCuListRes.cuResources[i].cuName);
            printf("   deviceId is:  %d\n", queryScalerCuListRes.cuResources[i].deviceId);
            printf("   cuId is:  %d\n", queryScalerCuListRes.cuResources[i].cuId);
            printf("   channelId is:  %d\n", queryScalerCuListRes.cuResources[i].channelId);
            printf("   cuType is:  %d\n", queryScalerCuListRes.cuResources[i].cuType);
            printf("   baseAddr is:  0x%lx\n", queryScalerCuListRes.cuResources[i].baseAddr);
            printf("   membankId is:  %d\n", queryScalerCuListRes.cuResources[i].membankId);
            printf("   membankType is:  %d\n", queryScalerCuListRes.cuResources[i].membankType);
            printf("   membankSize is:  0x%lx\n", queryScalerCuListRes.cuResources[i].membankSize);
            printf("   membankBaseAddr is:  0x%lx\n", queryScalerCuListRes.cuResources[i].membankBaseAddr);
            printf("   allocServiceId is:  %lu\n", queryScalerCuListRes.cuResources[i].allocServiceId);
            printf("   poolId is:  %lu\n", queryScalerCuListRes.cuResources[i].poolId);
            printf("   channelLoad is:  %d\n", queryScalerCuListRes.cuResources[i].channelLoad);
        }
    }

    printf("Test 7-3:  query scaler cu list with allocServiceId and kernel alias\n");
    memset(&allocQuery, 0, sizeof(xrmAllocationQueryInfo));
    memset(&queryScalerCuListRes, 0, sizeof(xrmCuListResource));
    allocQuery.allocServiceId = scalerCuListRes.cuResources[0].allocServiceId;
    strcpy(allocQuery.kernelName, "");
    strcpy(allocQuery.kernelAlias, "SCALER_MPSOC");
    ret = xrmAllocationQuery(ctx, &allocQuery, &queryScalerCuListRes);
    if (ret != 0) {
        printf("xrmAllocationQuery: fail to query allocated scaler cu list with allocServiceId and kernel alias\n");
    } else {
        for (i = 0; i < queryScalerCuListRes.cuNum; i++) {
            printf("Query Allocated scaler cu list: cu %d\n", i);
            printf("   xclbinFileName is:  %s\n", queryScalerCuListRes.cuResources[i].xclbinFileName);
            printf("   kernelPluginFileName is:  %s\n", queryScalerCuListRes.cuResources[i].kernelPluginFileName);
            printf("   kernelName is:  %s\n", queryScalerCuListRes.cuResources[i].kernelName);
            printf("   kernelAlias is:  %s\n", queryScalerCuListRes.cuResources[i].kernelAlias);
            printf("   instanceName is:  %s\n", queryScalerCuListRes.cuResources[i].instanceName);
            printf("   cuName is:  %s\n", queryScalerCuListRes.cuResources[i].cuName);
            printf("   deviceId is:  %d\n", queryScalerCuListRes.cuResources[i].deviceId);
            printf("   cuId is:  %d\n", queryScalerCuListRes.cuResources[i].cuId);
            printf("   channelId is:  %d\n", queryScalerCuListRes.cuResources[i].channelId);
            printf("   cuType is:  %d\n", queryScalerCuListRes.cuResources[i].cuType);
            printf("   baseAddr is:  0x%lx\n", queryScalerCuListRes.cuResources[i].baseAddr);
            printf("   membankId is:  %d\n", queryScalerCuListRes.cuResources[i].membankId);
            printf("   membankType is:  %d\n", queryScalerCuListRes.cuResources[i].membankType);
            printf("   membankSize is:  0x%lx\n", queryScalerCuListRes.cuResources[i].membankSize);
            printf("   membankBaseAddr is:  0x%lx\n", queryScalerCuListRes.cuResources[i].membankBaseAddr);
            printf("   allocServiceId is:  %lu\n", queryScalerCuListRes.cuResources[i].allocServiceId);
            printf("   poolId is:  %lu\n", queryScalerCuListRes.cuResources[i].poolId);
            printf("   channelLoad is:  %d\n", queryScalerCuListRes.cuResources[i].channelLoad);
        }
    }

    printf("Test 7-4:  query scaler cu list with allocServiceId and kernel name\n");
    memset(&allocQuery, 0, sizeof(xrmAllocationQuery));
    memset(&queryScalerCuListRes, 0, sizeof(xrmCuListResource));
    allocQuery.allocServiceId = scalerCuListRes.cuResources[0].allocServiceId;
    strcpy(allocQuery.kernelName, "scaler");
    strcpy(allocQuery.kernelAlias, "");
    ret = xrmAllocationQuery(ctx, &allocQuery, &queryScalerCuListRes);
    if (ret != 0) {
        printf("xrmAllocationQuery: fail to query allocated scaler cu list with allocServiceId and kernel name\n");
    } else {
        for (i = 0; i < queryScalerCuListRes.cuNum; i++) {
            printf("Query Allocated scaler cu list: cu %d\n", i);
            printf("   xclbinFileName is:  %s\n", queryScalerCuListRes.cuResources[i].xclbinFileName);
            printf("   kernelPluginFileName is:  %s\n", queryScalerCuListRes.cuResources[i].kernelPluginFileName);
            printf("   kernelName is:  %s\n", queryScalerCuListRes.cuResources[i].kernelName);
            printf("   kernelAlias is:  %s\n", queryScalerCuListRes.cuResources[i].kernelAlias);
            printf("   instanceName is:  %s\n", queryScalerCuListRes.cuResources[i].instanceName);
            printf("   cuName is:  %s\n", queryScalerCuListRes.cuResources[i].cuName);
            printf("   deviceId is:  %d\n", queryScalerCuListRes.cuResources[i].deviceId);
            printf("   cuId is:  %d\n", queryScalerCuListRes.cuResources[i].cuId);
            printf("   channelId is:  %d\n", queryScalerCuListRes.cuResources[i].channelId);
            printf("   cuType is:  %d\n", queryScalerCuListRes.cuResources[i].cuType);
            printf("   baseAddr is:  0x%lx\n", queryScalerCuListRes.cuResources[i].baseAddr);
            printf("   membankId is:  %d\n", queryScalerCuListRes.cuResources[i].membankId);
            printf("   membankType is:  %d\n", queryScalerCuListRes.cuResources[i].membankType);
            printf("   membankSize is:  0x%lx\n", queryScalerCuListRes.cuResources[i].membankSize);
            printf("   membankBaseAddr is:  0x%lx\n", queryScalerCuListRes.cuResources[i].membankBaseAddr);
            printf("   allocServiceId is:  %lu\n", queryScalerCuListRes.cuResources[i].allocServiceId);
            printf("   poolId is:  %lu\n", queryScalerCuListRes.cuResources[i].poolId);
            printf("   channelLoad is:  %d\n", queryScalerCuListRes.cuResources[i].channelLoad);
        }
    }

    printf("<<<<<<<==  end the xrm allocation query with kernel alias test ===>>>>>>>>\n");

    printf("Test 7-5:  release scaler cu\n");
    if (xrmCuListRelease(ctx, &scalerCuListRes))
        printf("success to release scaler cu list\n");
    else
        printf("fail to release scaler cu list\n");
}

void xrmCheckCuListAvailableNumUsingAliasTest(xrmContext* ctx) {
    int i, ret;

    printf("<<<<<<<==  start the xrm check cu list available num with kernel alias test ===>>>>>>>>\n");

    printf("Test 8-1: check scaler cu list available num with kernel alias\n");
    if (ctx == NULL) {
        printf("ctx is null, fail to check cu list available num with kernel alias\n");
        return;
    }

    xrmCuListProperty scalerCuListProp;

    memset(&scalerCuListProp, 0, sizeof(scalerCuListProp));

    scalerCuListProp.cuNum = 4;
    scalerCuListProp.sameDevice = false;
    for (i = 0; i < scalerCuListProp.cuNum; i++) {
        strcpy(scalerCuListProp.cuProps[i].kernelName, "");
        strcpy(scalerCuListProp.cuProps[i].kernelAlias, "SCALER_MPSOC");
        scalerCuListProp.cuProps[i].devExcl = false;
        scalerCuListProp.cuProps[i].requestLoad = 45;
        scalerCuListProp.cuProps[i].poolId = 0;
    }

    ret = xrmCheckCuListAvailableNum(ctx, &scalerCuListProp);
    if (ret < 0) {
        printf("xrmCheckCuListAvailableNum: fail to check scaler cu list available num with kernel alias\n");
    } else {
        printf("xrmCheckCuListAvailableNum: scaler cu list available num with kernel alias = %d\n", ret);
    }

    printf("Test 8-2: check scaler cu list available num with kernel name\n");
    memset(&scalerCuListProp, 0, sizeof(scalerCuListProp));

    scalerCuListProp.cuNum = 4;
    scalerCuListProp.sameDevice = false;
    for (i = 0; i < scalerCuListProp.cuNum; i++) {
        strcpy(scalerCuListProp.cuProps[i].kernelName, "scaler");
        strcpy(scalerCuListProp.cuProps[i].kernelAlias, "");
        scalerCuListProp.cuProps[i].devExcl = false;
        scalerCuListProp.cuProps[i].requestLoad = 45;
        scalerCuListProp.cuProps[i].poolId = 0;
    }

    ret = xrmCheckCuListAvailableNum(ctx, &scalerCuListProp);
    if (ret < 0) {
        printf("xrmCheckCuListAvailableNum: fail to check scaler cu list available num with kernel name\n");
    } else {
        printf("xrmCheckCuListAvailableNum: scaler cu list available num with kernel name = %d\n", ret);
    }

    printf("Test 8-3: check scaler cu list available num with kernel name\n");
    memset(&scalerCuListProp, 0, sizeof(scalerCuListProp));

    scalerCuListProp.cuNum = 7;
    scalerCuListProp.sameDevice = false;
    for (i = 0; i < scalerCuListProp.cuNum; i++) {
        strcpy(scalerCuListProp.cuProps[i].kernelName, "scaler");
        strcpy(scalerCuListProp.cuProps[i].kernelAlias, "");
        scalerCuListProp.cuProps[i].devExcl = false;
        scalerCuListProp.cuProps[i].requestLoad = 65;
        scalerCuListProp.cuProps[i].poolId = 0;
    }

    ret = xrmCheckCuListAvailableNum(ctx, &scalerCuListProp);
    if (ret < 0) {
        printf("xrmCheckCuListAvailableNum: fail to check scaler cu list available num with kernel name\n");
    } else {
        printf("xrmCheckCuListAvailableNum: scaler cu list available num with kernel name = %d\n", ret);
    }
    printf("<<<<<<<==  end the xrm check cu list available num with kernel alias test ===>>>>>>>>\n");
}

/**
 * xrmTestHexstrToBin() - conver hex string like "0123456789abcdef" to bin 0123456789abcdef
 * @inStr: input hex string
 * @inze: size of input
 * @out: output binary array
 * @return: void
 **/
void xrmTestHexstrToBin(const char* inStr, int32_t insz, char* out) {
    char in[insz];
    char* pout = out;
    char hex_0, hex_1;
    strncpy(in, inStr, insz);
    for (int32_t i = 0; i < insz; i += 2, pout++) {
        if (in[i] < 'a')
            hex_0 = (in[i] - '0');
        else
            hex_0 = (in[i] - 'a') + 10;
        if (in[i + 1] < 'a')
            hex_1 = (in[i + 1] - '0');
        else
            hex_1 = (in[i + 1] - 'a') + 10;
        *pout = ((hex_0 << 4) & 0xF0) | (hex_1 & 0xF);
    }
}

void xrmCuPoolReserveAllocReleaseRelinquishTest(xrmContext* ctx) {
    int i;
    int32_t ret;
    uint64_t scalerReservePoolId;
    printf("<<<<<<<==  start the xrm cu reserve alloc release relinquish test ===>>>>>>>>\n");

    printf("Test 9-1: reserve scaler cu pool\n");
    if (ctx == NULL) {
        printf("ctx is null, fail to reserve cu pool\n");
        return;
    }
    // reserve scaler cu
    xrmCuPoolProperty scalerCuPoolProp;

    memset(&scalerCuPoolProp, 0, sizeof(scalerCuPoolProp));

    scalerCuPoolProp.cuListProp.cuNum = 4;
    for (i = 0; i < scalerCuPoolProp.cuListProp.cuNum; i++) {
        strcpy(scalerCuPoolProp.cuListProp.cuProps[i].kernelName, "scaler");
        strcpy(scalerCuPoolProp.cuListProp.cuProps[i].kernelAlias, "");
        scalerCuPoolProp.cuListProp.cuProps[i].devExcl = false;
        scalerCuPoolProp.cuListProp.cuProps[i].requestLoad = 50;
    }
    scalerCuPoolProp.cuListNum = 1;
    char uuidStr[64];
    strcpy(uuidStr, "6b7b13fbff2649048d744307dd711466");
    xrmTestHexstrToBin(uuidStr, 2 * sizeof(uuid_t), (char*)&scalerCuPoolProp.xclbinUuid);
    scalerCuPoolProp.xclbinNum = 1;

    ret = xrmCheckCuPoolAvailableNum(ctx, &scalerCuPoolProp);
    if (ret < 0) {
        printf("xrmCheckCuPoolAvailableNum: fail to check scaler cu pool available num\n");
    } else {
        printf("xrmCheckCuPoolAvailableNum: scaler cu pool available num = %d\n", ret);
    }

    scalerReservePoolId = xrmCuPoolReserve(ctx, &scalerCuPoolProp);
    if (scalerReservePoolId == 0) {
        printf("xrmCuPoolReserve: fail to reserve scaler cu pool\n");
    } else {
        printf("xrmCuPoolReserve: reservePoolId = %lu\n", scalerReservePoolId);
    }

    // query the reserve result
    xrmCuPoolResource scalerCuPoolRes;
    memset(&scalerCuPoolRes, 0, sizeof(scalerCuPoolRes));

    printf("Test 9-2: xrmReservationQuery\n");
    ret = xrmReservationQuery(ctx, scalerReservePoolId, &scalerCuPoolRes);
    if (ret != 0) {
        printf("xrmReservationQuery: fail to query reserved scaler cu pool\n");
    } else {
        for (i = 0; i < scalerCuPoolRes.cuNum; i++) {
            printf("query the reserved scaler cu pool: cu %d\n", i);
            printf("   xclbinFileName is:  %s\n", scalerCuPoolRes.cuResources[i].xclbinFileName);
            printf("   kernelPluginFileName is:  %s\n", scalerCuPoolRes.cuResources[i].kernelPluginFileName);
            printf("   kernelName is:  %s\n", scalerCuPoolRes.cuResources[i].kernelName);
            printf("   kernelAlias is:  %s\n", scalerCuPoolRes.cuResources[i].kernelAlias);
            printf("   instanceName is:  %s\n", scalerCuPoolRes.cuResources[i].instanceName);
            printf("   cuName is:  %s\n", scalerCuPoolRes.cuResources[i].cuName);
            printf("   deviceId is:  %d\n", scalerCuPoolRes.cuResources[i].deviceId);
            printf("   cuId is:  %d\n", scalerCuPoolRes.cuResources[i].cuId);
            printf("   cuType is:  %d\n", scalerCuPoolRes.cuResources[i].cuType);
            printf("   baseAddr is:  0x%lx\n", scalerCuPoolRes.cuResources[i].baseAddr);
            printf("   membankId is:  %d\n", scalerCuPoolRes.cuResources[i].membankId);
            printf("   membankType is:  %d\n", scalerCuPoolRes.cuResources[i].membankType);
            printf("   membankSize is:  0x%lx\n", scalerCuPoolRes.cuResources[i].membankSize);
            printf("   membankBaseAddr is:  0x%lx\n", scalerCuPoolRes.cuResources[i].membankBaseAddr);
            printf("   poolId is:  %lu\n", scalerCuPoolRes.cuResources[i].poolId);
        }
    }

    // alloc scaler cu
    xrmCuProperty scalerCuProp;
    xrmCuResource scalerCuRes;
    memset(&scalerCuProp, 0, sizeof(xrmCuProperty));
    memset(&scalerCuRes, 0, sizeof(xrmCuResource));
    strcpy(scalerCuProp.kernelName, "scaler");
    strcpy(scalerCuProp.kernelAlias, "");
    scalerCuProp.devExcl = false;
    scalerCuProp.requestLoad = 45;
    scalerCuProp.poolId = scalerReservePoolId;

    printf("Test 9-3: scaler cu prop: kernelName is %s\n", scalerCuProp.kernelName);
    printf("          scaler cu prop: kernelAlias is %s\n", scalerCuProp.kernelAlias);

    printf("Test 9-4: xrmCheckCuAvailableNum in reserve pool\n");
    ret = xrmCheckCuAvailableNum(ctx, &scalerCuProp);
    if (ret < 0) {
        printf("xrmCheckCuAvailableNum: fail to check scaler cu available number\n");
    } else {
        printf("xrmCheckCuAvailableNum: scaler cu available number = %d\n", ret);
    }

    printf("Test 9-5: xrmCuAlloc in reserve pool\n");
    ret = xrmCuAlloc(ctx, &scalerCuProp, &scalerCuRes);
    if (ret != 0) {
        printf("xrmCuAlloc: fail to alloc scaler cu from reservation\n");
    } else {
        printf("Allocated scaler cu from reserve pool: \n");
        printf("   xclbinFileName is:  %s\n", scalerCuRes.xclbinFileName);
        printf("   kernelPluginFileName is:  %s\n", scalerCuRes.kernelPluginFileName);
        printf("   kernelName is:  %s\n", scalerCuRes.kernelName);
        printf("   kernelAlias is:  %s\n", scalerCuRes.kernelAlias);
        printf("   instanceName is:  %s\n", scalerCuRes.instanceName);
        printf("   cuName is:  %s\n", scalerCuRes.cuName);
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

    // alloc scaler cu list
    xrmCuListProperty scalerCuListProp;
    xrmCuListResource scalerCuListRes;
    memset(&scalerCuListProp, 0, sizeof(scalerCuListProp));
    memset(&scalerCuListRes, 0, sizeof(scalerCuListRes));

    scalerCuListProp.cuNum = 4;
    for (i = 0; i < scalerCuListProp.cuNum; i++) {
        strcpy(scalerCuListProp.cuProps[i].kernelName, "scaler");
        strcpy(scalerCuListProp.cuProps[i].kernelAlias, "");
        scalerCuListProp.cuProps[i].devExcl = false;
        scalerCuListProp.cuProps[i].requestLoad = 40;
        scalerCuListProp.cuProps[i].poolId = scalerReservePoolId;
    }

    printf("Test 9-6: xrmCheckCuListAvailableNum in reserve pool\n");
    ret = xrmCheckCuListAvailableNum(ctx, &scalerCuListProp);
    if (ret < 0) {
        printf("xrmCheckCuListAvailableNum: fail to check scaler cu list available number\n");
    } else {
        printf("xrmCheckCuListAvailableNum: scaler cu list available number = %d\n", ret);
    }

    printf("Test 9-7: xrmCuListAlloc in reserve pool\n");
    ret = xrmCuListAlloc(ctx, &scalerCuListProp, &scalerCuListRes);
    if (ret != 0) {
        printf("xrmCuListAlloc: fail to alloc scaler cu list from reservation\n");
    } else {
        for (i = 0; i < scalerCuListRes.cuNum; i++) {
            printf("Allocated scaler cu list from reserve pool: cu %d\n", i);
            printf("   xclbinFileName is:  %s\n", scalerCuListRes.cuResources[i].xclbinFileName);
            printf("   kernelPluginFileName is:  %s\n", scalerCuListRes.cuResources[i].kernelPluginFileName);
            printf("   kernelName is:  %s\n", scalerCuListRes.cuResources[i].kernelName);
            printf("   kernelAlias is:  %s\n", scalerCuListRes.cuResources[i].kernelAlias);
            printf("   instanceName is:  %s\n", scalerCuListRes.cuResources[i].instanceName);
            printf("   cuName is:  %s\n", scalerCuListRes.cuResources[i].cuName);
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

    printf("Test 9-8:   release scaler cu\n");
    if (xrmCuRelease(ctx, &scalerCuRes))
        printf("success to release scaler cu\n");
    else
        printf("fail to release scaler cu\n");

    printf("Test 9-9:   release scaler cu list\n");
    if (xrmCuListRelease(ctx, &scalerCuListRes))
        printf("success to release scaler cu list\n");
    else
        printf("fail to release scaler cu list\n");

    printf("Test 9-10:  relinquish scaler cu pool\n");
    if (xrmCuPoolRelinquish(ctx, scalerReservePoolId))
        printf("success to relinquish scaler cu pool\n");
    else
        printf("fail to relinquish encoder cu pool\n");
}

void xrmLoadUnloadXclbinTest(xrmContext* ctx) {
    int32_t ret;
    printf("<<<<<<<==  start the xrm load xclbin test ===>>>>>>>>\n");
    if (ctx == NULL) {
        printf("ctx is null, fail to alloc cu list\n");
        return;
    }

    char xclbinFileName[XRM_MAX_PATH_NAME_LEN];
    int32_t devId;
    strcpy(xclbinFileName, "/tmp/xclbins/test_xrm_u30.xclbin");

    devId = 0;
    printf("Test 10-1: unload xclbin\n");
    printf("  devId is %d\n", devId);
    ret = xrmUnloadOneDevice(ctx, devId);
    if (ret == 0)
        printf("success to unload xclbin from device %d\n", devId);
    else
        printf("fail to unload xclbin from device %d\n", devId);

    devId = 0;
    printf("Test 10-2: load xclbin file to specified device\n");
    printf("  xclbin file name is %s\n", xclbinFileName);
    printf("  devId is %d\n", devId);
    ret = xrmLoadOneDevice(ctx, devId, xclbinFileName);
    if (ret >= 0)
        printf("success to load xclbin to device %d\n", ret);
    else
        printf("fail to load xclbin to device %d\n", devId);

    devId = -1;
    printf("Test 10-3: load xclbin file to any device\n");
    printf("  xclbin file name is %s\n", xclbinFileName);
    printf("  devId is %d\n", devId);
    ret = xrmLoadOneDevice(ctx, devId, xclbinFileName);
    if (ret >= 0)
        printf("success to load xclbin to device %d\n", ret);
    else
        printf("fail to load xclbin to device %d\n", devId);

    devId = 1;
    printf("Test 10-4: disable device\n");
    printf("  devId is %d\n", devId);
    ret = xrmDisableOneDevice(ctx, devId);
    if (ret == 0)
        printf("success to disable device %d\n", devId);
    else
        printf("fail to disable device %d\n", devId);

    printf("Test 10-5: load xclbin file to specified device\n");
    printf("  xclbin file name is %s\n", xclbinFileName);
    printf("  devId is %d\n", devId);
    ret = xrmLoadOneDevice(ctx, devId, xclbinFileName);
    if (ret >= 0)
        printf("success to load xclbin to device %d\n", ret);
    else
        printf("fail to load xclbin to device %d\n", devId);

    printf("Test 10-6: enable device\n");
    printf("  devId is %d\n", devId);
    ret = xrmEnableOneDevice(ctx, devId);
    if (ret == 0)
        printf("success to enable device %d\n", devId);
    else
        printf("fail to enable device %d\n", devId);

    printf("Test 10-7: load xclbin file to specified device\n");
    printf("  xclbin file name is %s\n", xclbinFileName);
    printf("  devId is %d\n", devId);
    ret = xrmLoadOneDevice(ctx, devId, xclbinFileName);
    if (ret >= 0)
        printf("success to load xclbin to device %d\n", ret);
    else
        printf("fail to load xclbin to device %d\n", devId);
}

void xrmCheckDaemonTest(xrmContext* ctx) {
    bool ret;
    printf("<<<<<<<==  start the xrm check daemon test ===>>>>>>>>\n");
    if (ctx == NULL) {
        printf("ctx is null, fail to run check daemon test\n");
        return;
    }

    printf("Test 11-1: check whether daemon is running\n");
    ret = xrmIsDaemonRunning(ctx);
    if (ret == true)
        printf("XRM daemon is running\n");
    else
        printf("XRM daemon is NOT running\n");

    printf("<<<<<<<==  end the xrm check daemon test ===>>>>>>>>\n");
}

void xrmPluginTest(xrmContext* ctx) {
    printf("<<<<<<<==  start the xrm plugin test ===>>>>>>>>\n");
    if (ctx == NULL) {
        printf("ctx is null, fail to run plugin test\n");
        return;
    }

    printf("Test 12-1: test the plugin function\n");
    char xrmPluginName[XRM_MAX_NAME_LEN];
    strcpy(xrmPluginName, "xrmPluginExample");
    xrmPluginFuncParam* param = (xrmPluginFuncParam*)malloc(sizeof(xrmPluginFuncParam));
    for (int32_t funcId = 0; funcId <= 7; funcId++) {
        memset(param, 0, sizeof(xrmPluginFuncParam));
        strcpy(param->input, "xrmPluginExampleFunc_");
        param->input[21] = funcId + '0';
        if (xrmExecPluginFunc(ctx, xrmPluginName, funcId, param) != XRM_SUCCESS)
            printf("test plugin function %d, fail to run the function\n", funcId);
        else
            printf("test plugin function %d, success to run the function, input: %s, output: %s\n", funcId,
                   param->input, param->output);
    }
    free(param);

    printf("<<<<<<<==  end the xrm plugin test ===>>>>>>>>\n");
}

void xrmUdfCuGroupTest(xrmContext* ctx) {
    int32_t i, ret;
    printf("<<<<<<<==  start the xrm user defined cu group test ===>>>>>>>>\n");
    if (ctx == NULL) {
        printf("ctx is null, fail to run user defined cu group test\n");
        return;
    }

    printf("Test 13-1: test the user defined cu group declaration function\n");
    char udfCuGroupName[XRM_MAX_NAME_LEN];
    xrmUdfCuGroupProperty* udfCuGroupProp = (xrmUdfCuGroupProperty*)malloc(sizeof(xrmUdfCuGroupProperty));

    /* declare user defined cu group from same device */
    printf("Test 13-2: user defined cu group from same device declaration\n");
    memset(udfCuGroupProp, 0, sizeof(xrmUdfCuGroupProperty));
    strcpy(udfCuGroupName, "udfCuGroupSameDevice");
    xrmUdfCuListProperty* udfCuListProp;
    xrmUdfCuProperty* udfCuProp;
    udfCuGroupProp->optionUdfCuListNum = 2;
    /* define the first option cu list */
    udfCuListProp = &udfCuGroupProp->optionUdfCuListProps[0];
    udfCuListProp->cuNum = 5;
    udfCuListProp->sameDevice = true;
    for (int32_t cuIdx = 0; cuIdx < udfCuListProp->cuNum; cuIdx++) {
        udfCuProp = &udfCuListProp->udfCuProps[cuIdx];
        strcpy(udfCuProp->cuName, "encoder:encoder_1");
        udfCuProp->devExcl = false;
        udfCuProp->requestLoad = 30;
    }
    /* define the second option cu list */
    udfCuListProp = &udfCuGroupProp->optionUdfCuListProps[1];
    udfCuListProp->cuNum = 5;
    udfCuListProp->sameDevice = true;
    for (int32_t cuIdx = 0; cuIdx < udfCuListProp->cuNum; cuIdx++) {
        udfCuProp = &udfCuListProp->udfCuProps[cuIdx];
        strcpy(udfCuProp->cuName, "decoder:decoder_1");
        udfCuProp->devExcl = false;
        udfCuProp->requestLoad = 20;
    }
    ret = xrmUdfCuGroupDeclare(ctx, udfCuGroupProp, udfCuGroupName);
    if (ret == XRM_SUCCESS)
        printf("xrmUdfCuGroupDeclare(): user defined cu group from same device declaration success\n");
    else
        printf("xrmUdfCuGroupDeclare(): user defined cu group from same device declaration fail\n");

    /* declare user defined cu group from any device */
    printf("Test 13-3: user defined cu group from any device declaration\n");
    memset(udfCuGroupProp, 0, sizeof(xrmUdfCuGroupProperty));
    strcpy(udfCuGroupName, "udfCuGroupAnyDevice");
    udfCuGroupProp->optionUdfCuListNum = 2;
    udfCuListProp = &udfCuGroupProp->optionUdfCuListProps[0];
    udfCuListProp->cuNum = 5;
    udfCuListProp->sameDevice = false;
    for (int32_t cuIdx = 0; cuIdx < udfCuListProp->cuNum; cuIdx++) {
        udfCuProp = &udfCuListProp->udfCuProps[cuIdx];
        strcpy(udfCuProp->cuName, "encoder:encoder_1");
        udfCuProp->devExcl = false;
        udfCuProp->requestLoad = 30;
    }
    udfCuListProp = &udfCuGroupProp->optionUdfCuListProps[1];
    udfCuListProp->cuNum = 5;
    udfCuListProp->sameDevice = false;
    for (int32_t cuIdx = 0; cuIdx < udfCuListProp->cuNum; cuIdx++) {
        udfCuProp = &udfCuListProp->udfCuProps[cuIdx];
        strcpy(udfCuProp->cuName, "decoder:decoder_1");
        udfCuProp->devExcl = false;
        udfCuProp->requestLoad = 20;
    }
    ret = xrmUdfCuGroupDeclare(ctx, udfCuGroupProp, udfCuGroupName);
    if (ret == XRM_SUCCESS)
        printf("xrmUdfCuGroupDeclare(): user defined cu group from any device declaration success\n");
    else
        printf("xrmUdfCuGroupDeclare(): user defined cu group from any device declaration fail\n");

    printf("Test 13-4: check user defined cu group (same device) available num\n");
    xrmCuGroupResource cuGroupRes;
    xrmCuGroupProperty cuGroupProp;
    memset(&cuGroupProp, 0, sizeof(xrmCuGroupProperty));
    strcpy(cuGroupProp.udfCuGroupName, "udfCuGroupSameDevice");
    cuGroupProp.poolId = 0;
    ret = xrmCheckCuGroupAvailableNum(ctx, &cuGroupProp);
    if (ret < 0) {
        printf("xrmCheckCuGroupAvaiableNum: fail to check user defined cu group (same device) available num\n");
    } else {
        printf("xrmCheckCuGroupAvaiableNum: user defined cu group (same device) available num = %d\n", ret);
    }

    printf("Test 13-5: alloc user defined cu group (same device)\n");
    memset(&cuGroupRes, 0, sizeof(xrmCuGroupResource));
    ret = xrmCuGroupAlloc(ctx, &cuGroupProp, &cuGroupRes);
    if (ret != XRM_SUCCESS) {
        printf("xrmCuGroupAlloc: fail to alloc user defined cu group (same device)\n");
    } else {
        for (i = 0; i < cuGroupRes.cuNum; i++) {
            printf("Allocated user defined cu group (same device): cu %d\n", i);
            printf("   xclbinFileName is:  %s\n", cuGroupRes.cuResources[i].xclbinFileName);
            printf("   kernelPluginFileName is:  %s\n", cuGroupRes.cuResources[i].kernelPluginFileName);
            printf("   kernelName is:  %s\n", cuGroupRes.cuResources[i].kernelName);
            printf("   kernelAlias is:  %s\n", cuGroupRes.cuResources[i].kernelAlias);
            printf("   instanceName is:  %s\n", cuGroupRes.cuResources[i].instanceName);
            printf("   cuName is:  %s\n", cuGroupRes.cuResources[i].cuName);
            printf("   deviceId is:  %d\n", cuGroupRes.cuResources[i].deviceId);
            printf("   cuId is:  %d\n", cuGroupRes.cuResources[i].cuId);
            printf("   channelId is:  %d\n", cuGroupRes.cuResources[i].channelId);
            printf("   cuType is:  %d\n", cuGroupRes.cuResources[i].cuType);
            printf("   baseAddr is:  0x%lx\n", cuGroupRes.cuResources[i].baseAddr);
            printf("   membankId is:  %d\n", cuGroupRes.cuResources[i].membankId);
            printf("   membankType is:  %d\n", cuGroupRes.cuResources[i].membankType);
            printf("   membankSize is:  0x%lx\n", cuGroupRes.cuResources[i].membankSize);
            printf("   membankBaseAddr is:  0x%lx\n", cuGroupRes.cuResources[i].membankBaseAddr);
            printf("   allocServiceId is:  %lu\n", cuGroupRes.cuResources[i].allocServiceId);
            printf("   poolId is:  %lu\n", cuGroupRes.cuResources[i].poolId);
            printf("   channelLoad is:  %d\n", cuGroupRes.cuResources[i].channelLoad);
        }
    }

    printf("Test 13-6: release user defined cu group (same device)\n");
    if (xrmCuGroupRelease(ctx, &cuGroupRes))
        printf("success to release user defined cu group (same device)\n");
    else
        printf("fail to release user defined cu group (same device)\n");

    printf("Test 13-7: check user defined cu group (any device) available num\n");
    memset(&cuGroupProp, 0, sizeof(xrmCuGroupProperty));
    strcpy(cuGroupProp.udfCuGroupName, "udfCuGroupAnyDevice");
    cuGroupProp.poolId = 0;
    ret = xrmCheckCuGroupAvailableNum(ctx, &cuGroupProp);
    if (ret < 0) {
        printf("xrmCheckCuGroupAvaiableNum: fail to check user defined cu group (any device) available num\n");
    } else {
        printf("xrmCheckCuGroupAvaiableNum: user defined cu group (any device) available num = %d\n", ret);
    }

    printf("Test 13-8: alloc user defined cu group (any device)\n");
    memset(&cuGroupRes, 0, sizeof(xrmCuGroupResource));
    ret = xrmCuGroupAlloc(ctx, &cuGroupProp, &cuGroupRes);
    if (ret != XRM_SUCCESS) {
        printf("xrmCuGroupAlloc: fail to alloc user defined cu group (any device)\n");
    } else {
        for (i = 0; i < cuGroupRes.cuNum; i++) {
            printf("Allocated user defined cu group (any device): cu %d\n", i);
            printf("   xclbinFileName is:  %s\n", cuGroupRes.cuResources[i].xclbinFileName);
            printf("   kernelPluginFileName is:  %s\n", cuGroupRes.cuResources[i].kernelPluginFileName);
            printf("   kernelName is:  %s\n", cuGroupRes.cuResources[i].kernelName);
            printf("   kernelAlias is:  %s\n", cuGroupRes.cuResources[i].kernelAlias);
            printf("   instanceName is:  %s\n", cuGroupRes.cuResources[i].instanceName);
            printf("   cuName is:  %s\n", cuGroupRes.cuResources[i].cuName);
            printf("   deviceId is:  %d\n", cuGroupRes.cuResources[i].deviceId);
            printf("   cuId is:  %d\n", cuGroupRes.cuResources[i].cuId);
            printf("   channelId is:  %d\n", cuGroupRes.cuResources[i].channelId);
            printf("   cuType is:  %d\n", cuGroupRes.cuResources[i].cuType);
            printf("   baseAddr is:  0x%lx\n", cuGroupRes.cuResources[i].baseAddr);
            printf("   membankId is:  %d\n", cuGroupRes.cuResources[i].membankId);
            printf("   membankType is:  %d\n", cuGroupRes.cuResources[i].membankType);
            printf("   membankSize is:  0x%lx\n", cuGroupRes.cuResources[i].membankSize);
            printf("   membankBaseAddr is:  0x%lx\n", cuGroupRes.cuResources[i].membankBaseAddr);
            printf("   allocServiceId is:  %lu\n", cuGroupRes.cuResources[i].allocServiceId);
            printf("   poolId is:  %lu\n", cuGroupRes.cuResources[i].poolId);
            printf("   channelLoad is:  %d\n", cuGroupRes.cuResources[i].channelLoad);
        }
    }

    printf("Test 13-9: release user defined cu group (any device)\n");
    if (xrmCuGroupRelease(ctx, &cuGroupRes))
        printf("success to release user defined cu group (any device)\n");
    else
        printf("fail to release user defined cu group (any device)\n");

    printf("Test 13-10: user defined cu group from same device undeclaration\n");
    strcpy(udfCuGroupName, "udfCuGroupSameDevice");
    ret = xrmUdfCuGroupUndeclare(ctx, udfCuGroupName);
    if (ret == XRM_SUCCESS)
        printf("xrmUdfCuGroupUndeclare(): user defined cu group from same device undeclaration success\n");
    else
        printf("xrmUdfCuGroupUndeclare(): user defined cu group from same device undeclaration fail\n");

    printf("Test 13-11: user defined cu group from any device undeclaration\n");
    strcpy(udfCuGroupName, "udfCuGroupAnyDevice");
    ret = xrmUdfCuGroupUndeclare(ctx, udfCuGroupName);
    if (ret == XRM_SUCCESS)
        printf("xrmUdfCuGroupUndeclare(): user defined cu group from any device undeclaration success\n");
    else
        printf("xrmUdfCuGroupUndeclare(): user defined cu group from any device undeclaration fail\n");

    free(udfCuGroupProp);

    printf("<<<<<<<==  end the xrm user defined cu group test ===>>>>>>>>\n");
}

void xrmCuAllocWithLoadTest(xrmContext* ctx) {
    int32_t ret;
    printf("<<<<<<<==  start the xrm cu alloc with load test ===>>>>>>>>\n");
    if (ctx == NULL) {
        printf("ctx is null, fail to alloc cu list\n");
        return;
    }

    // alloc scaler cu
    xrmCuProperty scalerCuProp;
    xrmCuResource scalerCuRes;

    memset(&scalerCuProp, 0, sizeof(xrmCuProperty));
    memset(&scalerCuRes, 0, sizeof(xrmCuResource));
    strcpy(scalerCuProp.kernelName, "scaler");
    strcpy(scalerCuProp.kernelAlias, "");
    scalerCuProp.devExcl = false;
    scalerCuProp.requestLoad = 45;
    scalerCuProp.poolId = 0;

    printf("Test 14-1: alloc scaler with load\n");
    printf("    scaler cu prop: kernelName is %s\n", scalerCuProp.kernelName);
    printf("    scaler cu prop: kernelAlias is %s\n", scalerCuProp.kernelAlias);

    char xclbinFileName[XRM_MAX_PATH_NAME_LEN];
    strcpy(xclbinFileName, "/tmp/xclbins/test_xrm_u30.xclbin");

    printf("Test 14-2: Alloc scaler cu\n");
    ret = xrmCuAllocWithLoad(ctx, &scalerCuProp, xclbinFileName, &scalerCuRes);
    if (ret != 0) {
        printf("xrmCuAllocWithLoad: fail to alloc scaler cu\n");
    } else {
        printf("Allocated scaler cu with load: \n");
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

    printf("Test 14-3:  release scaler cu\n");
    if (xrmCuRelease(ctx, &scalerCuRes))
        printf("success to  release scaler cu\n");
    else
        printf("fail to release scaler cu\n");

    printf("<<<<<<<==  end the xrm allocation with load test ===>>>>>>>>\n");
}

void xrmLoadAndAllCuAllocTest(xrmContext* ctx) {
    int i;
    int32_t ret;
    printf("<<<<<<<==  start the xrm load and allocate all cu test ===>>>>>>>>\n");

    printf("Test 15-1: load xclbin and alloc all cu from this device\n");
    if (ctx == NULL) {
        printf("ctx is null, fail to do the test\n");
        return;
    }
    // load xclbin and alloc all cu
    xrmCuListResource cuListRes;

    memset(&cuListRes, 0, sizeof(xrmCuListResource));

    char xclbinFileName[XRM_MAX_PATH_NAME_LEN];
    strcpy(xclbinFileName, "/tmp/xclbins/test_xrm_u30.xclbin");

    ret = xrmLoadAndAllCuAlloc(ctx, xclbinFileName, &cuListRes);
    if (ret != 0) {
        printf("xrmLoadAndAllCuAlloc: fail to load xclbin and alloc all cu\n");
    } else {
        for (i = 0; i < cuListRes.cuNum; i++) {
            printf("Allocated scaler cu list: cu %d\n", i);
            printf("   xclbinFileName is:  %s\n", cuListRes.cuResources[i].xclbinFileName);
            printf("   kernelPluginFileName is:  %s\n", cuListRes.cuResources[i].kernelPluginFileName);
            printf("   kernelName is:  %s\n", cuListRes.cuResources[i].kernelName);
            printf("   kernelAlias is:  %s\n", cuListRes.cuResources[i].kernelAlias);
            printf("   instanceName is:  %s\n", cuListRes.cuResources[i].instanceName);
            printf("   cuName is:  %s\n", cuListRes.cuResources[i].cuName);
            printf("   deviceId is:  %d\n", cuListRes.cuResources[i].deviceId);
            printf("   cuId is:  %d\n", cuListRes.cuResources[i].cuId);
            printf("   channelId is:  %d\n", cuListRes.cuResources[i].channelId);
            printf("   cuType is:  %d\n", cuListRes.cuResources[i].cuType);
            printf("   baseAddr is:  0x%lx\n", cuListRes.cuResources[i].baseAddr);
            printf("   membankId is:  %d\n", cuListRes.cuResources[i].membankId);
            printf("   membankType is:  %d\n", cuListRes.cuResources[i].membankType);
            printf("   membankSize is:  0x%lx\n", cuListRes.cuResources[i].membankSize);
            printf("   membankBaseAddr is:  0x%lx\n", cuListRes.cuResources[i].membankBaseAddr);
            printf("   allocServiceId is:  %lu\n", cuListRes.cuResources[i].allocServiceId);
            printf("   poolId is:  %lu\n", cuListRes.cuResources[i].poolId);
            printf("   channelLoad is:  %d\n", cuListRes.cuResources[i].channelLoad);
        }
    }

    printf("Test 15-2:   release cu list\n");
    if (xrmCuListRelease(ctx, &cuListRes))
        printf("success to release cu list\n");
    else
        printf("fail to release cu list\n");
}

void xrmCuBlockingAllocReleaseTest(xrmContext* ctx) {
    int32_t ret;
    printf("<<<<<<<==  start the xrm blocing allocation test ===>>>>>>>>\n");
    if (ctx == NULL) {
        printf("ctx is null, fail to do cu blocking alloc test\n");
        return;
    }

    // alloc scaler cu
    xrmCuProperty scalerCuProp;
    xrmCuResource scalerCuRes;

    memset(&scalerCuProp, 0, sizeof(xrmCuProperty));
    memset(&scalerCuRes, 0, sizeof(xrmCuResource));
    strcpy(scalerCuProp.kernelName, "scaler");
    strcpy(scalerCuProp.kernelAlias, "");
    scalerCuProp.devExcl = false;
    scalerCuProp.requestLoad = 45;
    scalerCuProp.poolId = 0;

    uint64_t interval = 200000; // 200000 useconds (200 ms)

    printf("Test 16-1: Using blocking API to alloc scaler cu\n");
    printf("    scaler cu prop: kernelName is %s\n", scalerCuProp.kernelName);
    printf("    scaler cu prop: kernelAlias is %s\n", scalerCuProp.kernelAlias);
    printf("NOTE: The test will be blocked here if requested cu is NOT available on system.\n");
    printf("      You can safely stop the test with Ctrl+c\n");
    ret = xrmCuBlockingAlloc(ctx, &scalerCuProp, interval, &scalerCuRes);
    if (ret != 0) {
        printf("xrmCuBlockingAlloc: fail to alloc scaler cu, ret is %d\n", ret);
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

    printf("Test 16-2:  release scaler cu\n");
    if (xrmCuRelease(ctx, &scalerCuRes))
        printf("success to  release scaler cu\n");
    else
        printf("fail to release scaler cu\n");

    // alloc cal cu
    xrmCuProperty addCuProp;
    xrmCuResource addCuRes;

    memset(&addCuProp, 0, sizeof(xrmCuProperty));
    memset(&addCuRes, 0, sizeof(xrmCuResource));
    strcpy(addCuProp.kernelName, "krnl_vadd");
    strcpy(addCuProp.kernelAlias, "");
    addCuProp.devExcl = false;
    addCuProp.requestLoad = 45;
    addCuProp.poolId = 0;

    interval = 200000; // 200000 useconds (200 ms)

    printf("Test 16-3: Using blocking API to alloc add cu\n");
    printf("    add cu prop: kernelName is %s\n", addCuProp.kernelName);
    printf("    add cu prop: kernelAlias is %s\n", addCuProp.kernelAlias);
    printf("NOTE: The test will be blocked here if requested cu is NOT available on system.\n");
    printf("      You can safely stop the test with Ctrl+c\n");
    ret = xrmCuBlockingAlloc(ctx, &addCuProp, interval, &addCuRes);
    if (ret != 0) {
        printf("xrmCuBlockingAlloc: fail to alloc add cu, ret is %d\n", ret);
    } else {
        printf("Allocated add cu: \n");
        printf("   xclbinFileName is:  %s\n", addCuRes.xclbinFileName);
        printf("   kernelPluginFileName is:  %s\n", addCuRes.kernelPluginFileName);
        printf("   kernelName is:  %s\n", addCuRes.kernelName);
        printf("   instanceName is:  %s\n", addCuRes.instanceName);
        printf("   cuName is:  %s\n", addCuRes.cuName);
        printf("   kernelAlias is:  %s\n", addCuRes.kernelAlias);
        printf("   deviceId is:  %d\n", addCuRes.deviceId);
        printf("   cuId is:  %d\n", addCuRes.cuId);
        printf("   channelId is:  %d\n", addCuRes.channelId);
        printf("   cuType is:  %d\n", addCuRes.cuType);
        printf("   baseAddr is:  0x%lx\n", addCuRes.baseAddr);
        printf("   membankId is:  %d\n", addCuRes.membankId);
        printf("   membankType is:  %d\n", addCuRes.membankType);
        printf("   membankSize is:  0x%lx\n", addCuRes.membankSize);
        printf("   membankBaseAddr is:  0x%lx\n", addCuRes.membankBaseAddr);
        printf("   allocServiceId is:  %lu\n", addCuRes.allocServiceId);
        printf("   poolId is:  %lu\n", addCuRes.poolId);
        printf("   channelLoad is:  %d\n", addCuRes.channelLoad);
    }

    printf("Test 16-4:  release add cu\n");
    if (xrmCuRelease(ctx, &addCuRes))
        printf("success to  release add cu\n");
    else
        printf("fail to release add cu\n");

    printf("<<<<<<<==  end the xrm cu blocking allocation test ===>>>>>>>>\n");
}

void xrmCuListBlockingAllocReleaseTest(xrmContext* ctx) {
    int i;
    int32_t ret;
    printf("<<<<<<<==  start the xrm cu list blocking allocation test ===>>>>>>>>\n");

    printf("Test 17-1: Using blocking API to alloc scaler cu list\n");
    printf("NOTE: The test will be blocked here if requested cu list is NOT available on system.\n");
    printf("      You can safely stop the test with Ctrl+c\n");
    if (ctx == NULL) {
        printf("ctx is null, fail to do blocking alloc cu list\n");
        return;
    }
    // alloc scaler cu
    xrmCuListProperty scalerCuListProp;
    xrmCuListResource scalerCuListRes;

    memset(&scalerCuListProp, 0, sizeof(xrmCuListProperty));
    memset(&scalerCuListRes, 0, sizeof(xrmCuListResource));

    scalerCuListProp.cuNum = 4;
    for (i = 0; i < scalerCuListProp.cuNum; i++) {
        strcpy(scalerCuListProp.cuProps[i].kernelName, "scaler");
        strcpy(scalerCuListProp.cuProps[i].kernelAlias, "");
        scalerCuListProp.cuProps[i].devExcl = false;
        scalerCuListProp.cuProps[i].requestLoad = 45;
        scalerCuListProp.cuProps[i].poolId = 0;
    }

    uint64_t interval = 200000; // 200000 useconds (200 ms)

    ret = xrmCuListBlockingAlloc(ctx, &scalerCuListProp, interval, &scalerCuListRes);
    if (ret != 0) {
        printf("xrmCuListBlockingAlloc: fail to alloc scaler cu list\n");
    } else {
        for (i = 0; i < scalerCuListRes.cuNum; i++) {
            printf("Blocking allocated scaler cu list: cu %d\n", i);
            printf("   xclbinFileName is:  %s\n", scalerCuListRes.cuResources[i].xclbinFileName);
            printf("   kernelPluginFileName is:  %s\n", scalerCuListRes.cuResources[i].kernelPluginFileName);
            printf("   kernelName is:  %s\n", scalerCuListRes.cuResources[i].kernelName);
            printf("   kernelAlias is:  %s\n", scalerCuListRes.cuResources[i].kernelAlias);
            printf("   instanceName is:  %s\n", scalerCuListRes.cuResources[i].instanceName);
            printf("   cuName is:  %s\n", scalerCuListRes.cuResources[i].cuName);
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

    printf("Test 17-1:   release scaler cu list\n");
    if (xrmCuListRelease(ctx, &scalerCuListRes))
        printf("success to release scaler cu list\n");
    else
        printf("fail to release scaler cu list\n");

    // alloc calculate cu
    xrmCuListProperty calculateCuListProp;
    xrmCuListResource calculateCuListRes;

    memset(&calculateCuListProp, 0, sizeof(xrmCuListProperty));
    memset(&calculateCuListRes, 0, sizeof(xrmCuListResource));

    calculateCuListProp.cuNum = 2;
    strcpy(calculateCuListProp.cuProps[0].kernelName, "krnl_vadd");
    strcpy(calculateCuListProp.cuProps[0].kernelAlias, "");
    calculateCuListProp.cuProps[0].devExcl = false;
    calculateCuListProp.cuProps[0].requestLoad = 45;
    calculateCuListProp.cuProps[0].poolId = 0;

    strcpy(calculateCuListProp.cuProps[1].kernelName, "krnl_vsub");
    strcpy(calculateCuListProp.cuProps[1].kernelAlias, "");
    calculateCuListProp.cuProps[1].devExcl = false;
    calculateCuListProp.cuProps[1].requestLoad = 45;
    calculateCuListProp.cuProps[1].poolId = 0;

    interval = 200000; // 200000 useconds (200 ms)

    ret = xrmCuListBlockingAlloc(ctx, &calculateCuListProp, interval, &calculateCuListRes);
    if (ret != 0) {
        printf("xrmCuListBlockingAlloc: fail to alloc calculate cu list\n");
    } else {
        for (i = 0; i < calculateCuListRes.cuNum; i++) {
            printf("Blocking allocated calculate cu list: cu %d\n", i);
            printf("   xclbinFileName is:  %s\n", calculateCuListRes.cuResources[i].xclbinFileName);
            printf("   kernelPluginFileName is:  %s\n", calculateCuListRes.cuResources[i].kernelPluginFileName);
            printf("   kernelName is:  %s\n", calculateCuListRes.cuResources[i].kernelName);
            printf("   kernelAlias is:  %s\n", calculateCuListRes.cuResources[i].kernelAlias);
            printf("   instanceName is:  %s\n", calculateCuListRes.cuResources[i].instanceName);
            printf("   cuName is:  %s\n", calculateCuListRes.cuResources[i].cuName);
            printf("   deviceId is:  %d\n", calculateCuListRes.cuResources[i].deviceId);
            printf("   cuId is:  %d\n", calculateCuListRes.cuResources[i].cuId);
            printf("   channelId is:  %d\n", calculateCuListRes.cuResources[i].channelId);
            printf("   cuType is:  %d\n", calculateCuListRes.cuResources[i].cuType);
            printf("   baseAddr is:  0x%lx\n", calculateCuListRes.cuResources[i].baseAddr);
            printf("   membankId is:  %d\n", calculateCuListRes.cuResources[i].membankId);
            printf("   membankType is:  %d\n", calculateCuListRes.cuResources[i].membankType);
            printf("   membankSize is:  0x%lx\n", calculateCuListRes.cuResources[i].membankSize);
            printf("   membankBaseAddr is:  0x%lx\n", calculateCuListRes.cuResources[i].membankBaseAddr);
            printf("   allocServiceId is:  %lu\n", calculateCuListRes.cuResources[i].allocServiceId);
            printf("   poolId is:  %lu\n", calculateCuListRes.cuResources[i].poolId);
            printf("   channelLoad is:  %d\n", calculateCuListRes.cuResources[i].channelLoad);
        }
    }

    printf("Test 17-2:   release calculate cu list\n");
    if (xrmCuListRelease(ctx, &calculateCuListRes))
        printf("success to release calculate cu list\n");
    else
        printf("fail to release calculate cu list\n");
    printf("<<<<<<<==  end the xrm cu list blocking allocation test ===>>>>>>>>\n");
}

void xrmCuGroupBlockingAllocReleaseTest(xrmContext* ctx) {
    int32_t i, ret;
    printf("<<<<<<<==  start the xrm user defined cu group blocking alloc release test ===>>>>>>>>\n");
    if (ctx == NULL) {
        printf("ctx is null, fail to run user defined cu group blocking alloc release test\n");
        return;
    }

    printf("Test 18-1: test the user defined cu group declaration function\n");
    char udfCuGroupName[XRM_MAX_NAME_LEN];
    xrmUdfCuGroupProperty* udfCuGroupProp = (xrmUdfCuGroupProperty*)malloc(sizeof(xrmUdfCuGroupProperty));

    /* declare user defined cu group from same device */
    printf("Test 18-2: user defined cu group from same device declaration\n");
    memset(udfCuGroupProp, 0, sizeof(xrmUdfCuGroupProperty));
    strcpy(udfCuGroupName, "udfCuGroupSameDevice");
    xrmUdfCuListProperty* udfCuListProp;
    xrmUdfCuProperty* udfCuProp;
    udfCuGroupProp->optionUdfCuListNum = 2;
    /* define the first option cu list */
    udfCuListProp = &udfCuGroupProp->optionUdfCuListProps[0];
    udfCuListProp->cuNum = 5;
    udfCuListProp->sameDevice = true;
    for (int32_t cuIdx = 0; cuIdx < udfCuListProp->cuNum; cuIdx++) {
        udfCuProp = &udfCuListProp->udfCuProps[cuIdx];
        strcpy(udfCuProp->cuName, "encoder:encoder_1");
        udfCuProp->devExcl = false;
        udfCuProp->requestLoad = 30;
    }
    /* define the second option cu list */
    udfCuListProp = &udfCuGroupProp->optionUdfCuListProps[1];
    udfCuListProp->cuNum = 5;
    udfCuListProp->sameDevice = true;
    for (int32_t cuIdx = 0; cuIdx < udfCuListProp->cuNum; cuIdx++) {
        udfCuProp = &udfCuListProp->udfCuProps[cuIdx];
        strcpy(udfCuProp->cuName, "decoder:decoder_1");
        udfCuProp->devExcl = false;
        udfCuProp->requestLoad = 20;
    }
    ret = xrmUdfCuGroupDeclare(ctx, udfCuGroupProp, udfCuGroupName);
    if (ret == XRM_SUCCESS)
        printf("xrmUdfCuGroupDeclare(): user defined cu group from same device declaration success\n");
    else
        printf("xrmUdfCuGroupDeclare(): user defined cu group from same device declaration fail\n");

    printf("Test 18-3: check user defined cu group (same device) available num\n");
    xrmCuGroupResource cuGroupRes;
    xrmCuGroupProperty cuGroupProp;
    memset(&cuGroupProp, 0, sizeof(xrmCuGroupProperty));
    strcpy(cuGroupProp.udfCuGroupName, "udfCuGroupSameDevice");
    cuGroupProp.poolId = 0;
    ret = xrmCheckCuGroupAvailableNum(ctx, &cuGroupProp);
    if (ret < 0) {
        printf("xrmCheckCuGroupAvaiableNum: fail to check user defined cu group (same device) available num\n");
    } else {
        printf("xrmCheckCuGroupAvaiableNum: user defined cu group (same device) available num = %d\n", ret);
    }

    printf("Test 18-4: Using blocking API to alloc user defined cu group (same device)\n");
    printf("NOTE: The test will be blocked here if requested cu group is NOT available on system.\n");
    printf("      You can safely stop the test with Ctrl+c\n");
    memset(&cuGroupRes, 0, sizeof(xrmCuGroupResource));

    uint64_t interval = 0; // to use the XRM default interval

    ret = xrmCuGroupBlockingAlloc(ctx, &cuGroupProp, interval, &cuGroupRes);
    if (ret != XRM_SUCCESS) {
        printf("xrmCuGroupBlockingAlloc: fail to alloc user defined cu group (same device)\n");
    } else {
        for (i = 0; i < cuGroupRes.cuNum; i++) {
            printf("Blocking allocated user defined cu group (same device): cu %d\n", i);
            printf("   xclbinFileName is:  %s\n", cuGroupRes.cuResources[i].xclbinFileName);
            printf("   kernelPluginFileName is:  %s\n", cuGroupRes.cuResources[i].kernelPluginFileName);
            printf("   kernelName is:  %s\n", cuGroupRes.cuResources[i].kernelName);
            printf("   kernelAlias is:  %s\n", cuGroupRes.cuResources[i].kernelAlias);
            printf("   instanceName is:  %s\n", cuGroupRes.cuResources[i].instanceName);
            printf("   cuName is:  %s\n", cuGroupRes.cuResources[i].cuName);
            printf("   deviceId is:  %d\n", cuGroupRes.cuResources[i].deviceId);
            printf("   cuId is:  %d\n", cuGroupRes.cuResources[i].cuId);
            printf("   channelId is:  %d\n", cuGroupRes.cuResources[i].channelId);
            printf("   cuType is:  %d\n", cuGroupRes.cuResources[i].cuType);
            printf("   baseAddr is:  0x%lx\n", cuGroupRes.cuResources[i].baseAddr);
            printf("   membankId is:  %d\n", cuGroupRes.cuResources[i].membankId);
            printf("   membankType is:  %d\n", cuGroupRes.cuResources[i].membankType);
            printf("   membankSize is:  0x%lx\n", cuGroupRes.cuResources[i].membankSize);
            printf("   membankBaseAddr is:  0x%lx\n", cuGroupRes.cuResources[i].membankBaseAddr);
            printf("   allocServiceId is:  %lu\n", cuGroupRes.cuResources[i].allocServiceId);
            printf("   poolId is:  %lu\n", cuGroupRes.cuResources[i].poolId);
            printf("   channelLoad is:  %d\n", cuGroupRes.cuResources[i].channelLoad);
        }
    }

    printf("Test 18-5: release user defined cu group (same device)\n");
    if (xrmCuGroupRelease(ctx, &cuGroupRes))
        printf("success to release user defined cu group (same device)\n");
    else
        printf("fail to release user defined cu group (same device)\n");

    printf("Test 18-6: user defined cu group from same device undeclaration\n");
    strcpy(udfCuGroupName, "udfCuGroupSameDevice");
    ret = xrmUdfCuGroupUndeclare(ctx, udfCuGroupName);
    if (ret == XRM_SUCCESS)
        printf("xrmUdfCuGroupUndeclare(): user defined cu group from same device undeclaration success\n");
    else
        printf("xrmUdfCuGroupUndeclare(): user defined cu group from same device undeclaration fail\n");

    free(udfCuGroupProp);

    printf("<<<<<<<==  end the xrm user defined cu group blocking alloc release test ===>>>>>>>>\n");
}

void xrmCuAllocFromDevReleaseTest(xrmContext* ctx) {
    int32_t ret;
    printf("<<<<<<<==  start the xrm allocation from specified device test ===>>>>>>>>\n");
    if (ctx == NULL) {
        printf("ctx is null, fail to do cu alloc from specified device test\n");
        return;
    }

    // alloc scaler cu
    xrmCuProperty scalerCuProp;
    xrmCuResource scalerCuRes;
    int32_t deviceId;

    // alloc scaler cu from device 0
    memset(&scalerCuProp, 0, sizeof(xrmCuProperty));
    memset(&scalerCuRes, 0, sizeof(xrmCuResource));
    strcpy(scalerCuProp.kernelName, "scaler");
    strcpy(scalerCuProp.kernelAlias, "");
    scalerCuProp.devExcl = false;
    scalerCuProp.requestLoad = 45;
    scalerCuProp.poolId = 0;
    deviceId = 0;

    printf("Test 19-1: Alloc scaler cu from device %d\n", deviceId);
    ret = xrmCuAllocFromDev(ctx, deviceId, &scalerCuProp, &scalerCuRes);
    if (ret != 0) {
        printf("xrmCuAlloc: fail to alloc scaler cu from device %d\n", deviceId);
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

    printf("Test 19-2:  release scaler cu\n");
    if (xrmCuRelease(ctx, &scalerCuRes))
        printf("success to  release scaler cu\n");
    else
        printf("fail to release scaler cu\n");

    // alloc scaler cu from device 1
    memset(&scalerCuProp, 0, sizeof(xrmCuProperty));
    memset(&scalerCuRes, 0, sizeof(xrmCuResource));
    strcpy(scalerCuProp.kernelName, "");
    strcpy(scalerCuProp.kernelAlias, "SCALER_MPSOC");
    scalerCuProp.devExcl = false;
    scalerCuProp.requestLoad = 55;
    scalerCuProp.poolId = 0;
    deviceId = 1;

    printf("Test 19-3: Alloc scaler cu from device %d\n", deviceId);
    ret = xrmCuAllocFromDev(ctx, deviceId, &scalerCuProp, &scalerCuRes);
    if (ret != 0) {
        printf("xrmCuAlloc: fail to alloc scaler cu from device %d\n", deviceId);
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

    printf("Test 19-4:  release scaler cu\n");
    if (xrmCuRelease(ctx, &scalerCuRes))
        printf("success to  release scaler cu\n");
    else
        printf("fail to release scaler cu\n");
    printf("<<<<<<<==  end the xrm allocation from specified device test ===>>>>>>>>\n");
}

void xrmCuAllocReleaseGranularity1000000Test(xrmContext* ctx) {
    int32_t ret;
    printf("<<<<<<<==  start the xrm allocation Granularity 1000000 test ===>>>>>>>>\n");
    if (ctx == NULL) {
        printf("ctx is null, fail to do cu alloc test\n");
        return;
    }

    // alloc scaler cu
    xrmCuProperty scalerCuProp;
    xrmCuResource scalerCuRes;
    xrmCuStat scalerCuStat;
    uint64_t maxCapacity;

    memset(&scalerCuProp, 0, sizeof(xrmCuProperty));
    memset(&scalerCuRes, 0, sizeof(xrmCuResource));
    memset(&scalerCuStat, 0, sizeof(xrmCuStat));
    strcpy(scalerCuProp.kernelName, "scaler");
    strcpy(scalerCuProp.kernelAlias, "");
    scalerCuProp.devExcl = false;
    scalerCuProp.requestLoad = 450000 << 8;
    scalerCuProp.poolId = 0;

    printf("Test 20-1: check scaler cu available number\n");
    printf("    scaler cu prop: kernelName is %s\n", scalerCuProp.kernelName);
    printf("    scaler cu prop: kernelAlias is %s\n", scalerCuProp.kernelAlias);

    ret = xrmCheckCuAvailableNum(ctx, &scalerCuProp);
    if (ret < 0) {
        printf("xrmCheckCuAvailableNum: fail to check scaler cu available number\n");
    } else {
        printf("xrmCheckCuAvailableNum: scaler cu available number = %d\n", ret);
    }

    printf("Test 20-2: Alloc scaler cu\n");
    ret = xrmCuAlloc(ctx, &scalerCuProp, &scalerCuRes);
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

    maxCapacity = xrmCuGetMaxCapacity(ctx, &scalerCuProp);
    printf("Test 20-3: Get Max Capacity: scaler cu max capacity is: %lu\n", maxCapacity);

    printf("Test 20-4: Check SCALER cu status: \n");
    ret = xrmCuCheckStatus(ctx, &scalerCuRes, &scalerCuStat);
    if (ret != 0) {
        printf("xrmCuCheckStatus: fail to check status of scaler cu\n");
    } else {
        if (scalerCuStat.isBusy)
            printf("   isBusy:  true\n");
        else
            printf("   isBusy:  false\n");
        printf("   usedLoad:  %d\n", scalerCuStat.usedLoad);
    }

    // alloc encoder cu
    xrmCuProperty encCuProp;
    xrmCuResource encCuRes;

    memset(&encCuProp, 0, sizeof(xrmCuProperty));
    memset(&encCuRes, 0, sizeof(xrmCuResource));
    strcpy(encCuProp.kernelName, "encoder");
    strcpy(encCuProp.kernelAlias, "");
    encCuProp.devExcl = false;
    encCuProp.requestLoad = 550000 << 8;
    encCuProp.poolId = 0;

    maxCapacity = xrmCuGetMaxCapacity(ctx, &encCuProp);
    printf("Test 20-5: Get Max Capacity: encoder cu max capacity is: %lu\n", maxCapacity);

    printf("Test 20-6: Check encoder cu available number ...\n");
    printf("    kernelName is %s\n", encCuProp.kernelName);

    ret = xrmCheckCuAvailableNum(ctx, &encCuProp);
    if (ret < 0) {
        printf("xrmCheckCuAvailableNum: fail to check encoder cu available number\n");
    } else {
        printf("xrmCheckCuAvailableNum: encoder cu available number = %d\n", ret);
    }

    printf("Test 20-7: Alloc encoder cu\n");
    ret = xrmCuAlloc(ctx, &encCuProp, &encCuRes);
    if (ret != 0) {
        printf("xrmCuAlloc: fail to alloc encoder cu\n");
    } else {
        printf("Allocated encoder cu: \n");
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

    printf("<<<<<<<==  end the xrm allocation test ===>>>>>>>>\n");

    printf("Test 20-8:  release scaler cu\n");
    if (xrmCuRelease(ctx, &scalerCuRes))
        printf("success to  release scaler cu\n");
    else
        printf("fail to release scaler cu\n");

    printf("Test 20-9:   release encoder cu\n");
    if (xrmCuRelease(ctx, &encCuRes))
        printf("success to release encoder cu\n");
    else
        printf("fail to release encoder cu\n");
}

void xrmCuListAllocReleaseGranularity1000000Test(xrmContext* ctx) {
    int i;
    int32_t ret;
    printf("<<<<<<<==  start the xrm cu list allocation Granularity 1000000 test ===>>>>>>>>\n");

    printf("Test 21-1: alloc scaler cu list\n");
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
        strcpy(scalerCuListProp.cuProps[i].kernelName, "scaler");
        strcpy(scalerCuListProp.cuProps[i].kernelAlias, "");
        scalerCuListProp.cuProps[i].devExcl = false;
        scalerCuListProp.cuProps[i].requestLoad = 450000 << 8;
        scalerCuListProp.cuProps[i].poolId = 0;
    }

    ret = xrmCuListAlloc(ctx, &scalerCuListProp, &scalerCuListRes);
    if (ret != 0) {
        printf("xrmCuListAlloc: fail to alloc scaler cu list\n");
    } else {
        for (i = 0; i < scalerCuListRes.cuNum; i++) {
            printf("Allocated scaler cu list: cu %d\n", i);
            printf("   xclbinFileName is:  %s\n", scalerCuListRes.cuResources[i].xclbinFileName);
            printf("   kernelPluginFileName is:  %s\n", scalerCuListRes.cuResources[i].kernelPluginFileName);
            printf("   kernelName is:  %s\n", scalerCuListRes.cuResources[i].kernelName);
            printf("   kernelAlias is:  %s\n", scalerCuListRes.cuResources[i].kernelAlias);
            printf("   instanceName is:  %s\n", scalerCuListRes.cuResources[i].instanceName);
            printf("   cuName is:  %s\n", scalerCuListRes.cuResources[i].cuName);
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

    printf("Test 21-2:  alloc encoder cu list\n");
    // alloc enc cu
    xrmCuListProperty encCuListProp;
    xrmCuListResource encCuListRes;
    memset(&encCuListProp, 0, sizeof(encCuListProp));
    memset(&encCuListRes, 0, sizeof(encCuListRes));
    encCuListProp.cuNum = 4;
    for (i = 0; i < encCuListProp.cuNum; i++) {
        strcpy(encCuListProp.cuProps[i].kernelName, "encoder");
        strcpy(encCuListProp.cuProps[i].kernelAlias, "");
        encCuListProp.cuProps[i].devExcl = false;
        encCuListProp.cuProps[i].requestLoad = 550000 << 8;
        encCuListProp.cuProps[i].poolId = 0;
    }

    ret = xrmCuListAlloc(ctx, &encCuListProp, &encCuListRes);
    if (ret != 0) {
        printf("xrmCuListAlloc: fail to alloc encoder cu list\n");
    } else {
        for (i = 0; i < encCuListRes.cuNum; i++) {
            printf("Allocated ENCODER cu list: cu %d\n", i);
            printf("   xclbinFileName is:  %s\n", encCuListRes.cuResources[i].xclbinFileName);
            printf("   kernelPluginFileName is:  %s\n", encCuListRes.cuResources[i].kernelPluginFileName);
            printf("   kernelName is:  %s\n", encCuListRes.cuResources[i].kernelName);
            printf("   kernelAlias is:  %s\n", encCuListRes.cuResources[i].kernelAlias);
            printf("   instanceName is:  %s\n", encCuListRes.cuResources[i].instanceName);
            printf("   cuName is:  %s\n", encCuListRes.cuResources[i].cuName);
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

    printf("Test 21-3:   release scaler cu list\n");
    if (xrmCuListRelease(ctx, &scalerCuListRes))
        printf("success to release scaler cu list\n");
    else
        printf("fail to release scaler cu list\n");

    printf("Test 21-4:  release scaler cu list\n");
    if (xrmCuListRelease(ctx, &encCuListRes))
        printf("success to release encoder cu list\n");
    else
        printf("fail to release encoder cu list\n");
}

void xrmSoftCuAllocReleaseGranularity1000000Test(xrmContext* ctx) {
    printf("<<<<<<<==  start the xrm soft cu allocation Granularity 1000000test ===>>>>>>>>\n");

    // alloc scaler cu
    xrmCuProperty scalerCuProp;
    xrmCuResource scalerCuRes;
    xrmCuStat scalerCuStat;
    uint64_t maxCapacity;

    memset(&scalerCuProp, 0, sizeof(xrmCuProperty));
    memset(&scalerCuRes, 0, sizeof(xrmCuResource));
    memset(&scalerCuStat, 0, sizeof(xrmCuStat));
    strcpy(scalerCuProp.kernelName, "kernel_vcu_decoder");
    strcpy(scalerCuProp.kernelAlias, "");
    scalerCuProp.devExcl = false;
    scalerCuProp.requestLoad = 450000 << 8;
    scalerCuProp.poolId = 0;

    printf("Test 22-1: Alloc scaler cu, kernelName is %s\n", scalerCuProp.kernelName);
    printf("Test 22-1: Alloc scaler cu, kernelAlias is %s\n", scalerCuProp.kernelAlias);

    int32_t ret = xrmCuAlloc(ctx, &scalerCuProp, &scalerCuRes);
    if (ret != 0) {
        printf("Allocated scaler cu failed.\n");
    } else {
        printf("Allocated scaler cu: \n");
        printf("   xclbinFileName is:  %s\n", scalerCuRes.xclbinFileName);
        printf("   kernelPluginFileName is:  %s\n", scalerCuRes.kernelPluginFileName);
        printf("   kernelName is:  %s\n", scalerCuRes.kernelName);
        printf("   kernelAlias is:  %s\n", scalerCuRes.kernelAlias);
        printf("   instanceName is:  %s\n", scalerCuRes.instanceName);
        printf("   cuName is:  %s\n", scalerCuRes.cuName);
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

    maxCapacity = xrmCuGetMaxCapacity(ctx, &scalerCuProp);
    printf("Test 22-2: Get Max Capacity: kernel_vcu_decoder cu max capacity is: %lu\n", maxCapacity);

    printf("Test 22-3: Check SCALER cu status: \n");
    ret = (xrmCuCheckStatus(ctx, &scalerCuRes, &scalerCuStat));
    if (ret != 0) {
        printf("xrmCuCheckStatus: fail to check status of scaler cu\n");
    } else {
        if (scalerCuStat.isBusy)
            printf("   isBusy:  true\n");
        else
            printf("   isBusy:  false\n");
        printf("   usedLoad:  %d\n", scalerCuStat.usedLoad);
    }

    printf("<<<<<<<==  end the xrm soft cu allocation test ===>>>>>>>>\n");

    printf("Test 22-4:  release scaler cu\n");
    if (xrmCuRelease(ctx, &scalerCuRes))
        printf("success to release scaler cu\n");
    else
        printf("fail to release scaler cu\n");
}

void xrmCuListAllocReleaseFromSameDevGranularity1000000Test(xrmContext* ctx) {
    int i;
    int32_t ret;
    printf("<<<<<<<==  start the xrm cu list from same dev allocation Granularity 1000000 test ===>>>>>>>>\n");

    printf("Test 23-1: alloc scaler cu list\n");
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
    scalerCuListProp.sameDevice = true;
    for (i = 0; i < scalerCuListProp.cuNum; i++) {
        strcpy(scalerCuListProp.cuProps[i].kernelName, "scaler");
        strcpy(scalerCuListProp.cuProps[i].kernelAlias, "");
        scalerCuListProp.cuProps[i].devExcl = false;
        scalerCuListProp.cuProps[i].requestLoad = 200000 << 8;
        scalerCuListProp.cuProps[i].poolId = 0;
    }

    ret = xrmCuListAlloc(ctx, &scalerCuListProp, &scalerCuListRes);
    if (ret != 0) {
        printf("xrmCuListAlloc: fail to alloc scaler cu list\n");
    } else {
        for (i = 0; i < scalerCuListRes.cuNum; i++) {
            printf("Allocated scaler cu list: cu %d\n", i);
            printf("   xclbinFileName is:  %s\n", scalerCuListRes.cuResources[i].xclbinFileName);
            printf("   kernelPluginFileName is:  %s\n", scalerCuListRes.cuResources[i].kernelPluginFileName);
            printf("   kernelName is:  %s\n", scalerCuListRes.cuResources[i].kernelName);
            printf("   kernelAlias is:  %s\n", scalerCuListRes.cuResources[i].kernelAlias);
            printf("   instanceName is:  %s\n", scalerCuListRes.cuResources[i].instanceName);
            printf("   cuName is:  %s\n", scalerCuListRes.cuResources[i].cuName);
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

    printf("Test 23-2:  alloc encoder cu list\n");
    // alloc enc cu
    xrmCuListProperty encCuListProp;
    xrmCuListResource encCuListRes;
    memset(&encCuListProp, 0, sizeof(encCuListProp));
    memset(&encCuListRes, 0, sizeof(encCuListRes));
    encCuListProp.cuNum = 4;
    encCuListProp.sameDevice = true;
    for (i = 0; i < encCuListProp.cuNum; i++) {
        strcpy(encCuListProp.cuProps[i].kernelName, "encoder");
        strcpy(encCuListProp.cuProps[i].kernelAlias, "");
        encCuListProp.cuProps[i].devExcl = false;
        encCuListProp.cuProps[i].requestLoad = 300000 << 8;
        encCuListProp.cuProps[i].poolId = 0;
    }

    ret = xrmCuListAlloc(ctx, &encCuListProp, &encCuListRes);
    if (ret != 0) {
        printf("xrmCuListAlloc: fail to alloc encoder cu list\n");
    } else {
        for (i = 0; i < encCuListRes.cuNum; i++) {
            printf("Allocated ENCODER cu list: cu %d\n", i);
            printf("   xclbinFileName is:  %s\n", encCuListRes.cuResources[i].xclbinFileName);
            printf("   kernelPluginFileName is:  %s\n", encCuListRes.cuResources[i].kernelPluginFileName);
            printf("   kernelName is:  %s\n", encCuListRes.cuResources[i].kernelName);
            printf("   kernelAlias is:  %s\n", encCuListRes.cuResources[i].kernelAlias);
            printf("   instanceName is:  %s\n", encCuListRes.cuResources[i].instanceName);
            printf("   cuName is:  %s\n", encCuListRes.cuResources[i].cuName);
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

    printf("Test 23-3:   release scaler cu list\n");
    if (xrmCuListRelease(ctx, &scalerCuListRes))
        printf("success to release scaler cu list\n");
    else
        printf("fail to release scaler cu list\n");

    printf("Test 23-4:  release scaler cu list\n");
    if (xrmCuListRelease(ctx, &encCuListRes))
        printf("success to release encoder cu list\n");
    else
        printf("fail to release encoder cu list\n");
}

void xrmCuAllocReleaseUsingAliasGranularity1000000Test(xrmContext* ctx) {
    printf("<<<<<<<==  start the xrm allocation with kernel alias Granularity 1000000 test ===>>>>>>>>\n");

    // alloc scaler cu
    xrmCuProperty scalerCuProp;
    xrmCuResource scalerCuRes;
    xrmCuStat scalerCuStat;
    uint64_t maxCapacity;

    memset(&scalerCuProp, 0, sizeof(xrmCuProperty));
    memset(&scalerCuRes, 0, sizeof(xrmCuResource));
    memset(&scalerCuStat, 0, sizeof(xrmCuStat));
    strcpy(scalerCuProp.kernelName, "");
    strcpy(scalerCuProp.kernelAlias, "SCALER_MPSOC");
    scalerCuProp.devExcl = false;
    scalerCuProp.requestLoad = 450000 << 8;
    scalerCuProp.poolId = 0;

    printf("Test 24-1: Alloc scaler cu, kernelName is %s\n", scalerCuProp.kernelName);
    printf("Test 24-1: Alloc scaler cu, kernelAlias is %s\n", scalerCuProp.kernelAlias);

    int32_t ret = xrmCuAlloc(ctx, &scalerCuProp, &scalerCuRes);
    if (ret != 0) {
        printf("Allocated scaler cu failed.\n");
    } else {
        printf("Allocated scaler cu: \n");
        printf("   xclbinFileName is:  %s\n", scalerCuRes.xclbinFileName);
        printf("   kernelPluginFileName is:  %s\n", scalerCuRes.kernelPluginFileName);
        printf("   kernelName is:  %s\n", scalerCuRes.kernelName);
        printf("   kernelAlias is:  %s\n", scalerCuRes.kernelAlias);
        printf("   instanceName is:  %s\n", scalerCuRes.instanceName);
        printf("   cuName is:  %s\n", scalerCuRes.cuName);
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

    maxCapacity = xrmCuGetMaxCapacity(ctx, &scalerCuProp);
    printf("Test 24-2: Get Max Capacity: SCALER_MPSOC cu max capacity is: %lu\n", maxCapacity);

    printf("Test 24-3: Check SCALER cu status: \n");
    ret = (xrmCuCheckStatus(ctx, &scalerCuRes, &scalerCuStat));
    if (ret != 0) {
        printf("xrmCuCheckStatus: fail to check status of scaler cu\n");
    } else {
        if (scalerCuStat.isBusy)
            printf("   isBusy:  true\n");
        else
            printf("   isBusy:  false\n");
        printf("   usedLoad:  %d\n", scalerCuStat.usedLoad);
    }

    printf("<<<<<<<==  end the xrm allocation with kernel alias test ===>>>>>>>>\n");

    printf("Test 24-4:  release scaler cu\n");
    if (xrmCuRelease(ctx, &scalerCuRes))
        printf("success to release scaler cu\n");
    else
        printf("fail to release scaler cu\n");
}

void xrmCuAllocReleaseUsingKernelNameAndAliasGranularity1000000Test(xrmContext* ctx) {
    printf("<<<<<<<==  start the xrm allocation with kernel name and alias Granularity 1000000 test ===>>>>>>>>\n");

    // alloc scaler cu
    xrmCuProperty scalerCuProp;
    xrmCuResource scalerCuRes;
    xrmCuStat scalerCuStat;
    uint64_t maxCapacity;

    memset(&scalerCuProp, 0, sizeof(xrmCuProperty));
    memset(&scalerCuRes, 0, sizeof(xrmCuResource));
    memset(&scalerCuStat, 0, sizeof(xrmCuStat));
    strcpy(scalerCuProp.kernelName, "scaler");
    strcpy(scalerCuProp.kernelAlias, "SCALER_MPSOC");
    scalerCuProp.devExcl = false;
    scalerCuProp.requestLoad = 450000 << 8;
    scalerCuProp.poolId = 0;

    printf("Test 25-1: Alloc scaler cu, kernelName is %s\n", scalerCuProp.kernelName);
    printf("Test 25-1: Alloc scaler cu, kernelAlias is %s\n", scalerCuProp.kernelAlias);

    int32_t ret = xrmCuAlloc(ctx, &scalerCuProp, &scalerCuRes);
    if (ret != 0) {
        printf("Allocated scaler cu failed.\n");
    } else {
        printf("Allocated scaler cu: \n");
        printf("   xclbinFileName is:  %s\n", scalerCuRes.xclbinFileName);
        printf("   kernelPluginFileName is:  %s\n", scalerCuRes.kernelPluginFileName);
        printf("   kernelName is:  %s\n", scalerCuRes.kernelName);
        printf("   kernelAlias is:  %s\n", scalerCuRes.kernelAlias);
        printf("   instanceName is:  %s\n", scalerCuRes.instanceName);
        printf("   cuName is:  %s\n", scalerCuRes.cuName);
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

    maxCapacity = xrmCuGetMaxCapacity(ctx, &scalerCuProp);
    printf("Test 25-2: Get Max Capacity: scaler cu max capacity is: %lu\n", maxCapacity);

    printf("Test 25-3: Check SCALER cu status: \n");
    ret = (xrmCuCheckStatus(ctx, &scalerCuRes, &scalerCuStat));
    if (ret != 0) {
        printf("xrmCuCheckStatus: fail to check status of scaler cu\n");
    } else {
        if (scalerCuStat.isBusy)
            printf("   isBusy:  true\n");
        else
            printf("   isBusy:  false\n");
        printf("   usedLoad:  %d\n", scalerCuStat.usedLoad);
    }

    printf("<<<<<<<==  end the xrm allocation with kernel name and alias test ===>>>>>>>>\n");

    printf("Test 25-4:  release scaler cu\n");
    if (xrmCuRelease(ctx, &scalerCuRes))
        printf("success to release scaler cu\n");
    else
        printf("fail to release scaler cu\n");
}

void xrmCuAllocQueryReleaseUsingAliasGranularity1000000Test(xrmContext* ctx) {
    int i, ret;

    printf("<<<<<<<==  start the xrm allocation and query with kernel alias Granularity 1000000 test ===>>>>>>>>\n");

    printf("Test 26-1: alloc scaler cu list\n");
    if (ctx == NULL) {
        printf("ctx is null, fail to alloc cu list\n");
        return;
    }

    xrmCuListProperty scalerCuListProp;
    xrmCuListResource scalerCuListRes;

    memset(&scalerCuListProp, 0, sizeof(scalerCuListProp));
    memset(&scalerCuListRes, 0, sizeof(scalerCuListRes));

    scalerCuListProp.cuNum = 4;
    scalerCuListProp.sameDevice = false;
    for (i = 0; i < scalerCuListProp.cuNum; i++) {
        strcpy(scalerCuListProp.cuProps[i].kernelName, "scaler");
        strcpy(scalerCuListProp.cuProps[i].kernelAlias, "");
        scalerCuListProp.cuProps[i].devExcl = false;
        scalerCuListProp.cuProps[i].requestLoad = 450000 << 8;
        scalerCuListProp.cuProps[i].poolId = 0;
    }

    ret = xrmCuListAlloc(ctx, &scalerCuListProp, &scalerCuListRes);
    if (ret != 0) {
        printf("xrmCuListAlloc: fail to alloc scaler cu list\n");
    } else {
        for (i = 0; i < scalerCuListRes.cuNum; i++) {
            printf("Allocated scaler cu list: cu %d\n", i);
            printf("   xclbinFileName is:  %s\n", scalerCuListRes.cuResources[i].xclbinFileName);
            printf("   kernelPluginFileName is:  %s\n", scalerCuListRes.cuResources[i].kernelPluginFileName);
            printf("   kernelName is:  %s\n", scalerCuListRes.cuResources[i].kernelName);
            printf("   kernelAlias is:  %s\n", scalerCuListRes.cuResources[i].kernelAlias);
            printf("   instanceName is:  %s\n", scalerCuListRes.cuResources[i].instanceName);
            printf("   cuName is:  %s\n", scalerCuListRes.cuResources[i].cuName);
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

    printf("Test 26-2:  query scaler cu list with allocServiceId\n");
    xrmAllocationQueryInfo allocQuery;
    xrmCuListResource queryScalerCuListRes;

    memset(&allocQuery, 0, sizeof(xrmAllocationQueryInfo));
    memset(&queryScalerCuListRes, 0, sizeof(xrmAllocationQueryInfo));
    allocQuery.allocServiceId = scalerCuListRes.cuResources[0].allocServiceId;
    ret = xrmAllocationQuery(ctx, &allocQuery, &queryScalerCuListRes);
    if (ret != 0) {
        printf("xrmAllocationQuery: fail to query allocated scaler cu list with allocServiceId\n");
    } else {
        for (i = 0; i < queryScalerCuListRes.cuNum; i++) {
            printf("Query Allocated scaler cu list: cu %d\n", i);
            printf("   xclbinFileName is:  %s\n", queryScalerCuListRes.cuResources[i].xclbinFileName);
            printf("   kernelPluginFileName is:  %s\n", queryScalerCuListRes.cuResources[i].kernelPluginFileName);
            printf("   kernelName is:  %s\n", queryScalerCuListRes.cuResources[i].kernelName);
            printf("   kernelAlias is:  %s\n", queryScalerCuListRes.cuResources[i].kernelAlias);
            printf("   instanceName is:  %s\n", queryScalerCuListRes.cuResources[i].instanceName);
            printf("   cuName is:  %s\n", queryScalerCuListRes.cuResources[i].cuName);
            printf("   deviceId is:  %d\n", queryScalerCuListRes.cuResources[i].deviceId);
            printf("   cuId is:  %d\n", queryScalerCuListRes.cuResources[i].cuId);
            printf("   channelId is:  %d\n", queryScalerCuListRes.cuResources[i].channelId);
            printf("   cuType is:  %d\n", queryScalerCuListRes.cuResources[i].cuType);
            printf("   baseAddr is:  0x%lx\n", queryScalerCuListRes.cuResources[i].baseAddr);
            printf("   membankId is:  %d\n", queryScalerCuListRes.cuResources[i].membankId);
            printf("   membankType is:  %d\n", queryScalerCuListRes.cuResources[i].membankType);
            printf("   membankSize is:  0x%lx\n", queryScalerCuListRes.cuResources[i].membankSize);
            printf("   membankBaseAddr is:  0x%lx\n", queryScalerCuListRes.cuResources[i].membankBaseAddr);
            printf("   allocServiceId is:  %lu\n", queryScalerCuListRes.cuResources[i].allocServiceId);
            printf("   poolId is:  %lu\n", queryScalerCuListRes.cuResources[i].poolId);
            printf("   channelLoad is:  %d\n", queryScalerCuListRes.cuResources[i].channelLoad);
        }
    }

    printf("Test 26-3:  query scaler cu list with allocServiceId and kernel alias\n");
    memset(&allocQuery, 0, sizeof(xrmAllocationQueryInfo));
    memset(&queryScalerCuListRes, 0, sizeof(xrmCuListResource));
    allocQuery.allocServiceId = scalerCuListRes.cuResources[0].allocServiceId;
    strcpy(allocQuery.kernelName, "");
    strcpy(allocQuery.kernelAlias, "SCALER_MPSOC");
    ret = xrmAllocationQuery(ctx, &allocQuery, &queryScalerCuListRes);
    if (ret != 0) {
        printf("xrmAllocationQuery: fail to query allocated scaler cu list with allocServiceId and kernel alias\n");
    } else {
        for (i = 0; i < queryScalerCuListRes.cuNum; i++) {
            printf("Query Allocated scaler cu list: cu %d\n", i);
            printf("   xclbinFileName is:  %s\n", queryScalerCuListRes.cuResources[i].xclbinFileName);
            printf("   kernelPluginFileName is:  %s\n", queryScalerCuListRes.cuResources[i].kernelPluginFileName);
            printf("   kernelName is:  %s\n", queryScalerCuListRes.cuResources[i].kernelName);
            printf("   kernelAlias is:  %s\n", queryScalerCuListRes.cuResources[i].kernelAlias);
            printf("   instanceName is:  %s\n", queryScalerCuListRes.cuResources[i].instanceName);
            printf("   cuName is:  %s\n", queryScalerCuListRes.cuResources[i].cuName);
            printf("   deviceId is:  %d\n", queryScalerCuListRes.cuResources[i].deviceId);
            printf("   cuId is:  %d\n", queryScalerCuListRes.cuResources[i].cuId);
            printf("   channelId is:  %d\n", queryScalerCuListRes.cuResources[i].channelId);
            printf("   cuType is:  %d\n", queryScalerCuListRes.cuResources[i].cuType);
            printf("   baseAddr is:  0x%lx\n", queryScalerCuListRes.cuResources[i].baseAddr);
            printf("   membankId is:  %d\n", queryScalerCuListRes.cuResources[i].membankId);
            printf("   membankType is:  %d\n", queryScalerCuListRes.cuResources[i].membankType);
            printf("   membankSize is:  0x%lx\n", queryScalerCuListRes.cuResources[i].membankSize);
            printf("   membankBaseAddr is:  0x%lx\n", queryScalerCuListRes.cuResources[i].membankBaseAddr);
            printf("   allocServiceId is:  %lu\n", queryScalerCuListRes.cuResources[i].allocServiceId);
            printf("   poolId is:  %lu\n", queryScalerCuListRes.cuResources[i].poolId);
            printf("   channelLoad is:  %d\n", queryScalerCuListRes.cuResources[i].channelLoad);
        }
    }

    printf("Test 26-4:  query scaler cu list with allocServiceId and kernel name\n");
    memset(&allocQuery, 0, sizeof(xrmAllocationQuery));
    memset(&queryScalerCuListRes, 0, sizeof(xrmCuListResource));
    allocQuery.allocServiceId = scalerCuListRes.cuResources[0].allocServiceId;
    strcpy(allocQuery.kernelName, "scaler");
    strcpy(allocQuery.kernelAlias, "");
    ret = xrmAllocationQuery(ctx, &allocQuery, &queryScalerCuListRes);
    if (ret != 0) {
        printf("xrmAllocationQuery: fail to query allocated scaler cu list with allocServiceId and kernel name\n");
    } else {
        for (i = 0; i < queryScalerCuListRes.cuNum; i++) {
            printf("Query Allocated scaler cu list: cu %d\n", i);
            printf("   xclbinFileName is:  %s\n", queryScalerCuListRes.cuResources[i].xclbinFileName);
            printf("   kernelPluginFileName is:  %s\n", queryScalerCuListRes.cuResources[i].kernelPluginFileName);
            printf("   kernelName is:  %s\n", queryScalerCuListRes.cuResources[i].kernelName);
            printf("   kernelAlias is:  %s\n", queryScalerCuListRes.cuResources[i].kernelAlias);
            printf("   instanceName is:  %s\n", queryScalerCuListRes.cuResources[i].instanceName);
            printf("   cuName is:  %s\n", queryScalerCuListRes.cuResources[i].cuName);
            printf("   deviceId is:  %d\n", queryScalerCuListRes.cuResources[i].deviceId);
            printf("   cuId is:  %d\n", queryScalerCuListRes.cuResources[i].cuId);
            printf("   channelId is:  %d\n", queryScalerCuListRes.cuResources[i].channelId);
            printf("   cuType is:  %d\n", queryScalerCuListRes.cuResources[i].cuType);
            printf("   baseAddr is:  0x%lx\n", queryScalerCuListRes.cuResources[i].baseAddr);
            printf("   membankId is:  %d\n", queryScalerCuListRes.cuResources[i].membankId);
            printf("   membankType is:  %d\n", queryScalerCuListRes.cuResources[i].membankType);
            printf("   membankSize is:  0x%lx\n", queryScalerCuListRes.cuResources[i].membankSize);
            printf("   membankBaseAddr is:  0x%lx\n", queryScalerCuListRes.cuResources[i].membankBaseAddr);
            printf("   allocServiceId is:  %lu\n", queryScalerCuListRes.cuResources[i].allocServiceId);
            printf("   poolId is:  %lu\n", queryScalerCuListRes.cuResources[i].poolId);
            printf("   channelLoad is:  %d\n", queryScalerCuListRes.cuResources[i].channelLoad);
        }
    }

    printf("<<<<<<<==  end the xrm allocation query with kernel alias test ===>>>>>>>>\n");

    printf("Test 26-5:  release scaler cu\n");
    if (xrmCuListRelease(ctx, &scalerCuListRes))
        printf("success to release scaler cu list\n");
    else
        printf("fail to release scaler cu list\n");
}

void xrmCheckCuListAvailableNumUsingAliasGranularity1000000Test(xrmContext* ctx) {
    int i, ret;

    printf(
        "<<<<<<<==  start the xrm check cu list available num with kernel alias Granularity 1000000 test "
        "===>>>>>>>>\n");

    printf("Test 27-1: check scaler cu list available num with kernel alias\n");
    if (ctx == NULL) {
        printf("ctx is null, fail to check cu list available num with kernel alias\n");
        return;
    }

    xrmCuListProperty scalerCuListProp;

    memset(&scalerCuListProp, 0, sizeof(scalerCuListProp));

    scalerCuListProp.cuNum = 4;
    scalerCuListProp.sameDevice = false;
    for (i = 0; i < scalerCuListProp.cuNum; i++) {
        strcpy(scalerCuListProp.cuProps[i].kernelName, "");
        strcpy(scalerCuListProp.cuProps[i].kernelAlias, "SCALER_MPSOC");
        scalerCuListProp.cuProps[i].devExcl = false;
        scalerCuListProp.cuProps[i].requestLoad = 450000 << 8;
        scalerCuListProp.cuProps[i].poolId = 0;
    }

    ret = xrmCheckCuListAvailableNum(ctx, &scalerCuListProp);
    if (ret < 0) {
        printf("xrmCheckCuListAvailableNum: fail to check scaler cu list available num with kernel alias\n");
    } else {
        printf("xrmCheckCuListAvailableNum: scaler cu list available num with kernel alias = %d\n", ret);
    }

    printf("Test 27-2: check scaler cu list available num with kernel name\n");
    memset(&scalerCuListProp, 0, sizeof(scalerCuListProp));

    scalerCuListProp.cuNum = 4;
    scalerCuListProp.sameDevice = false;
    for (i = 0; i < scalerCuListProp.cuNum; i++) {
        strcpy(scalerCuListProp.cuProps[i].kernelName, "scaler");
        strcpy(scalerCuListProp.cuProps[i].kernelAlias, "");
        scalerCuListProp.cuProps[i].devExcl = false;
        scalerCuListProp.cuProps[i].requestLoad = 450000 << 8;
        scalerCuListProp.cuProps[i].poolId = 0;
    }

    ret = xrmCheckCuListAvailableNum(ctx, &scalerCuListProp);
    if (ret < 0) {
        printf("xrmCheckCuListAvailableNum: fail to check scaler cu list available num with kernel name\n");
    } else {
        printf("xrmCheckCuListAvailableNum: scaler cu list available num with kernel name = %d\n", ret);
    }

    printf("Test 27-3: check scaler cu list available num with kernel name\n");
    memset(&scalerCuListProp, 0, sizeof(scalerCuListProp));

    scalerCuListProp.cuNum = 7;
    scalerCuListProp.sameDevice = false;
    for (i = 0; i < scalerCuListProp.cuNum; i++) {
        strcpy(scalerCuListProp.cuProps[i].kernelName, "scaler");
        strcpy(scalerCuListProp.cuProps[i].kernelAlias, "");
        scalerCuListProp.cuProps[i].devExcl = false;
        scalerCuListProp.cuProps[i].requestLoad = 650000 << 8;
        scalerCuListProp.cuProps[i].poolId = 0;
    }

    ret = xrmCheckCuListAvailableNum(ctx, &scalerCuListProp);
    if (ret < 0) {
        printf("xrmCheckCuListAvailableNum: fail to check scaler cu list available num with kernel name\n");
    } else {
        printf("xrmCheckCuListAvailableNum: scaler cu list available num with kernel name = %d\n", ret);
    }
    printf("<<<<<<<==  end the xrm check cu list available num with kernel alias test ===>>>>>>>>\n");
}

void xrmCuPoolReserveAllocReleaseRelinquishGranularity1000000Test(xrmContext* ctx) {
    int i;
    int32_t ret;
    uint64_t scalerReservePoolId;
    printf("<<<<<<<==  start the xrm cu reserve alloc release relinquish Granularity 1000000 test ===>>>>>>>>\n");

    printf("Test 28-1: reserve scaler cu pool\n");
    if (ctx == NULL) {
        printf("ctx is null, fail to reserve cu pool\n");
        return;
    }
    // reserve scaler cu
    xrmCuPoolProperty scalerCuPoolProp;

    memset(&scalerCuPoolProp, 0, sizeof(scalerCuPoolProp));

    scalerCuPoolProp.cuListProp.cuNum = 4;
    for (i = 0; i < scalerCuPoolProp.cuListProp.cuNum; i++) {
        strcpy(scalerCuPoolProp.cuListProp.cuProps[i].kernelName, "scaler");
        strcpy(scalerCuPoolProp.cuListProp.cuProps[i].kernelAlias, "");
        scalerCuPoolProp.cuListProp.cuProps[i].devExcl = false;
        scalerCuPoolProp.cuListProp.cuProps[i].requestLoad = 500000 << 8;
    }
    scalerCuPoolProp.cuListNum = 1;
    char uuidStr[64];
    strcpy(uuidStr, "9969c7a56f244caeb6fea631463cf7e5");
    xrmTestHexstrToBin(uuidStr, 2 * sizeof(uuid_t), (char*)&scalerCuPoolProp.xclbinUuid);
    scalerCuPoolProp.xclbinNum = 1;

    ret = xrmCheckCuPoolAvailableNum(ctx, &scalerCuPoolProp);
    if (ret < 0) {
        printf("xrmCheckCuPoolAvailableNum: fail to check scaler cu pool available num\n");
    } else {
        printf("xrmCheckCuPoolAvailableNum: scaler cu pool available num = %d\n", ret);
    }

    scalerReservePoolId = xrmCuPoolReserve(ctx, &scalerCuPoolProp);
    if (scalerReservePoolId == 0) {
        printf("xrmCuPoolReserve: fail to reserve scaler cu pool\n");
    } else {
        printf("xrmCuPoolReserve: reservePoolId = %lu\n", scalerReservePoolId);
    }

    // query the reserve result
    xrmCuPoolResource scalerCuPoolRes;
    memset(&scalerCuPoolRes, 0, sizeof(scalerCuPoolRes));

    printf("Test 28-2: xrmReservationQuery\n");
    ret = xrmReservationQuery(ctx, scalerReservePoolId, &scalerCuPoolRes);
    if (ret != 0) {
        printf("xrmReservationQuery: fail to query reserved scaler cu pool\n");
    } else {
        for (i = 0; i < scalerCuPoolRes.cuNum; i++) {
            printf("query the reserved scaler cu pool: cu %d\n", i);
            printf("   xclbinFileName is:  %s\n", scalerCuPoolRes.cuResources[i].xclbinFileName);
            printf("   kernelPluginFileName is:  %s\n", scalerCuPoolRes.cuResources[i].kernelPluginFileName);
            printf("   kernelName is:  %s\n", scalerCuPoolRes.cuResources[i].kernelName);
            printf("   kernelAlias is:  %s\n", scalerCuPoolRes.cuResources[i].kernelAlias);
            printf("   instanceName is:  %s\n", scalerCuPoolRes.cuResources[i].instanceName);
            printf("   cuName is:  %s\n", scalerCuPoolRes.cuResources[i].cuName);
            printf("   deviceId is:  %d\n", scalerCuPoolRes.cuResources[i].deviceId);
            printf("   cuId is:  %d\n", scalerCuPoolRes.cuResources[i].cuId);
            printf("   cuType is:  %d\n", scalerCuPoolRes.cuResources[i].cuType);
            printf("   baseAddr is:  0x%lx\n", scalerCuPoolRes.cuResources[i].baseAddr);
            printf("   membankId is:  %d\n", scalerCuPoolRes.cuResources[i].membankId);
            printf("   membankType is:  %d\n", scalerCuPoolRes.cuResources[i].membankType);
            printf("   membankSize is:  0x%lx\n", scalerCuPoolRes.cuResources[i].membankSize);
            printf("   membankBaseAddr is:  0x%lx\n", scalerCuPoolRes.cuResources[i].membankBaseAddr);
            printf("   poolId is:  %lu\n", scalerCuPoolRes.cuResources[i].poolId);
        }
    }

    // alloc scaler cu
    xrmCuProperty scalerCuProp;
    xrmCuResource scalerCuRes;
    memset(&scalerCuProp, 0, sizeof(xrmCuProperty));
    memset(&scalerCuRes, 0, sizeof(xrmCuResource));
    strcpy(scalerCuProp.kernelName, "scaler");
    strcpy(scalerCuProp.kernelAlias, "");
    scalerCuProp.devExcl = false;
    scalerCuProp.requestLoad = 450000 << 8;
    scalerCuProp.poolId = scalerReservePoolId;

    printf("Test 28-3: scaler cu prop: kernelName is %s\n", scalerCuProp.kernelName);
    printf("          scaler cu prop: kernelAlias is %s\n", scalerCuProp.kernelAlias);

    printf("Test 28-4: xrmCheckCuAvailableNum in reserve pool\n");
    ret = xrmCheckCuAvailableNum(ctx, &scalerCuProp);
    if (ret < 0) {
        printf("xrmCheckCuAvailableNum: fail to check scaler cu available number\n");
    } else {
        printf("xrmCheckCuAvailableNum: scaler cu available number = %d\n", ret);
    }

    printf("Test 28-5: xrmCuAlloc in reserve pool\n");
    ret = xrmCuAlloc(ctx, &scalerCuProp, &scalerCuRes);
    if (ret != 0) {
        printf("xrmCuAlloc: fail to alloc scaler cu from reservation\n");
    } else {
        printf("Allocated scaler cu from reserve pool: \n");
        printf("   xclbinFileName is:  %s\n", scalerCuRes.xclbinFileName);
        printf("   kernelPluginFileName is:  %s\n", scalerCuRes.kernelPluginFileName);
        printf("   kernelName is:  %s\n", scalerCuRes.kernelName);
        printf("   kernelAlias is:  %s\n", scalerCuRes.kernelAlias);
        printf("   instanceName is:  %s\n", scalerCuRes.instanceName);
        printf("   cuName is:  %s\n", scalerCuRes.cuName);
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

    // alloc scaler cu list
    xrmCuListProperty scalerCuListProp;
    xrmCuListResource scalerCuListRes;
    memset(&scalerCuListProp, 0, sizeof(scalerCuListProp));
    memset(&scalerCuListRes, 0, sizeof(scalerCuListRes));

    scalerCuListProp.cuNum = 4;
    for (i = 0; i < scalerCuListProp.cuNum; i++) {
        strcpy(scalerCuListProp.cuProps[i].kernelName, "scaler");
        strcpy(scalerCuListProp.cuProps[i].kernelAlias, "");
        scalerCuListProp.cuProps[i].devExcl = false;
        scalerCuListProp.cuProps[i].requestLoad = 400000 << 8;
        scalerCuListProp.cuProps[i].poolId = scalerReservePoolId;
    }

    printf("Test 28-6: xrmCheckCuListAvailableNum in reserve pool\n");
    ret = xrmCheckCuListAvailableNum(ctx, &scalerCuListProp);
    if (ret < 0) {
        printf("xrmCheckCuListAvailableNum: fail to check scaler cu list available number\n");
    } else {
        printf("xrmCheckCuListAvailableNum: scaler cu list available number = %d\n", ret);
    }

    printf("Test 28-7: xrmCuListAlloc in reserve pool\n");
    ret = xrmCuListAlloc(ctx, &scalerCuListProp, &scalerCuListRes);
    if (ret != 0) {
        printf("xrmCuListAlloc: fail to alloc scaler cu list from reservation\n");
    } else {
        for (i = 0; i < scalerCuListRes.cuNum; i++) {
            printf("Allocated scaler cu list from reserve pool: cu %d\n", i);
            printf("   xclbinFileName is:  %s\n", scalerCuListRes.cuResources[i].xclbinFileName);
            printf("   kernelPluginFileName is:  %s\n", scalerCuListRes.cuResources[i].kernelPluginFileName);
            printf("   kernelName is:  %s\n", scalerCuListRes.cuResources[i].kernelName);
            printf("   kernelAlias is:  %s\n", scalerCuListRes.cuResources[i].kernelAlias);
            printf("   instanceName is:  %s\n", scalerCuListRes.cuResources[i].instanceName);
            printf("   cuName is:  %s\n", scalerCuListRes.cuResources[i].cuName);
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

    printf("Test 28-8:   release scaler cu\n");
    if (xrmCuRelease(ctx, &scalerCuRes))
        printf("success to release scaler cu\n");
    else
        printf("fail to release scaler cu\n");

    printf("Test 28-9:   release scaler cu list\n");
    if (xrmCuListRelease(ctx, &scalerCuListRes))
        printf("success to release scaler cu list\n");
    else
        printf("fail to release scaler cu list\n");

    printf("Test 28-10:  relinquish scaler cu pool\n");
    if (xrmCuPoolRelinquish(ctx, scalerReservePoolId))
        printf("success to relinquish scaler cu pool\n");
    else
        printf("fail to relinquish encoder cu pool\n");
}

void xrmUdfCuGroupGranularity1000000Test(xrmContext* ctx) {
    int32_t i, ret;
    printf("<<<<<<<==  start the xrm user defined cu group Granularity 1000000 test ===>>>>>>>>\n");
    if (ctx == NULL) {
        printf("ctx is null, fail to run user defined cu group test\n");
        return;
    }

    printf("Test 29-1: test the user defined cu group declaration function\n");
    char udfCuGroupName[XRM_MAX_NAME_LEN];
    xrmUdfCuGroupProperty* udfCuGroupProp = (xrmUdfCuGroupProperty*)malloc(sizeof(xrmUdfCuGroupProperty));

    /* declare user defined cu group from same device */
    printf("Test 29-2: user defined cu group from same device declaration\n");
    memset(udfCuGroupProp, 0, sizeof(xrmUdfCuGroupProperty));
    strcpy(udfCuGroupName, "udfCuGroupSameDeviceGranularity1000000");
    xrmUdfCuListProperty* udfCuListProp;
    xrmUdfCuProperty* udfCuProp;
    udfCuGroupProp->optionUdfCuListNum = 2;
    /* define the first option cu list */
    udfCuListProp = &udfCuGroupProp->optionUdfCuListProps[0];
    udfCuListProp->cuNum = 5;
    udfCuListProp->sameDevice = true;
    for (int32_t cuIdx = 0; cuIdx < udfCuListProp->cuNum; cuIdx++) {
        udfCuProp = &udfCuListProp->udfCuProps[cuIdx];
        strcpy(udfCuProp->cuName, "encoder:encoder_1");
        udfCuProp->devExcl = false;
        udfCuProp->requestLoad = 300000 << 8;
    }
    /* define the second option cu list */
    udfCuListProp = &udfCuGroupProp->optionUdfCuListProps[1];
    udfCuListProp->cuNum = 5;
    udfCuListProp->sameDevice = true;
    for (int32_t cuIdx = 0; cuIdx < udfCuListProp->cuNum; cuIdx++) {
        udfCuProp = &udfCuListProp->udfCuProps[cuIdx];
        strcpy(udfCuProp->cuName, "decoder:decoder_1");
        udfCuProp->devExcl = false;
        udfCuProp->requestLoad = 200000 << 8;
    }
    ret = xrmUdfCuGroupDeclare(ctx, udfCuGroupProp, udfCuGroupName);
    if (ret == XRM_SUCCESS)
        printf("xrmUdfCuGroupDeclare(): user defined cu group from same device declaration success\n");
    else
        printf("xrmUdfCuGroupDeclare(): user defined cu group from same device declaration fail\n");

    /* declare user defined cu group from any device */
    printf("Test 29-3: user defined cu group from any device declaration\n");
    memset(udfCuGroupProp, 0, sizeof(xrmUdfCuGroupProperty));
    strcpy(udfCuGroupName, "udfCuGroupAnyDeviceGranularity1000000");
    udfCuGroupProp->optionUdfCuListNum = 2;
    udfCuListProp = &udfCuGroupProp->optionUdfCuListProps[0];
    udfCuListProp->cuNum = 5;
    udfCuListProp->sameDevice = false;
    for (int32_t cuIdx = 0; cuIdx < udfCuListProp->cuNum; cuIdx++) {
        udfCuProp = &udfCuListProp->udfCuProps[cuIdx];
        strcpy(udfCuProp->cuName, "encoder:encoder_1");
        udfCuProp->devExcl = false;
        udfCuProp->requestLoad = 300000 << 8;
    }
    udfCuListProp = &udfCuGroupProp->optionUdfCuListProps[1];
    udfCuListProp->cuNum = 5;
    udfCuListProp->sameDevice = false;
    for (int32_t cuIdx = 0; cuIdx < udfCuListProp->cuNum; cuIdx++) {
        udfCuProp = &udfCuListProp->udfCuProps[cuIdx];
        strcpy(udfCuProp->cuName, "decoder:decoder_1");
        udfCuProp->devExcl = false;
        udfCuProp->requestLoad = 200000 << 8;
    }
    ret = xrmUdfCuGroupDeclare(ctx, udfCuGroupProp, udfCuGroupName);
    if (ret == XRM_SUCCESS)
        printf("xrmUdfCuGroupDeclare(): user defined cu group from any device declaration success\n");
    else
        printf("xrmUdfCuGroupDeclare(): user defined cu group from any device declaration fail\n");

    printf("Test 29-4: check user defined cu group (same device) available num\n");
    xrmCuGroupResource cuGroupRes;
    xrmCuGroupProperty cuGroupProp;
    memset(&cuGroupProp, 0, sizeof(xrmCuGroupProperty));
    strcpy(cuGroupProp.udfCuGroupName, "udfCuGroupSameDeviceGranularity1000000");
    cuGroupProp.poolId = 0;
    ret = xrmCheckCuGroupAvailableNum(ctx, &cuGroupProp);
    if (ret < 0) {
        printf("xrmCheckCuGroupAvaiableNum: fail to check user defined cu group (same device) available num\n");
    } else {
        printf("xrmCheckCuGroupAvaiableNum: user defined cu group (same device) available num = %d\n", ret);
    }

    printf("Test 29-5: alloc user defined cu group (same device)\n");
    memset(&cuGroupRes, 0, sizeof(xrmCuGroupResource));
    ret = xrmCuGroupAlloc(ctx, &cuGroupProp, &cuGroupRes);
    if (ret != XRM_SUCCESS) {
        printf("xrmCuGroupAlloc: fail to alloc user defined cu group (same device)\n");
    } else {
        for (i = 0; i < cuGroupRes.cuNum; i++) {
            printf("Allocated user defined cu group (same device): cu %d\n", i);
            printf("   xclbinFileName is:  %s\n", cuGroupRes.cuResources[i].xclbinFileName);
            printf("   kernelPluginFileName is:  %s\n", cuGroupRes.cuResources[i].kernelPluginFileName);
            printf("   kernelName is:  %s\n", cuGroupRes.cuResources[i].kernelName);
            printf("   kernelAlias is:  %s\n", cuGroupRes.cuResources[i].kernelAlias);
            printf("   instanceName is:  %s\n", cuGroupRes.cuResources[i].instanceName);
            printf("   cuName is:  %s\n", cuGroupRes.cuResources[i].cuName);
            printf("   deviceId is:  %d\n", cuGroupRes.cuResources[i].deviceId);
            printf("   cuId is:  %d\n", cuGroupRes.cuResources[i].cuId);
            printf("   channelId is:  %d\n", cuGroupRes.cuResources[i].channelId);
            printf("   cuType is:  %d\n", cuGroupRes.cuResources[i].cuType);
            printf("   baseAddr is:  0x%lx\n", cuGroupRes.cuResources[i].baseAddr);
            printf("   membankId is:  %d\n", cuGroupRes.cuResources[i].membankId);
            printf("   membankType is:  %d\n", cuGroupRes.cuResources[i].membankType);
            printf("   membankSize is:  0x%lx\n", cuGroupRes.cuResources[i].membankSize);
            printf("   membankBaseAddr is:  0x%lx\n", cuGroupRes.cuResources[i].membankBaseAddr);
            printf("   allocServiceId is:  %lu\n", cuGroupRes.cuResources[i].allocServiceId);
            printf("   poolId is:  %lu\n", cuGroupRes.cuResources[i].poolId);
            printf("   channelLoad is:  %d\n", cuGroupRes.cuResources[i].channelLoad);
        }
    }

    printf("Test 29-6: release user defined cu group (same device)\n");
    if (xrmCuGroupRelease(ctx, &cuGroupRes))
        printf("success to release user defined cu group (same device)\n");
    else
        printf("fail to release user defined cu group (same device)\n");

    printf("Test 29-7: check user defined cu group (any device) available num\n");
    memset(&cuGroupProp, 0, sizeof(xrmCuGroupProperty));
    strcpy(cuGroupProp.udfCuGroupName, "udfCuGroupAnyDeviceGranularity1000000");
    cuGroupProp.poolId = 0;
    ret = xrmCheckCuGroupAvailableNum(ctx, &cuGroupProp);
    if (ret < 0) {
        printf("xrmCheckCuGroupAvaiableNum: fail to check user defined cu group (any device) available num\n");
    } else {
        printf("xrmCheckCuGroupAvaiableNum: user defined cu group (any device) available num = %d\n", ret);
    }

    printf("Test 29-8: alloc user defined cu group (any device)\n");
    memset(&cuGroupRes, 0, sizeof(xrmCuGroupResource));
    ret = xrmCuGroupAlloc(ctx, &cuGroupProp, &cuGroupRes);
    if (ret != XRM_SUCCESS) {
        printf("xrmCuGroupAlloc: fail to alloc user defined cu group (any device)\n");
    } else {
        for (i = 0; i < cuGroupRes.cuNum; i++) {
            printf("Allocated user defined cu group (any device): cu %d\n", i);
            printf("   xclbinFileName is:  %s\n", cuGroupRes.cuResources[i].xclbinFileName);
            printf("   kernelPluginFileName is:  %s\n", cuGroupRes.cuResources[i].kernelPluginFileName);
            printf("   kernelName is:  %s\n", cuGroupRes.cuResources[i].kernelName);
            printf("   kernelAlias is:  %s\n", cuGroupRes.cuResources[i].kernelAlias);
            printf("   instanceName is:  %s\n", cuGroupRes.cuResources[i].instanceName);
            printf("   cuName is:  %s\n", cuGroupRes.cuResources[i].cuName);
            printf("   deviceId is:  %d\n", cuGroupRes.cuResources[i].deviceId);
            printf("   cuId is:  %d\n", cuGroupRes.cuResources[i].cuId);
            printf("   channelId is:  %d\n", cuGroupRes.cuResources[i].channelId);
            printf("   cuType is:  %d\n", cuGroupRes.cuResources[i].cuType);
            printf("   baseAddr is:  0x%lx\n", cuGroupRes.cuResources[i].baseAddr);
            printf("   membankId is:  %d\n", cuGroupRes.cuResources[i].membankId);
            printf("   membankType is:  %d\n", cuGroupRes.cuResources[i].membankType);
            printf("   membankSize is:  0x%lx\n", cuGroupRes.cuResources[i].membankSize);
            printf("   membankBaseAddr is:  0x%lx\n", cuGroupRes.cuResources[i].membankBaseAddr);
            printf("   allocServiceId is:  %lu\n", cuGroupRes.cuResources[i].allocServiceId);
            printf("   poolId is:  %lu\n", cuGroupRes.cuResources[i].poolId);
            printf("   channelLoad is:  %d\n", cuGroupRes.cuResources[i].channelLoad);
        }
    }

    printf("Test 29-9: release user defined cu group (any device)\n");
    if (xrmCuGroupRelease(ctx, &cuGroupRes))
        printf("success to release user defined cu group (any device)\n");
    else
        printf("fail to release user defined cu group (any device)\n");

    printf("Test 29-10: user defined cu group from same device undeclaration\n");
    strcpy(udfCuGroupName, "udfCuGroupSameDeviceGranularity1000000");
    ret = xrmUdfCuGroupUndeclare(ctx, udfCuGroupName);
    if (ret == XRM_SUCCESS)
        printf("xrmUdfCuGroupUndeclare(): user defined cu group from same device undeclaration success\n");
    else
        printf("xrmUdfCuGroupUndeclare(): user defined cu group from same device undeclaration fail\n");

    printf("Test 29-11: user defined cu group from any device undeclaration\n");
    strcpy(udfCuGroupName, "udfCuGroupAnyDeviceGranularity1000000");
    ret = xrmUdfCuGroupUndeclare(ctx, udfCuGroupName);
    if (ret == XRM_SUCCESS)
        printf("xrmUdfCuGroupUndeclare(): user defined cu group from any device undeclaration success\n");
    else
        printf("xrmUdfCuGroupUndeclare(): user defined cu group from any device undeclaration fail\n");

    free(udfCuGroupProp);

    printf("<<<<<<<==  end the xrm user defined cu group test ===>>>>>>>>\n");
}

void xrmCuAllocWithLoadGranularity1000000Test(xrmContext* ctx) {
    int32_t ret;
    printf("<<<<<<<==  start the xrm cu alloc with load Granularity 1000000 test ===>>>>>>>>\n");
    if (ctx == NULL) {
        printf("ctx is null, fail to alloc cu list\n");
        return;
    }

    // alloc scaler cu
    xrmCuProperty scalerCuProp;
    xrmCuResource scalerCuRes;

    memset(&scalerCuProp, 0, sizeof(xrmCuProperty));
    memset(&scalerCuRes, 0, sizeof(xrmCuResource));
    strcpy(scalerCuProp.kernelName, "scaler");
    strcpy(scalerCuProp.kernelAlias, "");
    scalerCuProp.devExcl = false;
    scalerCuProp.requestLoad = 450000 << 8;
    scalerCuProp.poolId = 0;

    printf("Test 30-1: alloc scaler with load\n");
    printf("    scaler cu prop: kernelName is %s\n", scalerCuProp.kernelName);
    printf("    scaler cu prop: kernelAlias is %s\n", scalerCuProp.kernelAlias);

    char xclbinFileName[XRM_MAX_PATH_NAME_LEN];
    strcpy(xclbinFileName, "/tmp/xclbins/test_xrm_u30.xclbin");

    printf("Test 30-2: Alloc scaler cu\n");
    ret = xrmCuAllocWithLoad(ctx, &scalerCuProp, xclbinFileName, &scalerCuRes);
    if (ret != 0) {
        printf("xrmCuAllocWithLoad: fail to alloc scaler cu\n");
    } else {
        printf("Allocated scaler cu with load: \n");
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

    printf("Test 30-3:  release scaler cu\n");
    if (xrmCuRelease(ctx, &scalerCuRes))
        printf("success to  release scaler cu\n");
    else
        printf("fail to release scaler cu\n");

    printf("<<<<<<<==  end the xrm allocation with load test ===>>>>>>>>\n");
}

void xrmCuBlockingAllocReleaseGranularity1000000Test(xrmContext* ctx) {
    int32_t ret;
    printf("<<<<<<<==  start the xrm blocing allocation Granularity 1000000 test ===>>>>>>>>\n");
    if (ctx == NULL) {
        printf("ctx is null, fail to do cu blocking alloc test\n");
        return;
    }

    // alloc scaler cu
    xrmCuProperty scalerCuProp;
    xrmCuResource scalerCuRes;

    memset(&scalerCuProp, 0, sizeof(xrmCuProperty));
    memset(&scalerCuRes, 0, sizeof(xrmCuResource));
    strcpy(scalerCuProp.kernelName, "scaler");
    strcpy(scalerCuProp.kernelAlias, "");
    scalerCuProp.devExcl = false;
    scalerCuProp.requestLoad = 450000 << 8;
    scalerCuProp.poolId = 0;

    uint64_t interval = 200000; // 200000 useconds (200 ms)

    printf("Test 31-1: Using blocking API to alloc scaler cu\n");
    printf("    scaler cu prop: kernelName is %s\n", scalerCuProp.kernelName);
    printf("    scaler cu prop: kernelAlias is %s\n", scalerCuProp.kernelAlias);
    printf("NOTE: The test will be blocked here if requested cu is NOT available on system.\n");
    printf("      You can safely stop the test with Ctrl+c\n");
    ret = xrmCuBlockingAlloc(ctx, &scalerCuProp, interval, &scalerCuRes);
    if (ret != 0) {
        printf("xrmCuBlockingAlloc: fail to alloc scaler cu, ret is %d\n", ret);
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

    printf("Test 31-2:  release scaler cu\n");
    if (xrmCuRelease(ctx, &scalerCuRes))
        printf("success to  release scaler cu\n");
    else
        printf("fail to release scaler cu\n");

    // alloc cal cu
    xrmCuProperty addCuProp;
    xrmCuResource addCuRes;

    memset(&addCuProp, 0, sizeof(xrmCuProperty));
    memset(&addCuRes, 0, sizeof(xrmCuResource));
    strcpy(addCuProp.kernelName, "krnl_vadd");
    strcpy(addCuProp.kernelAlias, "");
    addCuProp.devExcl = false;
    addCuProp.requestLoad = 450000 << 8;
    addCuProp.poolId = 0;

    interval = 200000; // 200000 useconds (200 ms)

    printf("Test 31-3: Using blocking API to alloc add cu\n");
    printf("    add cu prop: kernelName is %s\n", addCuProp.kernelName);
    printf("    add cu prop: kernelAlias is %s\n", addCuProp.kernelAlias);
    printf("NOTE: The test will be blocked here if requested cu is NOT available on system.\n");
    printf("      You can safely stop the test with Ctrl+c\n");
    ret = xrmCuBlockingAlloc(ctx, &addCuProp, interval, &addCuRes);
    if (ret != 0) {
        printf("xrmCuBlockingAlloc: fail to alloc add cu, ret is %d\n", ret);
    } else {
        printf("Allocated add cu: \n");
        printf("   xclbinFileName is:  %s\n", addCuRes.xclbinFileName);
        printf("   kernelPluginFileName is:  %s\n", addCuRes.kernelPluginFileName);
        printf("   kernelName is:  %s\n", addCuRes.kernelName);
        printf("   instanceName is:  %s\n", addCuRes.instanceName);
        printf("   cuName is:  %s\n", addCuRes.cuName);
        printf("   kernelAlias is:  %s\n", addCuRes.kernelAlias);
        printf("   deviceId is:  %d\n", addCuRes.deviceId);
        printf("   cuId is:  %d\n", addCuRes.cuId);
        printf("   channelId is:  %d\n", addCuRes.channelId);
        printf("   cuType is:  %d\n", addCuRes.cuType);
        printf("   baseAddr is:  0x%lx\n", addCuRes.baseAddr);
        printf("   membankId is:  %d\n", addCuRes.membankId);
        printf("   membankType is:  %d\n", addCuRes.membankType);
        printf("   membankSize is:  0x%lx\n", addCuRes.membankSize);
        printf("   membankBaseAddr is:  0x%lx\n", addCuRes.membankBaseAddr);
        printf("   allocServiceId is:  %lu\n", addCuRes.allocServiceId);
        printf("   poolId is:  %lu\n", addCuRes.poolId);
        printf("   channelLoad is:  %d\n", addCuRes.channelLoad);
    }

    printf("Test 31-4:  release add cu\n");
    if (xrmCuRelease(ctx, &addCuRes))
        printf("success to  release add cu\n");
    else
        printf("fail to release add cu\n");

    printf("<<<<<<<==  end the xrm cu blocking allocation test ===>>>>>>>>\n");
}

void xrmCuListBlockingAllocReleaseGranularity1000000Test(xrmContext* ctx) {
    int i;
    int32_t ret;
    printf("<<<<<<<==  start the xrm cu list blocking allocation Granularity 1000000 test ===>>>>>>>>\n");

    printf("Test 32-1: Using blocking API to alloc scaler cu list\n");
    printf("NOTE: The test will be blocked here if requested cu list is NOT available on system.\n");
    printf("      You can safely stop the test with Ctrl+c\n");
    if (ctx == NULL) {
        printf("ctx is null, fail to do blocking alloc cu list\n");
        return;
    }
    // alloc scaler cu
    xrmCuListProperty scalerCuListProp;
    xrmCuListResource scalerCuListRes;

    memset(&scalerCuListProp, 0, sizeof(xrmCuListProperty));
    memset(&scalerCuListRes, 0, sizeof(xrmCuListResource));

    scalerCuListProp.cuNum = 4;
    for (i = 0; i < scalerCuListProp.cuNum; i++) {
        strcpy(scalerCuListProp.cuProps[i].kernelName, "scaler");
        strcpy(scalerCuListProp.cuProps[i].kernelAlias, "");
        scalerCuListProp.cuProps[i].devExcl = false;
        scalerCuListProp.cuProps[i].requestLoad = 450000 << 8;
        scalerCuListProp.cuProps[i].poolId = 0;
    }

    uint64_t interval = 200000; // 200000 useconds (200 ms)

    ret = xrmCuListBlockingAlloc(ctx, &scalerCuListProp, interval, &scalerCuListRes);
    if (ret != 0) {
        printf("xrmCuListBlockingAlloc: fail to alloc scaler cu list\n");
    } else {
        for (i = 0; i < scalerCuListRes.cuNum; i++) {
            printf("Blocking allocated scaler cu list: cu %d\n", i);
            printf("   xclbinFileName is:  %s\n", scalerCuListRes.cuResources[i].xclbinFileName);
            printf("   kernelPluginFileName is:  %s\n", scalerCuListRes.cuResources[i].kernelPluginFileName);
            printf("   kernelName is:  %s\n", scalerCuListRes.cuResources[i].kernelName);
            printf("   kernelAlias is:  %s\n", scalerCuListRes.cuResources[i].kernelAlias);
            printf("   instanceName is:  %s\n", scalerCuListRes.cuResources[i].instanceName);
            printf("   cuName is:  %s\n", scalerCuListRes.cuResources[i].cuName);
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

    printf("Test 32-2:   release scaler cu list\n");
    if (xrmCuListRelease(ctx, &scalerCuListRes))
        printf("success to release scaler cu list\n");
    else
        printf("fail to release scaler cu list\n");

    // alloc calculate cu
    xrmCuListProperty calculateCuListProp;
    xrmCuListResource calculateCuListRes;

    memset(&calculateCuListProp, 0, sizeof(xrmCuListProperty));
    memset(&calculateCuListRes, 0, sizeof(xrmCuListResource));

    calculateCuListProp.cuNum = 2;
    strcpy(calculateCuListProp.cuProps[0].kernelName, "krnl_vadd");
    strcpy(calculateCuListProp.cuProps[0].kernelAlias, "");
    calculateCuListProp.cuProps[0].devExcl = false;
    calculateCuListProp.cuProps[0].requestLoad = 450000 << 8;
    calculateCuListProp.cuProps[0].poolId = 0;

    strcpy(calculateCuListProp.cuProps[1].kernelName, "krnl_vsub");
    strcpy(calculateCuListProp.cuProps[1].kernelAlias, "");
    calculateCuListProp.cuProps[1].devExcl = false;
    calculateCuListProp.cuProps[1].requestLoad = 450000 < 8;
    calculateCuListProp.cuProps[1].poolId = 0;

    interval = 200000; // 200000 useconds (200 ms)

    ret = xrmCuListBlockingAlloc(ctx, &calculateCuListProp, interval, &calculateCuListRes);
    if (ret != 0) {
        printf("xrmCuListBlockingAlloc: fail to alloc calculate cu list\n");
    } else {
        for (i = 0; i < calculateCuListRes.cuNum; i++) {
            printf("Blocking allocated calculate cu list: cu %d\n", i);
            printf("   xclbinFileName is:  %s\n", calculateCuListRes.cuResources[i].xclbinFileName);
            printf("   kernelPluginFileName is:  %s\n", calculateCuListRes.cuResources[i].kernelPluginFileName);
            printf("   kernelName is:  %s\n", calculateCuListRes.cuResources[i].kernelName);
            printf("   kernelAlias is:  %s\n", calculateCuListRes.cuResources[i].kernelAlias);
            printf("   instanceName is:  %s\n", calculateCuListRes.cuResources[i].instanceName);
            printf("   cuName is:  %s\n", calculateCuListRes.cuResources[i].cuName);
            printf("   deviceId is:  %d\n", calculateCuListRes.cuResources[i].deviceId);
            printf("   cuId is:  %d\n", calculateCuListRes.cuResources[i].cuId);
            printf("   channelId is:  %d\n", calculateCuListRes.cuResources[i].channelId);
            printf("   cuType is:  %d\n", calculateCuListRes.cuResources[i].cuType);
            printf("   baseAddr is:  0x%lx\n", calculateCuListRes.cuResources[i].baseAddr);
            printf("   membankId is:  %d\n", calculateCuListRes.cuResources[i].membankId);
            printf("   membankType is:  %d\n", calculateCuListRes.cuResources[i].membankType);
            printf("   membankSize is:  0x%lx\n", calculateCuListRes.cuResources[i].membankSize);
            printf("   membankBaseAddr is:  0x%lx\n", calculateCuListRes.cuResources[i].membankBaseAddr);
            printf("   allocServiceId is:  %lu\n", calculateCuListRes.cuResources[i].allocServiceId);
            printf("   poolId is:  %lu\n", calculateCuListRes.cuResources[i].poolId);
            printf("   channelLoad is:  %d\n", calculateCuListRes.cuResources[i].channelLoad);
        }
    }

    printf("Test 32-3:   release calculate cu list\n");
    if (xrmCuListRelease(ctx, &calculateCuListRes))
        printf("success to release calculate cu list\n");
    else
        printf("fail to release calculate cu list\n");
    printf("<<<<<<<==  end the xrm cu list blocking allocation test ===>>>>>>>>\n");
}

void xrmCuGroupBlockingAllocReleaseGranularity1000000Test(xrmContext* ctx) {
    int32_t i, ret;
    printf(
        "<<<<<<<==  start the xrm user defined cu group blocking alloc release Granularity 1000000 test ===>>>>>>>>\n");
    if (ctx == NULL) {
        printf("ctx is null, fail to run user defined cu group blocking alloc release test\n");
        return;
    }

    printf("Test 33-1: test the user defined cu group declaration function\n");
    char udfCuGroupName[XRM_MAX_NAME_LEN];
    xrmUdfCuGroupProperty* udfCuGroupProp = (xrmUdfCuGroupProperty*)malloc(sizeof(xrmUdfCuGroupProperty));

    /* declare user defined cu group from same device */
    printf("Test 33-2: user defined cu group from same device declaration\n");
    memset(udfCuGroupProp, 0, sizeof(xrmUdfCuGroupProperty));
    strcpy(udfCuGroupName, "udfCuGroupSameDeviceGranularity1000000");
    xrmUdfCuListProperty* udfCuListProp;
    xrmUdfCuProperty* udfCuProp;
    udfCuGroupProp->optionUdfCuListNum = 2;
    /* define the first option cu list */
    udfCuListProp = &udfCuGroupProp->optionUdfCuListProps[0];
    udfCuListProp->cuNum = 5;
    udfCuListProp->sameDevice = true;
    for (int32_t cuIdx = 0; cuIdx < udfCuListProp->cuNum; cuIdx++) {
        udfCuProp = &udfCuListProp->udfCuProps[cuIdx];
        strcpy(udfCuProp->cuName, "encoder:encoder_1");
        udfCuProp->devExcl = false;
        udfCuProp->requestLoad = 300000 << 8;
    }
    /* define the second option cu list */
    udfCuListProp = &udfCuGroupProp->optionUdfCuListProps[1];
    udfCuListProp->cuNum = 5;
    udfCuListProp->sameDevice = true;
    for (int32_t cuIdx = 0; cuIdx < udfCuListProp->cuNum; cuIdx++) {
        udfCuProp = &udfCuListProp->udfCuProps[cuIdx];
        strcpy(udfCuProp->cuName, "scaler:scaler_1");
        udfCuProp->devExcl = false;
        udfCuProp->requestLoad = 200000 << 8;
    }
    ret = xrmUdfCuGroupDeclare(ctx, udfCuGroupProp, udfCuGroupName);
    if (ret == XRM_SUCCESS)
        printf("xrmUdfCuGroupDeclare(): user defined cu group from same device declaration success\n");
    else
        printf("xrmUdfCuGroupDeclare(): user defined cu group from same device declaration fail\n");

    printf("Test 33-3: check user defined cu group (same device) available num\n");
    xrmCuGroupResource cuGroupRes;
    xrmCuGroupProperty cuGroupProp;
    memset(&cuGroupProp, 0, sizeof(xrmCuGroupProperty));
    strcpy(cuGroupProp.udfCuGroupName, "udfCuGroupSameDeviceGranularity1000000");
    cuGroupProp.poolId = 0;
    ret = xrmCheckCuGroupAvailableNum(ctx, &cuGroupProp);
    if (ret < 0) {
        printf("xrmCheckCuGroupAvaiableNum: fail to check user defined cu group (same device) available num\n");
    } else {
        printf("xrmCheckCuGroupAvaiableNum: user defined cu group (same device) available num = %d\n", ret);
    }

    printf("Test 33-4: Using blocking API to alloc user defined cu group (same device)\n");
    printf("NOTE: The test will be blocked here if requested cu group is NOT available on system.\n");
    printf("      You can safely stop the test with Ctrl+c\n");
    memset(&cuGroupRes, 0, sizeof(xrmCuGroupResource));

    uint64_t interval = 0; // to use the XRM default interval

    ret = xrmCuGroupBlockingAlloc(ctx, &cuGroupProp, interval, &cuGroupRes);
    if (ret != XRM_SUCCESS) {
        printf("xrmCuGroupBlockingAlloc: fail to alloc user defined cu group (same device)\n");
    } else {
        for (i = 0; i < cuGroupRes.cuNum; i++) {
            printf("Blocking allocated user defined cu group (same device): cu %d\n", i);
            printf("   xclbinFileName is:  %s\n", cuGroupRes.cuResources[i].xclbinFileName);
            printf("   kernelPluginFileName is:  %s\n", cuGroupRes.cuResources[i].kernelPluginFileName);
            printf("   kernelName is:  %s\n", cuGroupRes.cuResources[i].kernelName);
            printf("   kernelAlias is:  %s\n", cuGroupRes.cuResources[i].kernelAlias);
            printf("   instanceName is:  %s\n", cuGroupRes.cuResources[i].instanceName);
            printf("   cuName is:  %s\n", cuGroupRes.cuResources[i].cuName);
            printf("   deviceId is:  %d\n", cuGroupRes.cuResources[i].deviceId);
            printf("   cuId is:  %d\n", cuGroupRes.cuResources[i].cuId);
            printf("   channelId is:  %d\n", cuGroupRes.cuResources[i].channelId);
            printf("   cuType is:  %d\n", cuGroupRes.cuResources[i].cuType);
            printf("   baseAddr is:  0x%lx\n", cuGroupRes.cuResources[i].baseAddr);
            printf("   membankId is:  %d\n", cuGroupRes.cuResources[i].membankId);
            printf("   membankType is:  %d\n", cuGroupRes.cuResources[i].membankType);
            printf("   membankSize is:  0x%lx\n", cuGroupRes.cuResources[i].membankSize);
            printf("   membankBaseAddr is:  0x%lx\n", cuGroupRes.cuResources[i].membankBaseAddr);
            printf("   allocServiceId is:  %lu\n", cuGroupRes.cuResources[i].allocServiceId);
            printf("   poolId is:  %lu\n", cuGroupRes.cuResources[i].poolId);
            printf("   channelLoad is:  %d\n", cuGroupRes.cuResources[i].channelLoad);
        }
    }

    printf("Test 33-5: release user defined cu group (same device)\n");
    if (xrmCuGroupRelease(ctx, &cuGroupRes))
        printf("success to release user defined cu group (same device)\n");
    else
        printf("fail to release user defined cu group (same device)\n");

    printf("Test 33-6: user defined cu group from same device undeclaration\n");
    strcpy(udfCuGroupName, "udfCuGroupSameDeviceGranularity1000000");
    ret = xrmUdfCuGroupUndeclare(ctx, udfCuGroupName);
    if (ret == XRM_SUCCESS)
        printf("xrmUdfCuGroupUndeclare(): user defined cu group from same device undeclaration success\n");
    else
        printf("xrmUdfCuGroupUndeclare(): user defined cu group from same device undeclaration fail\n");

    free(udfCuGroupProp);

    printf("<<<<<<<==  end the xrm user defined cu group blocking alloc release test ===>>>>>>>>\n");
}

void xrmCuAllocFromDevReleaseGranularity1000000Test(xrmContext* ctx) {
    int32_t ret;
    printf("<<<<<<<==  start the xrm allocation from specified device Granularity 1000000 test ===>>>>>>>>\n");
    if (ctx == NULL) {
        printf("ctx is null, fail to do cu alloc from specified device test\n");
        return;
    }

    // alloc scaler cu
    xrmCuProperty scalerCuProp;
    xrmCuResource scalerCuRes;
    int32_t deviceId;

    // alloc scaler cu from device 0
    memset(&scalerCuProp, 0, sizeof(xrmCuProperty));
    memset(&scalerCuRes, 0, sizeof(xrmCuResource));
    strcpy(scalerCuProp.kernelName, "scaler");
    strcpy(scalerCuProp.kernelAlias, "");
    scalerCuProp.devExcl = false;
    scalerCuProp.requestLoad = 450000 << 8;
    scalerCuProp.poolId = 0;
    deviceId = 0;

    printf("Test 34-1: Alloc scaler cu from device %d\n", deviceId);
    ret = xrmCuAllocFromDev(ctx, deviceId, &scalerCuProp, &scalerCuRes);
    if (ret != 0) {
        printf("xrmCuAlloc: fail to alloc scaler cu from device %d\n", deviceId);
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

    printf("Test 34-2:  release scaler cu\n");
    if (xrmCuRelease(ctx, &scalerCuRes))
        printf("success to  release scaler cu\n");
    else
        printf("fail to release scaler cu\n");

    // alloc scaler cu from device 1
    memset(&scalerCuProp, 0, sizeof(xrmCuProperty));
    memset(&scalerCuRes, 0, sizeof(xrmCuResource));
    strcpy(scalerCuProp.kernelName, "");
    strcpy(scalerCuProp.kernelAlias, "SCALER_MPSOC");
    scalerCuProp.devExcl = false;
    scalerCuProp.requestLoad = 550000 << 8;
    scalerCuProp.poolId = 0;
    deviceId = 1;

    printf("Test 34-3: Alloc scaler cu from device %d\n", deviceId);
    ret = xrmCuAllocFromDev(ctx, deviceId, &scalerCuProp, &scalerCuRes);
    if (ret != 0) {
        printf("xrmCuAlloc: fail to alloc scaler cu from device %d\n", deviceId);
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

    printf("Test 34-4:  release scaler cu\n");
    if (xrmCuRelease(ctx, &scalerCuRes))
        printf("success to  release scaler cu\n");
    else
        printf("fail to release scaler cu\n");
    printf("<<<<<<<==  end the xrm allocation from specified device test ===>>>>>>>>\n");
}

void xrmConcurrentContextTest(int32_t numContext) {
    printf("<<<<<<<==  Start the xrm context test ===>>>>>>>>\n\n");
    xrmContext* ctx;
    ctx = malloc(numContext * sizeof(xrmContext*));
    int i;
    for (i = 0; i < numContext; i++) {
        printf("Test 0-01: create context round %d\n", (i + 1));
        ctx[i] = (xrmContext*)xrmCreateContext(XRM_API_VERSION_1);
        if (ctx[i] == NULL) {
            printf("Test 0-01: create context failed\n");
            break;
        } else {
            printf("Test 0-01: create context success\n");
        }
    }

    for (i = 0; i < numContext; i++) {
        printf("Test 0-02: destory context round %d\n", (i + 1));
        if (ctx[i] == NULL) {
            break;
        }
        printf("Test 0-02: destroy context\n");
        if (xrmDestroyContext(ctx[i]) != XRM_SUCCESS)
            printf("Test 0-02: destroy context failed\n");
        else
            printf("Test 0-02: destroy context success\n");
    }
    free(ctx);

    printf("<<<<<<<==  End the xrm context test ===>>>>>>>>\n\n");
}

void testXrmFunctions(void) {
    printf("<<<<<<<==  Start the xrm function test ===>>>>>>>>\n\n");
    xrmContext* ctx = (xrmContext*)xrmCreateContext(XRM_API_VERSION_1);
    if (ctx == NULL) {
        printf("Test 0-1: create context failed\n");
        return;
    }
    printf("Test 0-1: create context success\n");

    xrmConcurrentContextTest(256);
    xrmCheckDaemonTest(ctx);
    xrmCuAllocReleaseTest(ctx);
    xrmCuListAllocReleaseTest(ctx);
    xrmSoftCuAllocReleaseTest(ctx);
    xrmCuListAllocReleaseFromSameDevTest(ctx);
    xrmCuAllocReleaseUsingAliasTest(ctx);
    xrmCuAllocReleaseUsingKernelNameAndAliasTest(ctx);
    xrmCuAllocQueryReleaseUsingAliasTest(ctx);
    xrmCheckCuListAvailableNumUsingAliasTest(ctx);
    xrmCuPoolReserveAllocReleaseRelinquishTest(ctx);
    xrmLoadUnloadXclbinTest(ctx);
    xrmCheckDaemonTest(ctx);
    xrmPluginTest(ctx);
    xrmUdfCuGroupTest(ctx);
    xrmCuAllocWithLoadTest(ctx);
    xrmLoadAndAllCuAllocTest(ctx);
    xrmCuBlockingAllocReleaseTest(ctx);
    xrmCuListBlockingAllocReleaseTest(ctx);
    xrmCuGroupBlockingAllocReleaseTest(ctx);
    xrmCuAllocFromDevReleaseTest(ctx);
    xrmCuAllocReleaseGranularity1000000Test(ctx);
    xrmCuListAllocReleaseGranularity1000000Test(ctx);
    xrmSoftCuAllocReleaseGranularity1000000Test(ctx);
    xrmCuListAllocReleaseFromSameDevGranularity1000000Test(ctx);
    xrmCuAllocReleaseUsingAliasGranularity1000000Test(ctx);
    xrmCuAllocReleaseUsingKernelNameAndAliasGranularity1000000Test(ctx);
    xrmCuAllocQueryReleaseUsingAliasGranularity1000000Test(ctx);
    xrmCheckCuListAvailableNumUsingAliasGranularity1000000Test(ctx);
    xrmCuPoolReserveAllocReleaseRelinquishGranularity1000000Test(ctx);
    xrmUdfCuGroupGranularity1000000Test(ctx);
    xrmCuAllocWithLoadGranularity1000000Test(ctx);
    xrmCuBlockingAllocReleaseGranularity1000000Test(ctx);
    xrmCuListBlockingAllocReleaseGranularity1000000Test(ctx);
    xrmCuGroupBlockingAllocReleaseGranularity1000000Test(ctx);
    xrmCuAllocFromDevReleaseGranularity1000000Test(ctx);
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
