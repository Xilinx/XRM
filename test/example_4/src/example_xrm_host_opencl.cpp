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

#include <vector>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <xrm.h>
#include "example_xcl2.hpp"

#define LENGTH (1024)

int runKernel(int32_t deviceId, std::string kernelName, const char* pXclbin) {
    int pid = getpid();
    printf("[PID: %d] Start vector operation (Parent pid: %d)\n", pid, getppid());
    size_t vectorSizeBytes = sizeof(int) * LENGTH;
    std::vector<int, aligned_allocator<int> > sourceA(LENGTH);
    std::vector<int, aligned_allocator<int> > sourceB(LENGTH);
    std::vector<int, aligned_allocator<int> > resultSim(LENGTH);
    std::vector<int, aligned_allocator<int> > resultKrnl(LENGTH);

    /* Create the test data and run the vector addition locally */
    printf("[PID: %d] Fill the input data and generate golden output\n", pid);
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
            std::cout << "[ERROR] Krnl ID is unknown!!" << std::endl;
        }
    }

    printf("[PID: %d] Get Xilinx device information\n", pid);
    std::vector<cl::Device> devices = xcl::get_xil_devices();
    cl::Device device = devices[deviceId];

    printf("[PID: %d] Create context and command queue\n", pid);
    cl::Context context(device);
    cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE);
    std::string deviceName = device.getInfo<CL_DEVICE_NAME>();

    printf("[PID: %d] Import xclbin file\n", pid);
    cl::Program::Binaries bins = xcl::import_binary_file(pXclbin);
    devices.resize(1);

    //program the specified device.
    devices[0] = device;
    cl::Program program(context, devices, bins);
    cl::Kernel kernel(program, kernelName.c_str());

    printf("[PID: %d] Allocate OpenCL buffers\n", pid);
    std::vector<cl::Memory> inBufVec, outBufVec;
    cl::Buffer bufferA(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, vectorSizeBytes, sourceA.data());
    cl::Buffer bufferB(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, vectorSizeBytes, sourceB.data());
    cl::Buffer bufferC(context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, vectorSizeBytes, resultKrnl.data());
    inBufVec.push_back(bufferA);
    inBufVec.push_back(bufferB);
    outBufVec.push_back(bufferC);

    /* Copy input vectors to memory */
    printf("[PID: %d] Transfer the input data to device\n", pid);
    q.enqueueMigrateMemObjects(inBufVec, 0 /* 0 means from host*/);

    /* Set the kernel arguments */
    printf("[PID: %d] Set the kernel arguments\n", pid);
    int vectorLength = LENGTH;
    kernel.setArg(0, bufferA);
    kernel.setArg(1, bufferB);
    kernel.setArg(2, bufferC);
    kernel.setArg(3, vectorLength);

    /* Launch the kernel */
    printf("\n[PID: %d] Launch kernel\n", pid);
    q.enqueueTask(kernel);

    printf("\n[PID: %d] Wait until the kernel completes\n", pid);
    q.finish();

    /* Copy result to local buffer */
    printf("[PID: %d] Transfer the output data from device\n", pid);
    q.enqueueMigrateMemObjects(outBufVec, CL_MIGRATE_MEM_OBJECT_HOST);
    q.finish();

    /* Compare the results of the kernel to the simulation */
    int kernelMatch = 0;
    printf("[PID: %d] Check the output data with golden results\n", pid);
    for (int i = 0; i < LENGTH; i++) {
        if (resultSim[i] != resultKrnl[i]) {
            printf("[PID: %d] Error: i = %d CPU result = %d Krnl Result = %d\n", pid, i, resultSim[i], resultKrnl[i]);
            kernelMatch = 1;
            // break;
        } else {
            // printf("[PID: %d] Result Match: i = %d CPU result = %d Krnl Result = %d\n", pid, i, resultSim[i],
            // resultKrnl[i]);
        }
    }

    std::cout << "[PID: " << pid << "] TEST " << (kernelMatch ? "FAILED" : "PASSED") << std::endl;
    return (kernelMatch ? EXIT_FAILURE : EXIT_SUCCESS);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("How to run the test:\n");
        printf("./example_xrm_host_opencl krnl_vadd\n");
        printf("./example_xrm_host_opencl krnl_vsub\n");
        printf("./example_xrm_host_opencl krnl_vmul\n");
        printf("./example_xrm_host_opencl krnl_vdiv\n");
        return 0;
    }
    int iter = 2;
    std::string l_kernelName = argv[1];
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
        printf("xrmCuAlloc: fail to alloc scaler cu\n");
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

    if (iter > 0) {
        std::cout << "Now create (" << iter << ") child processes" << std::endl;

        for (int i = 0; i < iter; i++) {
            if (fork() == 0) {
                // printf("[son] pid %d from [parent] pid %d\n",getpid(),getppid());
                ret = runKernel(addCuRes.deviceId, l_kernelName, addCuRes.xclbinFileName);
                exit(ret);
            }
        }
        std::cout << "[PID: " << getpid() << "] Parent waits child to finish.\n\n" << std::endl;
        wait(NULL);
    }

    xrmCuRelease(ctx, &addCuRes);

    return ret;
}
