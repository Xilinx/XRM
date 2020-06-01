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

#include <stdio.h>
#include <string.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

#include <fstream>

#include <ert.h>
#include <xclhal2.h>
#include <xclbin.h>
#include <xrm.h>

#include "example_xrm_host_xcl.hpp"

#define LENGTH 1024

using namespace std;

int runKernel(int32_t deviceId, int kernelId, string kernelName, const char* pXclbin) {
    int pid = getpid();

    size_t vectorSizeBytes = sizeof(int) * LENGTH;

    int* sourceA;
    int* sourceB;
    int* resultSim;
    int* resultKernel;

    posix_memalign((void**)&sourceA, 4096, LENGTH * sizeof(int));
    posix_memalign((void**)&sourceB, 4096, LENGTH * sizeof(int));
    posix_memalign((void**)&resultSim, 4096, LENGTH * sizeof(int));
    posix_memalign((void**)&resultKernel, 4096, LENGTH * sizeof(int));

    cout << "kernelName:" << kernelName << "\n";
    /* Create the test data and run the vector addition locally */
    for (int i = 0; i < LENGTH; i++) {
        sourceA[i] = 2 * (i + pid);
        sourceB[i] = (i + pid);
        if (kernelName == "krnl_vadd") {
            resultSim[i] = sourceA[i] + sourceB[i];
        } else if (kernelName == "krnl_vsub") {
            resultSim[i] = sourceA[i] - sourceB[i];
        } else if (kernelName == "krnl_vmul") {
            resultSim[i] = sourceA[i] * sourceB[i];
        } else if (kernelName == "krnl_vdiv") {
            resultSim[i] = sourceA[i] / sourceB[i];
        } else {
            cout << "[ERROR] Krnl ID is unknown!!" << endl;
        }
    }

    xclDeviceHandle m_handle;
    uuid_t m_xclbinId;
    vector<int> m_mem;

    printf("[PID: %d] Get Xilinx device information\n", pid);
    m_handle = xclOpen(deviceId, NULL, XCL_INFO);

    ifstream l_stream(pXclbin);
    l_stream.seekg(0, l_stream.end);
    int l_size = l_stream.tellg();
    l_stream.seekg(0, l_stream.beg);

    char* l_header = new char[l_size];
    l_stream.read(l_header, l_size);

    const axlf* l_top = (const axlf*)l_header;
    auto l_topo = xclbin::get_axlf_section(l_top, MEM_TOPOLOGY);
    struct mem_topology* l_topology = (mem_topology*)(l_header + l_topo->m_sectionOffset);

    for (int i = 0; i < l_topology->m_count; ++i) {
        if (l_topology->m_mem_data[i].m_used) {
            int l_mem = i;
            m_mem.push_back(l_mem);
        }
    }

    uuid_copy(m_xclbinId, l_top->m_header.uuid);
    delete[] l_header;

    if (xclOpenContext(m_handle, m_xclbinId, kernelId, true)) {
        cout << "[ERROR] open context fail\n";
    }

    printf("[PID: %d] Allocate buffers\n", pid);

    unsigned buffer_a = xclAllocUserPtrBO(m_handle, sourceA, vectorSizeBytes, m_mem[0]);
    unsigned buffer_b = xclAllocUserPtrBO(m_handle, sourceB, vectorSizeBytes, m_mem[0]);
    unsigned buffer_c = xclAllocUserPtrBO(m_handle, resultKernel, vectorSizeBytes, m_mem[0]);

    /* Copy input vectors to memory */
    printf("[PID: %d] Transfer data to device\n", pid);

    if (xclSyncBO(m_handle, buffer_a, XCL_BO_SYNC_BO_TO_DEVICE, vectorSizeBytes, 0)) {
        cout << "[ERROR] sync a fail\n";
    }
    if (xclSyncBO(m_handle, buffer_b, XCL_BO_SYNC_BO_TO_DEVICE, vectorSizeBytes, 0)) {
        cout << "[ERROR] sync b fail\n";
    }
    if (xclSyncBO(m_handle, buffer_c, XCL_BO_SYNC_BO_TO_DEVICE, vectorSizeBytes, 0)) {
        cout << "[ERROR] sync c fail\n";
    }

    // q.enqueueMigrateMemObjects(inBufVec,0/* 0 means from host*/);

    /* Set the kernel arguments */
    printf("[PID: %d] Set the kernel arguments\n", pid);

    xclBOProperties p;
    uint64_t address_a = !xclGetBOProperties(m_handle, buffer_a, &p) ? p.paddr : -1;
    uint64_t address_b = !xclGetBOProperties(m_handle, buffer_b, &p) ? p.paddr : -1;
    uint64_t address_c = !xclGetBOProperties(m_handle, buffer_c, &p) ? p.paddr : -1;

    int vectorLength = LENGTH;

    unsigned int m_execHandle = xclAllocBO(m_handle, vectorSizeBytes, xclBOKind(0), (1 << 31));
    void* execData = xclMapBO(m_handle, m_execHandle, true);
    auto ecmd = reinterpret_cast<ert_start_kernel_cmd*>(execData);
    auto rsz = XKRNL_VSUB_KRNL_VSUB_CONTROL_ADDR_N_ELEMENTS_DATA / 4 + 2; // regmap array size
    memset(ecmd, 0, (sizeof *ecmd) + rsz);
    ecmd->state = ERT_CMD_STATE_NEW;
    ecmd->opcode = ERT_START_CU;
    ecmd->count = 1 + rsz;
    ecmd->cu_mask = 0x1 << kernelId;
    ecmd->data[XKRNL_VSUB_KRNL_VSUB_CONTROL_ADDR_AP_CTRL] = 0x0; // ap_start
    ecmd->data[XKRNL_VSUB_KRNL_VSUB_CONTROL_ADDR_A_DATA / 4] = address_a;
    ecmd->data[XKRNL_VSUB_KRNL_VSUB_CONTROL_ADDR_B_DATA / 4] = address_b;
    ecmd->data[XKRNL_VSUB_KRNL_VSUB_CONTROL_ADDR_OUT_R_DATA / 4] = address_c;
    ecmd->data[XKRNL_VSUB_KRNL_VSUB_CONTROL_ADDR_A_DATA / 4 + 1] = address_a >> 32;
    ecmd->data[XKRNL_VSUB_KRNL_VSUB_CONTROL_ADDR_B_DATA / 4 + 1] = address_b >> 32;
    ecmd->data[XKRNL_VSUB_KRNL_VSUB_CONTROL_ADDR_OUT_R_DATA / 4 + 1] = address_c >> 32;
    ecmd->data[XKRNL_VSUB_KRNL_VSUB_CONTROL_ADDR_N_ELEMENTS_DATA / 4] = vectorLength;

    if (xclExecBuf(m_handle, m_execHandle)) {
        cout << "[ERROR] exec fail\n";
    }

    while (xclExecWait(m_handle, 1) == 0)
        ;

    if (xclSyncBO(m_handle, buffer_c, XCL_BO_SYNC_BO_FROM_DEVICE, vectorSizeBytes, 0)) {
        cout << "[ERROR] copy back fail\n";
    }

    /* Compare the results of the kernel to the simulation */
    int kernelMatch = 0;
    printf("[PID: %d] Check the output data with golden results\n", pid);
    for (int i = 0; i < 10; i++) {
        if (resultSim[i] != resultKernel[i]) {
            printf("[PID: %d] Error: i = %d CPU result = %d Krnl Result = %d\n", pid, i, resultSim[i], resultKernel[i]);
            kernelMatch = 1;
            // break;
        }
    }

    cout << "[PID: " << pid << "] TEST " << (kernelMatch ? "FAILED" : "PASSED") << endl;
    return (kernelMatch ? EXIT_FAILURE : EXIT_SUCCESS);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("How to run the test:\n");
        printf("./example_xrm_host_xcl krnl_vadd\n");
        printf("./example_xrm_host_xcl krnl_vsub\n");
        printf("./example_xrm_host_xcl krnl_vmul\n");
        printf("./example_xrm_host_xcl krnl_vdiv\n");
        return 0;
    }

    int iter = 2;

    string l_kernelName = argv[1];
    xrmContext* ctx = (xrmContext*)xrmCreateContext(XRM_API_VERSION_1);
    if (ctx == NULL) {
        printf("create context failed\n");
        return 0;
    }

    xrmCuProperty addCuProp;
    xrmCuResource addCuRes;

    memset(&addCuProp, 0, sizeof(xrmCuProperty));
    memset(&addCuRes, 0, sizeof(xrmCuResource));
    strcpy(addCuProp.kernelName, l_kernelName.c_str());
    strcpy(addCuProp.kernelAlias, "");
    addCuProp.devExcl = false;
    addCuProp.requestLoad = 100;

    int ret = xrmCuAlloc(ctx, &addCuProp, &addCuRes);
    if (ret != 0) {
        printf("xrmCuAlloc: fail to alloc cu\n");
        return ret;
    } else {
        printf("Allocated scaler cu: \n");
        printf("   xclbinFileName is:  %s\n", addCuRes.xclbinFileName);
        printf("   kernelPluginFileName is:  %s\n", addCuRes.kernelPluginFileName);
        printf("   kernelName is:  %s\n", addCuRes.kernelName);
        printf("   kernelAlias is:  %s\n", addCuRes.kernelAlias);
        printf("   deviceId is:  %d\n", addCuRes.deviceId);
        printf("   cuId is:  %d\n", addCuRes.cuId);
        printf("   channelId is:  %d\n", addCuRes.channelId);
        printf("   allocServiceId is:  %lu\n", addCuRes.allocServiceId);
        printf("   channelLoad is:  %d\n", addCuRes.channelLoad);
    }

    ret = runKernel(addCuRes.deviceId, addCuRes.cuId, l_kernelName, addCuRes.xclbinFileName);
    xrmCuRelease(ctx, &addCuRes);

    return ret;
}
