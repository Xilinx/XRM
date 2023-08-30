#
# Copyright (C) 2019-2020, Xilinx Inc - All rig#ts reserved
#
# Copyright (C) 2023, Advanced Micro Devices, Inc. All rights reserved.
#
# Xilinx Resouce Management
#
# Licensed under the Apache License, Version 2.0 (the "License'). You may
# not use this file except in compliance with the License. A copy of the
# License is located at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations
# under the License.
#

import sys
import os
sys.path.append(os.path.abspath(os.environ['XILINX_XRM']+"/include"))
from xrm import *

def xrmCuAllocReleaseTest(ctx):
    # alloc scaler cu
    scalerCuProp = xrmCuProperty()
    scalerCuRes = xrmCuResource()
    scalerCuStat = xrmCuStat()
    scalerCuProp.kernelName = b"v_abrscaler_top"
    scalerCuProp.kernelAlias = b""
    scalerCuProp.devExcl = False
    scalerCuProp.requestLoad = 45
    scalerCuProp.poolId = 0
    print('Test 1-1: scaler cu prop: kernelName is {}'.format(scalerCuProp.kernelName))
    print('Test 1-1: scaler cu prop: kernelAlias is {}'.format(scalerCuProp.kernelAlias))
    ret = xrmCuAlloc(ctx, scalerCuProp, scalerCuRes)
    if ret != 0:
      print('xrmCuAlloc: fail to alloc scaler cu')
    else:
        print('Allocated scaler cu:')
        print('   xclbinFileName is:  {}'.format(scalerCuRes.xclbinFileName))
        print('   kernelPluginFileName is:  {}'.format(scalerCuRes.kernelPluginFileName))
        print('   kernelName is:  {}'.format(scalerCuRes.kernelName))
        print('   instanceName is:  {}'.format(scalerCuRes.instanceName))
        print('   cuName is:  {}'.format(scalerCuRes.cuName))
        print('   kernelAlias is:  {}'.format(scalerCuRes.kernelAlias))
        print('   deviceId is:  {}'.format(scalerCuRes.deviceId))
        print('   cuId is:  {}'.format(scalerCuRes.cuId))
        print('   channelId is:  {}'.format(scalerCuRes.channelId))
        print('   cuType is:  {}'.format(scalerCuRes.cuType))
        print('   baseAddr is:  {}'.format(scalerCuRes.baseAddr))
        print('   membankId is:  {}'.format(scalerCuRes.membankId))
        print('   membankType is:  {}'.format(scalerCuRes.membankType))
        print('   membankSize is:  {}'.format(scalerCuRes.membankSize))
        print('   membankBaseAddr is:  {}'.format(scalerCuRes.membankBaseAddr))
        print('   allocServiceId is:  {}'.format(scalerCuRes.allocServiceId))
        print('   poolId is:  {}'.format(scalerCuRes.poolId))
        print('   channelLoad is:  {}'.format(scalerCuRes.channelLoad))
    
    maxCapacity = xrmCuGetMaxCapacity(ctx, scalerCuProp)
    print('Test 1-2: Get Max Capacity: v_abrscaler_top cu max capacity is: {}'.format(maxCapacity))
    
    print('Test 1-3: Check SCALER cu status: ')
    ret = xrmCuCheckStatus(ctx, scalerCuRes, scalerCuStat)
    if ret != 0:
        print('xrmCuCheckStatus: fail to check status of scaler cu')
    else:
        if scalerCuStat.isBusy:
            print('   isBusy:  true')
        else:
            print('   isBusy:  false')
        print('   usedLoad: {}'.format(scalerCuStat.usedLoad))

    # alloc encoder cu
    encCuProp = xrmCuProperty()
    encCuRes = xrmCuResource()
    encCuProp.kernelName = b"krnl_ngcodec_pistachio_enc"
    encCuProp.kernelAlias = b""
    encCuProp.devExcl = False
    encCuProp.requestLoad = 55
    maxCapacity = xrmCuGetMaxCapacity(ctx, encCuProp)
    encCuProp.poolId = 0
    
    print('Test 1-4: Get Max Capacity: krnl_ngcodec_pistachio_enc cu max capacity is: {}'.format(maxCapacity))
    print('Test 1-5: Alloc cu, kernelName is {}'.format(encCuProp.kernelName))
    
    ret = xrmCuAlloc(ctx, encCuProp, encCuRes)
    if ret != 0:
      print('xrmCuAlloc: fail to alloc encoder cu')
    else:
        print('Allocated encoder cu:')
        print('   xclbinFileName is:  {}'.format(encCuRes.xclbinFileName))
        print('   kernelPluginFileName is:  {}'.format(encCuRes.kernelPluginFileName))
        print('   kernelName is:  {}'.format(encCuRes.kernelName))
        print('   instanceName is:  {}'.format(encCuRes.instanceName))
        print('   cuName is:  {}'.format(encCuRes.cuName))
        print('   kernelAlias is:  {}'.format(encCuRes.kernelAlias))
        print('   deviceId is:  {}'.format(encCuRes.deviceId))
        print('   cuId is:  {}'.format(encCuRes.cuId))
        print('   channelId is:  {}'.format(encCuRes.channelId))
        print('   cuType is:  {}'.format(encCuRes.cuType))
        print('   baseAddr is:  {}'.format(encCuRes.baseAddr))
        print('   membankId is:  {}'.format(encCuRes.membankId))
        print('   membankType is:  {}'.format(encCuRes.membankType))
        print('   membankSize is:  {}'.format(encCuRes.membankSize))
        print('   membankBaseAddr is:  {}'.format(encCuRes.membankBaseAddr))
        print('   allocServiceId is:  {}'.format(encCuRes.allocServiceId))
        print('   poolId is:  {}'.format(encCuRes.poolId))
        print('   channelLoad is:  {}'.format(encCuRes.channelLoad))
      
    print('<<<<<<<==  end the xrm allocation test ===>>>>>>>>')
    
    print('Test 1-6:  release scaler cu')
    if xrmCuRelease(ctx, scalerCuRes):
        print('success to  release scaler cu')
    else:
        print('fail to release scaler cu')
        
    print('Test 1-7:   release encoder cu')
    if xrmCuRelease(ctx, encCuRes):
        print('success to release encoder cu')
    else:
        print('fail to release encoder cu')
  
def xrmCuListAllocReleaseTest(ctx):
    print('<<<<<<<==  start the xrm cu list allocation test ===>>>>>>>>')
    print('Test 2-1: alloc scaler cu list')
    # alloc scaler cu
    scalerCuListProp = xrmCuListProperty()
    scalerCuListRes = xrmCuListResource()
    scalerCuListProp.cuNum = 4
    for i in range(4):
        scalerCuListProp.cuProps[i].kernelName = b"v_abrscaler_top"
        scalerCuListProp.cuProps[i].kernelAlias = b""
        scalerCuListProp.cuProps[i].devExcl = False
        scalerCuListProp.cuProps[i].requestLoad = 45
        scalerCuListProp.cuProps[i].poolId = 0

    print('scalerCuListProp.cuNum is cu {}'.format(scalerCuListProp.cuNum))
    ret = xrmCuListAlloc(ctx, scalerCuListProp, scalerCuListRes)

    if ret != 0:
        print('xrmCuListAlloc: fail to alloc scaler cu list')
    else:
        for i in range(scalerCuListRes.cuNum):
            print('Allocated scaler cu list: cu {}'.format(i))
            print('   xclbinFileName is:  {}'.format(scalerCuListRes.cuResources[i].xclbinFileName))
            print('   kernelPluginFileName is:  {}'.format(scalerCuListRes.cuResources[i].kernelPluginFileName))
            print('   kernelName is:  {}'.format(scalerCuListRes.cuResources[i].kernelName))
            print('   kernelAlias is:  {}'.format(scalerCuListRes.cuResources[i].kernelAlias))
            print('   instanceName is:  {}'.format(scalerCuListRes.cuResources[i].instanceName))
            print('   cuName is:  {}'.format(scalerCuListRes.cuResources[i].cuName))
            print('   deviceId is:  {}'.format(scalerCuListRes.cuResources[i].deviceId))
            print('   cuId is:  {}'.format(scalerCuListRes.cuResources[i].cuId))
            print('   channelId is:  {}'.format(scalerCuListRes.cuResources[i].channelId))
            print('   cuType is:  {}'.format(scalerCuListRes.cuResources[i].cuType))
            print('   baseAddr is:  {}'.format(scalerCuListRes.cuResources[i].baseAddr))
            print('   membankId is:  {}'.format(scalerCuListRes.cuResources[i].membankId))
            print('   membankType is:  {}'.format(scalerCuListRes.cuResources[i].membankType))
            print('   membankSize is:  {}'.format(scalerCuListRes.cuResources[i].membankSize))
            print('   membankBaseAddr is:  {}'.format(scalerCuListRes.cuResources[i].membankBaseAddr))
            print('   allocServiceId is:  {}'.format(scalerCuListRes.cuResources[i].allocServiceId))
            print('   poolId is:  {}'.format(scalerCuListRes.cuResources[i].poolId))
            print('   channelLoad is:  {}'.format(scalerCuListRes.cuResources[i].channelLoad))
    
    print('Test 2-2:  alloc encoder cu list')
    encCuProps = xrmCuListProperty()
    encCuListRes = xrmCuListResource()
    encCuProps.cuNum = 4
    for i in range(encCuProps.cuNum):
        encCuProps.cuProps[i].kernelName = b"krnl_ngcodec_pistachio_enc"
        encCuProps.cuProps[i].kernelAlias = b""
        encCuProps.cuProps[i].devExcl = False
        encCuProps.cuProps[i].requestLoad = 55
        encCuProps.cuProps[i].poolId = 0
    ret = xrmCuListAlloc(ctx, encCuProps, encCuListRes)
    if ret != 0:
        print('xrmCuListAlloc: fail to alloc encoder cu list')
    else:
        for i in range(encCuListRes.cuNum):
            print('Allocated ENCODER cu list: cu {}'.format(i))
            print('   xclbinFileName is:  {}'.format(encCuListRes.cuResources[i].xclbinFileName))
            print('   kernelPluginFileName is:  {}'.format(encCuListRes.cuResources[i].kernelPluginFileName))
            print('   kernelName is:  {}'.format(encCuListRes.cuResources[i].kernelName))
            print('   kernelAlias is:  {}'.format(encCuListRes.cuResources[i].kernelAlias))
            print('   instanceName is:  {}'.format(encCuListRes.cuResources[i].instanceName))
            print('   cuName is:  {}'.format(encCuListRes.cuResources[i].cuName))
            print('   deviceId is:  {}'.format(encCuListRes.cuResources[i].deviceId))
            print('   cuId is:  {}'.format(encCuListRes.cuResources[i].cuId))
            print('   channelId is:  {}'.format(encCuListRes.cuResources[i].channelId))
            print('   cuType is:  {}'.format(encCuListRes.cuResources[i].cuType))
            print('   baseAddr is:  {}'.format(encCuListRes.cuResources[i].baseAddr))
            print('   membankId is:  {}'.format(encCuListRes.cuResources[i].membankId))
            print('   membankType is:  {}'.format(encCuListRes.cuResources[i].membankType))
            print('   membankSize is:  {}'.format(encCuListRes.cuResources[i].membankSize))
            print('   membankBaseAddr is:  {}'.format(encCuListRes.cuResources[i].membankBaseAddr))
            print('   allocServiceId is:  {}'.format(encCuListRes.cuResources[i].allocServiceId))
            print('   poolId is:  {}'.format(encCuListRes.cuResources[i].poolId))
            print('   channelLoad is:  {}'.format(encCuListRes.cuResources[i].channelLoad))

    print('Test 2-3:   release scaler cu list')
    if xrmCuListRelease(ctx, scalerCuListRes):
        print('success to release scaler cu list')
    else:
        print('fail to release scaler cu list')

    print('Test 2-4:  release encoder cu list')
    if xrmCuListRelease(ctx, encCuListRes):
        print('success to release encoder cu list')
    else:
        print('fail to release encoder cu list')

def xrmSoftCuAllocReleaseTest(ctx):
    print('<<<<<<<==  start the xrm soft cu allocation test ===>>>>>>>>')

    # alloc scaler cu
    scalerCuProp = xrmCuProperty()
    scalerCuRes = xrmCuResource()
    scalerCuStat = xrmCuStat()

    scalerCuProp.kernelName = b"v_abrscaler_top"
    scalerCuProp.kernelAlias = b""
    scalerCuProp.devExcl = False
    scalerCuProp.requestLoad = 45
    scalerCuProp.poolId = 0

    print('Test 3-1: Alloc scaler cu, kernelName is {}'.format(scalerCuProp.kernelName))
    print('Test 3-1: Alloc scaler cu, kernelAlias is {}'.format(scalerCuProp.kernelAlias))

    ret = xrmCuAlloc(ctx, scalerCuProp, scalerCuRes)
    if (ret != 0):
        print('Allocated scaler cu failed.')
    else:
        print('Allocated scaler cu:')
        print('   xclbinFileName is:  {}'.format(scalerCuRes.xclbinFileName))
        print('   kernelPluginFileName is:  {}'.format(scalerCuRes.kernelPluginFileName))
        print('   kernelName is:  {}'.format(scalerCuRes.kernelName))
        print('   kernelAlias is:  {}'.format(scalerCuRes.kernelAlias))
        print('   instanceName is:  {}'.format(scalerCuRes.instanceName))
        print('   cuName is:  {}'.format(scalerCuRes.cuName))
        print('   deviceId is:  {}'.format(scalerCuRes.deviceId))
        print('   cuId is:  {}'.format(scalerCuRes.cuId))
        print('   channelId is:  {}'.format(scalerCuRes.channelId))
        print('   cuType is:  {}'.format(scalerCuRes.cuType))
        print('   baseAddr is:  {}'.format(scalerCuRes.baseAddr))
        print('   membankId is:  {}'.format(scalerCuRes.membankId))
        print('   membankType is:  {}'.format(scalerCuRes.membankType))
        print('   membankSize is:  {}'.format(scalerCuRes.membankSize))
        print('   membankBaseAddr is:  {}'.format(scalerCuRes.membankBaseAddr))
        print('   allocServiceId is:  {}'.format(scalerCuRes.allocServiceId))
        print('   poolId is:  {}'.format(scalerCuRes.poolId))
        print('   channelLoad is:  {}'.format(scalerCuRes.channelLoad))

    maxCapacity = xrmCuGetMaxCapacity(ctx, scalerCuProp)
    print('Test 3-2: Get Max Capacity: v_abrscaler_top cu max capacity is: {}'.format(maxCapacity))

    print('Test 3-3: Check SCALER cu status: ')
    ret = (xrmCuCheckStatus(ctx, scalerCuRes, scalerCuStat))
    if (ret != 0):
        print('xrmCuCheckStatus: fail to check status of scaler cu')
    else:
        if (scalerCuStat.isBusy):
            print('   isBusy:  true')
        else:
            print('   isBusy:  false')
        print('   usedLoad:  {}'.format(scalerCuStat.usedLoad))

    print('<<<<<<<==  end the xrm soft cu allocation test ===>>>>>>>>')

    print('Test 3-4:  release scaler cu')
    if (xrmCuRelease(ctx, scalerCuRes)):
        print('success to release scaler cu')
    else:
        print('fail to release scaler cu')

def xrmCuListAllocReleaseFromSameDevTest(ctx):
    print('<<<<<<<==  start the xrm cu list from same dev allocation test ===>>>>>>>>')
    print('Test 4-1: alloc scaler cu list')
    scalerCuListProp = xrmCuListProperty()
    scalerCuListRes = xrmCuListResource()
    scalerCuListProp.cuNum = 4
    scalerCuListProp.sameDevice = True
    for i in range(scalerCuListProp.cuNum):
      scalerCuListProp.cuProps[i].kernelName = b"v_abrscaler_top"
      scalerCuListProp.cuProps[i].kernelAlias = b""
      scalerCuListProp.cuProps[i].devExcl = False
      scalerCuListProp.cuProps[i].requestLoad = 20
      scalerCuListProp.cuProps[i].poolId = 0
    ret = xrmCuListAlloc(ctx, scalerCuListProp, scalerCuListRes)
    if ret != 0:
        print('xrmCuListAlloc: fail to alloc scaler cu list')
    else:
        for i in range(scalerCuListRes.cuNum):
            print('Allocated scaler cu list: cu {}'.format(i))
            print('   xclbinFileName is:  {}'.format(scalerCuListRes.cuResources[i].xclbinFileName))
            print('   kernelPluginFileName is:  {}'.format(scalerCuListRes.cuResources[i].kernelPluginFileName))
            print('   kernelName is:  {}'.format(scalerCuListRes.cuResources[i].kernelName))
            print('   kernelAlias is:  {}'.format(scalerCuListRes.cuResources[i].kernelAlias))
            print('   instanceName is:  {}'.format(scalerCuListRes.cuResources[i].instanceName))
            print('   cuName is:  {}'.format(scalerCuListRes.cuResources[i].cuName))
            print('   deviceId is:  {}'.format(scalerCuListRes.cuResources[i].deviceId))
            print('   cuId is:  {}'.format(scalerCuListRes.cuResources[i].cuId))
            print('   channelId is:  {}'.format(scalerCuListRes.cuResources[i].channelId))
            print('   cuType is:  {}'.format(scalerCuListRes.cuResources[i].cuType))
            print('   baseAddr is:  {}'.format(scalerCuListRes.cuResources[i].baseAddr))
            print('   membankId is:  {}'.format(scalerCuListRes.cuResources[i].membankId))
            print('   membankType is:  {}'.format(scalerCuListRes.cuResources[i].membankType))
            print('   membankSize is:  {}'.format(scalerCuListRes.cuResources[i].membankSize))
            print('   membankBaseAddr is:  {}'.format(scalerCuListRes.cuResources[i].membankBaseAddr))
            print('   allocServiceId is:  {}'.format(scalerCuListRes.cuResources[i].allocServiceId))
            print('   poolId is:  {}'.format(scalerCuListRes.cuResources[i].poolId))
            print('   channelLoad is:  {}'.format(scalerCuListRes.cuResources[i].channelLoad))

    print('Test 4-2:  alloc encoder cu list')
    encCuProps = xrmCuListProperty()
    encCuListRes = xrmCuListResource()
    encCuProps.cuNum = 4
    encCuProps.sameDevice = True
    for i in range(encCuProps.cuNum):
        encCuProps.cuProps[i].kernelName = b"krnl_ngcodec_pistachio_enc"
        encCuProps.cuProps[i].kernelAlias = b""
        encCuProps.cuProps[i].devExcl = False
        encCuProps.cuProps[i].requestLoad = 30
        encCuProps.cuProps[i].poolId = 0
        ret = xrmCuListAlloc(ctx, encCuProps, encCuListRes)
    if ret != 0:
        print('xrmCuListAlloc: fail to alloc encoder cu list')
    else:
        for i in range(encCuListRes.cuNum):
            print('Allocated ENCODER cu list: cu {}'.format(i))
            print('   xclbinFileName is:  {}'.format(encCuListRes.cuResources[i].xclbinFileName))
            print('   kernelPluginFileName is:  {}'.format(encCuListRes.cuResources[i].kernelPluginFileName))
            print('   kernelName is:  {}'.format(encCuListRes.cuResources[i].kernelName))
            print('   kernelAlias is:  {}'.format(encCuListRes.cuResources[i].kernelAlias))
            print('   instanceName is:  {}'.format(encCuListRes.cuResources[i].instanceName))
            print('   cuName is:  {}'.format(encCuListRes.cuResources[i].cuName))
            print('   deviceId is:  {}'.format(encCuListRes.cuResources[i].deviceId))
            print('   cuId is:  {}'.format(encCuListRes.cuResources[i].cuId))
            print('   channelId is:  {}'.format(encCuListRes.cuResources[i].channelId))
            print('   cuType is:  {}'.format(encCuListRes.cuResources[i].cuType))
            print('   baseAddr is:  {}'.format(encCuListRes.cuResources[i].baseAddr))
            print('   membankId is:  {}'.format(encCuListRes.cuResources[i].membankId))
            print('   membankType is:  {}'.format(encCuListRes.cuResources[i].membankType))
            print('   membankSize is:  {}'.format(encCuListRes.cuResources[i].membankSize))
            print('   membankBaseAddr is:  {}'.format(encCuListRes.cuResources[i].membankBaseAddr))
            print('   allocServiceId is:  {}'.format(encCuListRes.cuResources[i].allocServiceId))
            print('   poolId is:  {}'.format(encCuListRes.cuResources[i].poolId))
            print('   channelLoad is:  {}'.format(encCuListRes.cuResources[i].channelLoad))

    print('Test 4-3:   release scaler cu list')
    if xrmCuListRelease(ctx, scalerCuListRes):
        print('success to release scaler cu list')
    else:
        print('fail to release scaler cu list')

    print('Test 4-4:  release encoder cu list')
    if xrmCuListRelease(ctx, encCuListRes):
        print('success to release encoder cu list')
    else:
        print('fail to release encoder cu list')

def xrmCuAllocReleaseUsingAliasTest(ctx):
    print('<<<<<<<==  start the xrm allocation with kernel alias test ===>>>>>>>>')
    scalerCuProp = xrmCuProperty()
    scalerCuRes = xrmCuResource()
    scalerCuStat = xrmCuStat()
    scalerCuProp.kernelName = b""
    scalerCuProp.kernelAlias = b"SCALER_yuv420p_Xilinx"
    scalerCuProp.devExcl = False
    scalerCuProp.requestLoad = 45
    scalerCuProp.poolId = 0
    print('Test 5-1: Alloc scaler cu, kernelName is {}'.format(scalerCuProp.kernelName))
    print('Test 5-1: Alloc scaler cu, kernelAlias is {}'.format(scalerCuProp.kernelAlias))
    ret = xrmCuAlloc(ctx, scalerCuProp, scalerCuRes)
    if ret != 0:
        print('xrmCuAlloc: fail to alloc scaler cu')
    else:
        print('Allocated scaler cu: ')
        print('   xclbinFileName is:  {}'.format(scalerCuRes.xclbinFileName))
        print('   kernelPluginFileName is:  {}'.format(scalerCuRes.kernelPluginFileName))
        print('   kernelName is:  {}'.format(scalerCuRes.kernelName))
        print('   kernelAlias is:  {}'.format(scalerCuRes.kernelAlias))
        print('   instanceName is:  {}'.format(scalerCuRes.instanceName))
        print('   cuName is:  {}'.format(scalerCuRes.cuName))
        print('   deviceId is:  {}'.format(scalerCuRes.deviceId))
        print('   cuId is:  {}'.format(scalerCuRes.cuId))
        print('   channelId is:  {}'.format(scalerCuRes.channelId))
        print('   cuType is:  {}'.format(scalerCuRes.cuType))
        print('   baseAddr is:  {}'.format(scalerCuRes.baseAddr))
        print('   membankId is:  {}'.format(scalerCuRes.membankId))
        print('   membankType is:  {}'.format(scalerCuRes.membankType))
        print('   membankSize is:  {}'.format(scalerCuRes.membankSize))
        print('   membankBaseAddr is:  {}'.format(scalerCuRes.membankBaseAddr))
        print('   allocServiceId is:  {}'.format(scalerCuRes.allocServiceId))
        print('   poolId is:  {}'.format(scalerCuRes.poolId))
        print('   channelLoad is:  {}'.format(scalerCuRes.channelLoad))
    maxCapacity = xrmCuGetMaxCapacity(ctx, scalerCuProp)
    print('Test 5-2: Get Max Capacity: v_abrscaler_top cu max capacity is: {}'.format(maxCapacity))
    
    print('Test 5-3: Check SCALER cu status: ')
    ret = xrmCuCheckStatus(ctx, scalerCuRes, scalerCuStat)
    if ret != 0:
        print('xrmCuCheckStatus: fail to check status of scaler cu')
    else:
        if scalerCuStat.isBusy:
            print('   isBusy:  true')
        else:
            print('   isBusy:  false')
        print('   usedLoad: {}'.format(scalerCuStat.usedLoad))
    print('<<<<<<<==  end the xrm allocation with kernel alias test ===>>>>>>>>')
    print('Test 5-4:  release scaler cu')
    if xrmCuRelease(ctx, scalerCuRes):
        print('success to  release scaler cu')
    else:
        print('fail to release scaler cu')

def xrmCuAllocReleaseUsingKernelNameAndAliasTest(ctx):
    print('<<<<<<<==  start the xrm allocation with kernel name and alias test ===>>>>>>>>')
    scalerCuProp = xrmCuProperty()
    scalerCuRes = xrmCuResource()
    scalerCuStat = xrmCuStat()
    scalerCuProp.kernelName = b"v_abrscaler_top"
    scalerCuProp.kernelAlias = b"SCALER_yuv420p_Xilinx"
    scalerCuProp.devExcl = False
    scalerCuProp.requestLoad = 45
    scalerCuProp.poolId = 0
    print('Test 6-1: Alloc scaler cu, kernelName is {}'.format(scalerCuProp.kernelName))
    print('Test 6-1: Alloc scaler cu, kernelAlias is {}'.format(scalerCuProp.kernelAlias))
    ret = xrmCuAlloc(ctx, scalerCuProp, scalerCuRes)
    if ret != 0:
        print('xrmCuAlloc: fail to alloc scaler cu')
    else:
        print('   xclbinFileName is:  {}'.format(scalerCuRes.xclbinFileName))
        print('   kernelPluginFileName is:  {}'.format(scalerCuRes.kernelPluginFileName))
        print('   kernelName is:  {}'.format(scalerCuRes.kernelName))
        print('   kernelAlias is:  {}'.format(scalerCuRes.kernelAlias))
        print('   instanceName is:  {}'.format(scalerCuRes.instanceName))
        print('   cuName is:  {}'.format(scalerCuRes.cuName))
        print('   deviceId is:  {}'.format(scalerCuRes.deviceId))
        print('   cuId is:  {}'.format(scalerCuRes.cuId))
        print('   channelId is:  {}'.format(scalerCuRes.channelId))
        print('   cuType is:  {}'.format(scalerCuRes.cuType))
        print('   baseAddr is:  {}'.format(scalerCuRes.baseAddr))
        print('   membankId is:  {}'.format(scalerCuRes.membankId))
        print('   membankType is:  {}'.format(scalerCuRes.membankType))
        print('   membankSize is:  {}'.format(scalerCuRes.membankSize))
        print('   membankBaseAddr is:  {}'.format(scalerCuRes.membankBaseAddr))
        print('   allocServiceId is:  {}'.format(scalerCuRes.allocServiceId))
        print('   poolId is:  {}'.format(scalerCuRes.poolId))
        print('   channelLoad is:  {}'.format(scalerCuRes.channelLoad))

    maxCapacity = xrmCuGetMaxCapacity(ctx, scalerCuProp)
    print('Test 6-2: Get Max Capacity: v_abrscaler_top cu max capacity is: {}'.format(maxCapacity))
    print('Test 6-3: Check SCALER cu status: ')
    ret = xrmCuCheckStatus(ctx, scalerCuRes, scalerCuStat)
    if ret != 0:
        print('xrmCuCheckStatus: fail to check status of scaler cu')
    else:
        if scalerCuStat.isBusy:
            print('   isBusy:  true')
        else:
            print('   isBusy:  false')
        print('   usedLoad: {}'.format(scalerCuStat.usedLoad)) 
    print('<<<<<<<==  end the xrm allocation with kernel name and alias test ===>>>>>>>>')
    print('Test 6-4:  release scaler cu')
    if xrmCuRelease(ctx, scalerCuRes):
        print('success to  release scaler cu')
    else:
        print('fail to release scaler cu')

def xrmCuAllocQueryReleaseUsingAliasTest(ctx):
    print('<<<<<<<==  start the xrm allocation and query with kernel alias test ===>>>>>>>>')
    print('Test 7-1: alloc scaler cu list')
    # alloc scaler cu
    scalerCuListProp = xrmCuListProperty()
    scalerCuListRes = xrmCuListResource()
    scalerCuListProp.cuNum = 4
    scalerCuListProp.sameDevice = False
    for i in range(scalerCuListProp.cuNum):
        scalerCuListProp.cuProps[i].kernelName = b"v_abrscaler_top"
        scalerCuListProp.cuProps[i].kernelAlias = b""
        scalerCuListProp.cuProps[i].devExcl = False
        scalerCuListProp.cuProps[i].requestLoad = 45
        scalerCuListProp.cuProps[i].poolId = 0
    ret = xrmCuListAlloc(ctx, scalerCuListProp, scalerCuListRes)
    
    if ret != 0:
        print('xrmCuListAlloc: fail to alloc scaler cu list')
    else:
        for i in range(scalerCuListRes.cuNum):
            print('Allocated scaler cu list: cu {}'.format(i))
            print('   xclbinFileName is:  {}'.format(scalerCuListRes.cuResources[i].xclbinFileName))
            print('   kernelPluginFileName is:  {}'.format(scalerCuListRes.cuResources[i].kernelPluginFileName))
            print('   kernelName is:  {}'.format(scalerCuListRes.cuResources[i].kernelName))
            print('   kernelAlias is:  {}'.format(scalerCuListRes.cuResources[i].kernelAlias))
            print('   instanceName is:  {}'.format(scalerCuListRes.cuResources[i].instanceName))
            print('   cuName is:  {}'.format(scalerCuListRes.cuResources[i].cuName))
            print('   deviceId is:  {}'.format(scalerCuListRes.cuResources[i].deviceId))
            print('   cuId is:  {}'.format(scalerCuListRes.cuResources[i].cuId))
            print('   channelId is:  {}'.format(scalerCuListRes.cuResources[i].channelId))
            print('   cuType is:  {}'.format(scalerCuListRes.cuResources[i].cuType))
            print('   baseAddr is:  {}'.format(scalerCuListRes.cuResources[i].baseAddr))
            print('   membankId is:  {}'.format(scalerCuListRes.cuResources[i].membankId))
            print('   membankType is:  {}'.format(scalerCuListRes.cuResources[i].membankType))
            print('   membankSize is:  {}'.format(scalerCuListRes.cuResources[i].membankSize))
            print('   membankBaseAddr is:  {}'.format(scalerCuListRes.cuResources[i].membankBaseAddr))
            print('   allocServiceId is:  {}'.format(scalerCuListRes.cuResources[i].allocServiceId))
            print('   poolId is:  {}'.format(scalerCuListRes.cuResources[i].poolId))
            print('   channelLoad is:  {}'.format(scalerCuListRes.cuResources[i].channelLoad))
    
    print('Test 7-2:  query scaler cu list with allocServiceId')
    allocQuery = xrmAllocationQueryInfo()
    queryScalerCuListRes = xrmCuListResource()
    allocQuery.allocServiceId = scalerCuListRes.cuResources[0].allocServiceId
    ret = xrmAllocationQuery(ctx, allocQuery, queryScalerCuListRes)
    
    if ret != 0:
        print('xrmAllocationQuery: fail to query allocated scaler cu list with allocServiceId')
    else:
        for i in range(queryScalerCuListRes.cuNum):
            print('Query Allocated scaler cu list: cu {}'.format(i))
            print('   xclbinFileName is:  {}'.format(queryScalerCuListRes.cuResources[i].xclbinFileName))
            print('   kernelPluginFileName is:  {}'.format(queryScalerCuListRes.cuResources[i].kernelPluginFileName))
            print('   kernelName is:  {}'.format(queryScalerCuListRes.cuResources[i].kernelName))
            print('   kernelAlias is:  {}'.format(queryScalerCuListRes.cuResources[i].kernelAlias))
            print('   instanceName is:  {}'.format(queryScalerCuListRes.cuResources[i].instanceName))
            print('   cuName is:  {}'.format(queryScalerCuListRes.cuResources[i].cuName))
            print('   deviceId is:  {}'.format(queryScalerCuListRes.cuResources[i].deviceId))
            print('   cuId is:  {}'.format(queryScalerCuListRes.cuResources[i].cuId))
            print('   channelId is:  {}'.format(queryScalerCuListRes.cuResources[i].channelId))
            print('   cuType is:  {}'.format(queryScalerCuListRes.cuResources[i].cuType))
            print('   baseAddr is:  {}'.format(queryScalerCuListRes.cuResources[i].baseAddr))
            print('   membankId is:  {}'.format(queryScalerCuListRes.cuResources[i].membankId))
            print('   membankType is:  {}'.format(queryScalerCuListRes.cuResources[i].membankType))
            print('   membankSize is:  {}'.format(queryScalerCuListRes.cuResources[i].membankSize))
            print('   membankBaseAddr is:  {}'.format(queryScalerCuListRes.cuResources[i].membankBaseAddr))
            print('   allocServiceId is:  {}'.format(queryScalerCuListRes.cuResources[i].allocServiceId))
            print('   poolId is:  {}'.format(queryScalerCuListRes.cuResources[i].poolId))
            print('   channelLoad is:  {}'.format(queryScalerCuListRes.cuResources[i].channelLoad))

    print('Test 7-3:  query scaler cu list with allocServiceId and alias name')
    allocQuery = xrmAllocationQueryInfo()
    queryScalerCuListRes = xrmCuListResource()
    allocQuery.allocServiceId = scalerCuListRes.cuResources[0].allocServiceId
    allocQuery.kernelName = b""
    allocQuery.kernelAlias = b"SCALER_yuv420p_Xilinx"
    ret = xrmAllocationQuery(ctx, allocQuery, queryScalerCuListRes)
    if ret != 0:
        print('xrmAllocationQuery: fail to query allocated scaler cu list with allocServiceId and kernel alias')
    else:
        for i in range(queryScalerCuListRes.cuNum):
            print('Query Allocated scaler cu list: cu {}'.format(i))
            print('   xclbinFileName is:  {}'.format(queryScalerCuListRes.cuResources[i].xclbinFileName))
            print('   kernelPluginFileName is:  {}'.format(queryScalerCuListRes.cuResources[i].kernelPluginFileName))
            print('   kernelName is:  {}'.format(queryScalerCuListRes.cuResources[i].kernelName))
            print('   kernelAlias is:  {}'.format(queryScalerCuListRes.cuResources[i].kernelAlias))
            print('   instanceName is:  {}'.format(queryScalerCuListRes.cuResources[i].instanceName))
            print('   cuName is:  {}'.format(queryScalerCuListRes.cuResources[i].cuName))
            print('   deviceId is:  {}'.format(queryScalerCuListRes.cuResources[i].deviceId))
            print('   cuId is:  {}'.format(queryScalerCuListRes.cuResources[i].cuId))
            print('   channelId is:  {}'.format(queryScalerCuListRes.cuResources[i].channelId))
            print('   cuType is:  {}'.format(queryScalerCuListRes.cuResources[i].cuType))
            print('   baseAddr is:  {}'.format(queryScalerCuListRes.cuResources[i].baseAddr))
            print('   membankId is:  {}'.format(queryScalerCuListRes.cuResources[i].membankId))
            print('   membankType is:  {}'.format(queryScalerCuListRes.cuResources[i].membankType))
            print('   membankSize is:  {}'.format(queryScalerCuListRes.cuResources[i].membankSize))
            print('   membankBaseAddr is:  {}'.format(queryScalerCuListRes.cuResources[i].membankBaseAddr))
            print('   allocServiceId is:  {}'.format(queryScalerCuListRes.cuResources[i].allocServiceId))
            print('   poolId is:  {}'.format(queryScalerCuListRes.cuResources[i].poolId))
            print('   channelLoad is:  {}'.format(queryScalerCuListRes.cuResources[i].channelLoad))

    print('Test 7-4:  query scaler cu list with allocServiceId and kernel name')
    allocQuery = xrmAllocationQueryInfo()
    queryScalerCuListRes = xrmCuListResource()
    allocQuery.allocServiceId = scalerCuListRes.cuResources[0].allocServiceId
    allocQuery.kernelName = b"v_abrscaler_top"
    allocQuery.kernelAlias = b""
    ret = xrmAllocationQuery(ctx, allocQuery, queryScalerCuListRes)
    if ret != 0:
        print('xrmAllocationQuery: fail to query allocated scaler cu list with allocServiceId and kernel name')
    else:
        for i in range(queryScalerCuListRes.cuNum):
            print('Query Allocated scaler cu list: cu {}'.format(i))
            print('   xclbinFileName is:  {}'.format(queryScalerCuListRes.cuResources[i].xclbinFileName))
            print('   kernelPluginFileName is:  {}'.format(queryScalerCuListRes.cuResources[i].kernelPluginFileName))
            print('   kernelName is:  {}'.format(queryScalerCuListRes.cuResources[i].kernelName))
            print('   kernelAlias is:  {}'.format(queryScalerCuListRes.cuResources[i].kernelAlias))
            print('   instanceName is:  {}'.format(queryScalerCuListRes.cuResources[i].instanceName))
            print('   cuName is:  {}'.format(queryScalerCuListRes.cuResources[i].cuName))
            print('   deviceId is:  {}'.format(queryScalerCuListRes.cuResources[i].deviceId))
            print('   cuId is:  {}'.format(queryScalerCuListRes.cuResources[i].cuId))
            print('   channelId is:  {}'.format(queryScalerCuListRes.cuResources[i].channelId))
            print('   cuType is:  {}'.format(queryScalerCuListRes.cuResources[i].cuType))
            print('   baseAddr is:  {}'.format(queryScalerCuListRes.cuResources[i].baseAddr))
            print('   membankId is:  {}'.format(queryScalerCuListRes.cuResources[i].membankId))
            print('   membankType is:  {}'.format(queryScalerCuListRes.cuResources[i].membankType))
            print('   membankSize is:  {}'.format(queryScalerCuListRes.cuResources[i].membankSize))
            print('   membankBaseAddr is:  {}'.format(queryScalerCuListRes.cuResources[i].membankBaseAddr))
            print('   allocServiceId is:  {}'.format(queryScalerCuListRes.cuResources[i].allocServiceId))
            print('   poolId is:  {}'.format(queryScalerCuListRes.cuResources[i].poolId))
            print('   channelLoad is:  {}'.format(queryScalerCuListRes.cuResources[i].channelLoad))

    print('<<<<<<<==  end the xrm allocation query with kernel alias test ===>>>>>>>>')
    print('Test 7-5:  release scaler cu')
    if xrmCuListRelease(ctx, scalerCuListRes):
        print('success to  release scaler cu list')
    else:
        print('fail to release scaler cu list')
        
def xrmCheckCuListAvailableNumUsingAliasTest(ctx):
    print('<<<<<<<==  start the xrm check cu list available num with kernel alias test ===>>>>>>>>')
    print('Test 8-1: check scaler cu list available num with kernel alias')
    scalerCuListProp = xrmCuListProperty()
    scalerCuListProp.cuNum = 4
    scalerCuListProp.sameDevice = False
    for i in range(scalerCuListProp.cuNum):
      scalerCuListProp.cuProps[i].kernelName = b""
      scalerCuListProp.cuProps[i].kernelAlias = b"SCALER_yuv420p_Xilinx"
      scalerCuListProp.cuProps[i].devExcl = False
      scalerCuListProp.cuProps[i].requestLoad = 45
      scalerCuListProp.cuProps[i].poolId = 0
    ret = xrmCheckCuListAvailableNum(ctx, scalerCuListProp)
    if ret <= 0:
      print('xrmCheckCuListAvailableNum: fail to check scaler cu list available num with kernel alias')
    else:
      print('xrmCheckCuListAvailableNum: scaler cu list available num with kernel alias = {}'.format(ret))
    print('Test 8-2: check scaler cu list available num with kernel name')
    scalerCuListProp = xrmCuListProperty()
    scalerCuListProp.cuNum = 4
    scalerCuListProp.sameDevice = False
    for i in range(scalerCuListProp.cuNum):
      scalerCuListProp.cuProps[i].kernelName = b"v_abrscaler_top"
      scalerCuListProp.cuProps[i].kernelAlias = b""
      scalerCuListProp.cuProps[i].devExcl = False
      scalerCuListProp.cuProps[i].requestLoad = 45
      scalerCuListProp.cuProps[i].poolId = 0
    ret = xrmCheckCuListAvailableNum(ctx, scalerCuListProp)
    if ret < 0:
      print('xrmCheckCuListAvailableNum: fail to check scaler cu list available num with kernel name')
    else:
      print('xrmCheckCuListAvailableNum: scaler cu list available num with kernel name = {}'.format(ret))
    print('Test 8-3: check scaler cu list available num with kernel name')
    scalerCuListProp = xrmCuListProperty()
    scalerCuListProp.cuNum = 7
    scalerCuListProp.sameDevice = False
    for i in range(scalerCuListProp.cuNum):
      scalerCuListProp.cuProps[i].kernelName = b"v_abrscaler_top"
      scalerCuListProp.cuProps[i].kernelAlias = b""
      scalerCuListProp.cuProps[i].devExcl = False
      scalerCuListProp.cuProps[i].requestLoad = 65    
      scalerCuListProp.cuProps[i].poolId = 0    
    ret = xrmCheckCuListAvailableNum(ctx, scalerCuListProp)
    if ret < 0:
      print('xrmCheckCuListAvailableNum: fail to check scaler cu list available num with kernel name')
    else:
      print('xrmCheckCuListAvailableNum: scaler cu list available num with kernel name = {}'.format(ret))
    print('<<<<<<<==  end the xrm check cu list available num with kernel alias test ===>>>>>>>>')

def xrmCuPoolReserveAllocReleaseRelinquishTest(ctx):
    print('<<<<<<<==  start the xrm cu list reserve alloc release relinquish test ===>>>>>>>>')
    print('Test 9-1: reserve scaler cu list')

    scalerCuPoolProp = xrmCuPoolProperty()
    scalerCuPoolProp.cuListProp.cuNum = 4
    for i in range(scalerCuPoolProp.cuListProp.cuNum):
        scalerCuPoolProp.cuListProp.cuProps[i].kernelName = b"v_abrscaler_top"
        scalerCuPoolProp.cuListProp.cuProps[i].kernelAlias = b""
        scalerCuPoolProp.cuListProp.cuProps[i].devExcl = False
        scalerCuPoolProp.cuListProp.cuProps[i].requestLoad = 80
    scalerCuPoolProp.cuListNum = 1
    scalerCuPoolProp.xclbinNum = 0

    ret = xrmCheckCuPoolAvailableNum(ctx, scalerCuPoolProp)
    if (ret < 0):
        print('xrmCheckCuPoolAvailableNum: fail to check scaler cu pool available num')
    else:
        print('xrmCheckCuPoolAvailableNum: scaler cu pool available num = {}'.format(ret))

    scalerReservePoolId = xrmCuPoolReserve(ctx, scalerCuPoolProp)
    if scalerReservePoolId == 0:
        print('xrmCuPoolReserve: fail to reserve scaler cu pool')
    else:
        print('xrmCuPoolReserve: reservePoolId = {}'.format(scalerReservePoolId))
    
    scalerCuPoolRes = xrmCuPoolResource()
    print('Test 9-2: xrmReservationQuery')
    ret = xrmReservationQuery(ctx, scalerReservePoolId, scalerCuPoolRes)
    if ret != 0:
        print('xrmReservationQuery: fail to query reserved scaler cu pool')
    else:
        for i in range(scalerCuPoolRes.cuNum):
            print('query the reserved scaler cu pool: cu {}'.format(i))
            print('   xclbinFileName is:  {}'.format(scalerCuPoolRes.cuResources[i].xclbinFileName))
            print('   kernelPluginFileName is:  {}'.format(scalerCuPoolRes.cuResources[i].kernelPluginFileName))
            print('   kernelName is:  {}'.format(scalerCuPoolRes.cuResources[i].kernelName))
            print('   kernelAlias is:  {}'.format(scalerCuPoolRes.cuResources[i].kernelAlias))
            print('   instanceName is:  {}'.format(scalerCuPoolRes.cuResources[i].instanceName))
            print('   cuName is:  {}'.format(scalerCuPoolRes.cuResources[i].cuName))
            print('   deviceId is:  {}'.format(scalerCuPoolRes.cuResources[i].deviceId))
            print('   cuId is:  {}'.format(scalerCuPoolRes.cuResources[i].cuId))
            print('   cuType is:  {}'.format(scalerCuPoolRes.cuResources[i].cuType))
            print('   baseAddr is:  {}'.format(scalerCuPoolRes.cuResources[i].baseAddr))
            print('   membankId is:  {}'.format(scalerCuPoolRes.cuResources[i].membankId))
            print('   membankType is:  {}'.format(scalerCuPoolRes.cuResources[i].membankType))
            print('   membankSize is:  {}'.format(scalerCuPoolRes.cuResources[i].membankSize))
            print('   membankBaseAddr is:  {}'.format(scalerCuPoolRes.cuResources[i].membankBaseAddr))
            print('   poolId is:  {}'.format(scalerCuPoolRes.cuResources[i].poolId))
    scalerCuProp = xrmCuProperty()
    scalerCuRes = xrmCuResource()
    scalerCuProp.kernelName = b"v_abrscaler_top"
    scalerCuProp.kernelAlias = b""
    scalerCuProp.devExcl = False
    scalerCuProp.requestLoad = 45
    scalerCuProp.poolId = scalerReservePoolId
    
    print('Test 9-3: scaler cu prop: kernelName is {}'.format(scalerCuProp.kernelName))
    print('Test 9-3: scaler cu prop: kernelAlias is {}'.format(scalerCuProp.kernelAlias))
    ret = xrmCuAlloc(ctx, scalerCuProp, scalerCuRes)
    if (ret != 0):
        print('xrmCuAlloc: fail to alloc scaler cu from reservation')
    else:
        print('Allocated scaler cu: ')
        print('   xclbinFileName is:  {}'.format(scalerCuRes.xclbinFileName))
        print('   kernelPluginFileName is:  {}'.format(scalerCuRes.kernelPluginFileName))
        print('   kernelName is:  {}'.format(scalerCuRes.kernelName))
        print('   kernelAlias is:  {}'.format(scalerCuRes.kernelAlias))
        print('   instanceName is:  {}'.format(scalerCuRes.instanceName))
        print('   cuName is:  {}'.format(scalerCuRes.cuName))
        print('   deviceId is:  {}'.format(scalerCuRes.deviceId))
        print('   cuId is:  {}'.format(scalerCuRes.cuId))
        print('   channelId is:  {}'.format(scalerCuRes.channelId))
        print('   cuType is:  {}'.format(scalerCuRes.cuType))
        print('   baseAddr is:  {}'.format(scalerCuRes.baseAddr))
        print('   membankId is:  {}'.format(scalerCuRes.membankId))
        print('   membankType is:  {}'.format(scalerCuRes.membankType))
        print('   membankSize is:  {}'.format(scalerCuRes.membankSize))
        print('   membankBaseAddr is:  {}'.format(scalerCuRes.membankBaseAddr))
        print('   allocServiceId is:  {}'.format(scalerCuRes.allocServiceId))
        print('   poolId is:  {}'.format(scalerCuRes.poolId))
        print('   channelLoad is:  {}'.format(scalerCuRes.channelLoad))

    scalerCuListProp = xrmCuListProperty()
    scalerCuListRes = xrmCuListResource()
    scalerCuListProp.cuNum = 4
    for i in range(scalerCuListProp.cuNum):
        scalerCuListProp.cuProps[i].kernelName = b"v_abrscaler_top"
        scalerCuListProp.cuProps[i].kernelAlias = b""
        scalerCuListProp.cuProps[i].devExcl = False
        scalerCuListProp.cuProps[i].requestLoad = 40
        scalerCuListProp.cuProps[i].poolId = scalerReservePoolId
    print('Test 9-4: xrmCuListAlloc')
    ret = xrmCuListAlloc(ctx, scalerCuListProp, scalerCuListRes)
    if (ret != 0):
        print('xrmCuListAlloc: fail to alloc scaler cu list from reservation')
    else:
        for i in range(scalerCuListRes.cuNum):
            print('Allocated scaler cu list: cu {}'.format(i))
            print('   xclbinFileName is:  {}'.format(scalerCuListRes.cuResources[i].xclbinFileName))
            print('   kernelPluginFileName is:  {}'.format(scalerCuListRes.cuResources[i].kernelPluginFileName))
            print('   kernelName is:  {}'.format(scalerCuListRes.cuResources[i].kernelName))
            print('   kernelAlias is:  {}'.format(scalerCuListRes.cuResources[i].kernelAlias))
            print('   instanceName is:  {}'.format(scalerCuListRes.cuResources[i].instanceName))
            print('   cuName is:  {}'.format(scalerCuListRes.cuResources[i].cuName))
            print('   deviceId is:  {}'.format(scalerCuListRes.cuResources[i].deviceId))
            print('   cuId is:  {}'.format(scalerCuListRes.cuResources[i].cuId))
            print('   channelId is:  {}'.format(scalerCuListRes.cuResources[i].channelId))
            print('   cuType is:  {}'.format(scalerCuListRes.cuResources[i].cuType))
            print('   baseAddr is:  {}'.format(scalerCuListRes.cuResources[i].baseAddr))
            print('   membankId is:  {}'.format(scalerCuListRes.cuResources[i].membankId))
            print('   membankType is:  {}'.format(scalerCuListRes.cuResources[i].membankType))
            print('   membankSize is:  {}'.format(scalerCuListRes.cuResources[i].membankSize))
            print('   membankBaseAddr is:  {}'.format(scalerCuListRes.cuResources[i].membankBaseAddr))
            print('   allocServiceId is:  {}'.format(scalerCuListRes.cuResources[i].allocServiceId))
            print('   poolId is:  {}'.format(scalerCuListRes.cuResources[i].poolId))
            print('   channelLoad is:  {}'.format(scalerCuListRes.cuResources[i].channelLoad))

    print('Test 9-5:   release scaler cu')
    if xrmCuRelease(ctx, scalerCuRes):
        print('success to release scaler cu')
    else:
        print('fail to release scaler cu')

    print('Test 9-6:   release scaler cu list')
    if xrmCuListRelease(ctx, scalerCuListRes):
        print('success to release scaler cu list')
    else:
        print('fail to release scaler cu list')

    print('Test 9-7:  relinquish scaler cu pool')
    if xrmCuPoolRelinquish(ctx, scalerReservePoolId):
        print('success to relinquish encoder cu pool')
    else:
        print('fail to relinquish encoder cu pool')

def xrmLoadUnloadXclbinTest(ctx):
    print('<<<<<<<==  start the xrm load xclbin test ===>>>>>>>>')

    xclbinFileName = b"/tmp/xclbins/test_xrm.xclbin"
    devId = 0;
    print('Test 10-1: unload xclbin')
    print('  devId is {}'.format(devId))
    ret = xrmUnloadOneDevice(ctx, devId)
    if (ret == 0):
        print('success to unload xclbin from device {}'.format(devId))
    else:
        print('fail to unload xclbin from device {}'.format(devId))

    devId = 0;
    print('Test 10-2: load xclbin file to specified device')
    print('  xclbin file name is {}'.format(xclbinFileName))
    print('  devId is {}'.format(devId))
    ret = xrmLoadOneDevice(ctx, devId, xclbinFileName)
    if (ret >= 0):
        print('success to load xclbin to device {}'.format(ret))
    else:
        print('fail to load xclbin to device')

    devId = -1;
    print('Test 10-3: load xclbin file to any device')
    print('  xclbin file name is {}'.format(xclbinFileName))
    print('  devId is {}'.format(devId))
    ret = xrmLoadOneDevice(ctx, devId, xclbinFileName)
    if (ret >= 0):
        print('success to load xclbin to device {}'.format(ret))
    else:
        print('fail to load xclbin to device')

def xrmCheckDaemonTest(ctx):
    print('<<<<<<<==  start the xrm check daemon test ===>>>>>>>>')

    print('Test 11-1: check whether daemon is running')
    ret = xrmIsDaemonRunning(ctx)
    if (ret == True):
        print('XRM daemon is running')
    else:
        print('XRM daemon is NOT running')

    print('<<<<<<<==  end the xrm check daemon test ===>>>>>>>>')

def xrmPluginTest(ctx):
    print('<<<<<<<==  start the xrm plugin test ===>>>>>>>>')

    print('Test 12-1: test the plugin function')
    xrmPluginName = b"xrmPluginExample"
    param = xrmPluginFuncParam()
    for funcId in range(8):
        param.input = b"xrmPluginExampleFunc_" + str(funcId)
        if (xrmExecPluginFunc(ctx, xrmPluginName, funcId, param) != xrmError.XRM_SUCCESS.value):
            print('test plugin function {} fail to run the function'.format(funcId))
        else:
            print('test plugin function {}, success to run the function, input: {}, output: {}'.format(funcId, param.input, param.output))

    print('<<<<<<<==  end the xrm plugin test ===>>>>>>>>')

def xrmUdfCuGroupTest(ctx):
    print('<<<<<<<==  start the xrm user defined cu group test ===>>>>>>>>')

    print('Test 13-1: test the user defined cu group declaration function')
    udfCuGroupProp = xrmUdfCuGroupProperty()

    # declare user defined cu group from same device
    print('Test 13-2: user defined cu group from same device declaration')
    udfCuGroupName = b"udfCuGroupSameDevice"
    udfCuGroupProp.optionUdfCuListNum = 2
    # define the first option cu list
    udfCuGroupProp.optionUdfCuListProps[0].cuNum = 5
    udfCuGroupProp.optionUdfCuListProps[0].sameDevice = True
    for cuIdx in range(5):
        udfCuGroupProp.optionUdfCuListProps[0].udfCuProps[cuIdx].cuName = b"krnl_ngcodec_pistachio_enc:krnl_0"
        udfCuGroupProp.optionUdfCuListProps[0].udfCuProps[cuIdx].devExcl = False;
        udfCuGroupProp.optionUdfCuListProps[0].udfCuProps[cuIdx].requestLoad = 30;
    # define the second option cu list
    udfCuGroupProp.optionUdfCuListProps[1].cuNum = 5
    udfCuGroupProp.optionUdfCuListProps[1].sameDevice = True
    for cuIdx in range(5):
        udfCuGroupProp.optionUdfCuListProps[1].udfCuProps[cuIdx].cuName = b"v_abrscaler_top:krnl_1"
        udfCuGroupProp.optionUdfCuListProps[1].udfCuProps[cuIdx].devExcl = False;
        udfCuGroupProp.optionUdfCuListProps[1].udfCuProps[cuIdx].requestLoad = 20;
    ret = xrmUdfCuGroupDeclare(ctx, udfCuGroupProp, udfCuGroupName)
    if (ret == xrmError.XRM_SUCCESS.value):
        print('xrmUdfCuGroupDeclare(): user defined cu group from same device declaration success')
    else:
        print('xrmUdfCuGroupDeclare(): user defined cu group from same device declaration fail')

    # declare user defined cu group from any device
    print('Test 13-3: user defined cu group from any device declaration')
    udfCuGroupName = b"udfCuGroupAnyDevice"
    udfCuGroupProp.optionUdfCuListNum = 2
    # define the first option cu list
    udfCuGroupProp.optionUdfCuListProps[0].cuNum = 5
    udfCuGroupProp.optionUdfCuListProps[0].sameDevice = False
    for cuIdx in range(5):
        udfCuGroupProp.optionUdfCuListProps[0].udfCuProps[cuIdx].cuName = b"krnl_ngcodec_pistachio_enc:krnl_0"
        udfCuGroupProp.optionUdfCuListProps[0].udfCuProps[cuIdx].devExcl = False;
        udfCuGroupProp.optionUdfCuListProps[0].udfCuProps[cuIdx].requestLoad = 30;
    # define the second option cu list
    udfCuGroupProp.optionUdfCuListProps[1].cuNum = 5
    udfCuGroupProp.optionUdfCuListProps[1].sameDevice = False
    for cuIdx in range(5):
        udfCuGroupProp.optionUdfCuListProps[1].udfCuProps[cuIdx].cuName = b"v_abrscaler_top:krnl_1"
        udfCuGroupProp.optionUdfCuListProps[1].udfCuProps[cuIdx].devExcl = False;
        udfCuGroupProp.optionUdfCuListProps[1].udfCuProps[cuIdx].requestLoad = 20;
    ret = xrmUdfCuGroupDeclare(ctx, udfCuGroupProp, udfCuGroupName)
    if (ret == xrmError.XRM_SUCCESS.value):
        print('xrmUdfCuGroupDeclare(): user defined cu group from any device declaration success')
    else:
        print('xrmUdfCuGroupDeclare(): user defined cu group from any device declaration fail')

    print('Test 13-4: check user defined cu group (same device) available num')
    cuGroupRes = xrmCuGroupResource()
    cuGroupProp = xrmCuGroupProperty()
    cuGroupProp.udfCuGroupName = b"udfCuGroupSameDevice"
    cuGroupProp.poolId = 0
    ret = xrmCheckCuGroupAvailableNum(ctx, cuGroupProp)
    if (ret < 0):
        print('xrmCheckCuGroupAvaiableNum: fail to check user defined cu group (same device) available num')
    else:
        print('xrmCheckCuGroupAvaiableNum: user defined cu group (same device) available num = {}'.format(ret))

    print('Test 13-5: alloc user defined cu group (same device)')
    ret = xrmCuGroupAlloc(ctx, cuGroupProp, cuGroupRes)
    if (ret != xrmError.XRM_SUCCESS.value):
        print('xrmCuGroupAlloc: fail to alloc user defined cu group (same device)')
    else:
        for i in range(cuGroupRes.cuNum):
            print('Allocated user defined cu group (same device): cu {}'.format(i))
            print('   xclbinFileName is:  {}'.format(cuGroupRes.cuResources[i].xclbinFileName))
            print('   kernelPluginFileName is:  {}'.format(cuGroupRes.cuResources[i].kernelPluginFileName))
            print('   kernelName is:  {}'.format(cuGroupRes.cuResources[i].kernelName))
            print('   kernelAlias is:  {}'.format(cuGroupRes.cuResources[i].kernelAlias))
            print('   instanceName is:  {}'.format(cuGroupRes.cuResources[i].instanceName))
            print('   cuName is:  {}'.format(cuGroupRes.cuResources[i].cuName))
            print('   deviceId is:  {}'.format(cuGroupRes.cuResources[i].deviceId))
            print('   cuId is:  {}'.format(cuGroupRes.cuResources[i].cuId))
            print('   channelId is:  {}'.format(cuGroupRes.cuResources[i].channelId))
            print('   cuType is:  {}'.format(cuGroupRes.cuResources[i].cuType))
            print('   baseAddr is:  {}'.format(cuGroupRes.cuResources[i].baseAddr))
            print('   membankId is:  {}'.format(cuGroupRes.cuResources[i].membankId))
            print('   membankType is:  {}'.format(cuGroupRes.cuResources[i].membankType))
            print('   membankSize is:  {}'.format(cuGroupRes.cuResources[i].membankSize))
            print('   membankBaseAddr is:  {}'.format(cuGroupRes.cuResources[i].membankBaseAddr))
            print('   allocServiceId is:  {}'.format(cuGroupRes.cuResources[i].allocServiceId))
            print('   poolId is:  {}'.format(cuGroupRes.cuResources[i].poolId))
            print('   channelLoad is:  {}'.format(cuGroupRes.cuResources[i].channelLoad))

    print('Test 13-5: release user defined cu group (same device)')
    if (xrmCuGroupRelease(ctx, cuGroupRes)):
        print('success to release user defined cu group (same device)')
    else:
        print('fail to release user defined cu group (same device)')

    print('Test 13-6: check user defined cu group (any device) available num')
    cuGroupProp.udfCuGroupName = b"udfCuGroupAnyDevice"
    cuGroupProp.poolId = 0;
    ret = xrmCheckCuGroupAvailableNum(ctx, cuGroupProp)
    if (ret < 0):
        print('xrmCheckCuGroupAvaiableNum: fail to check user defined cu group (any device) available num')
    else:
        print('xrmCheckCuGroupAvaiableNum: user defined cu group (any device) available num = {}'.format(ret))

    print('Test 13-7: alloc user defined cu group (any device)')
    ret = xrmCuGroupAlloc(ctx, cuGroupProp, cuGroupRes)
    if (ret != xrmError.XRM_SUCCESS.value):
        print('xrmCuGroupAlloc: fail to alloc user defined cu group (any device)')
    else:
        for i in range(cuGroupRes.cuNum):
            print('Allocated user defined cu group (any device): cu {}'.format(i))
            print('   xclbinFileName is:  {}'.format(cuGroupRes.cuResources[i].xclbinFileName))
            print('   kernelPluginFileName is:  {}'.format(cuGroupRes.cuResources[i].kernelPluginFileName))
            print('   kernelName is:  {}'.format(cuGroupRes.cuResources[i].kernelName))
            print('   kernelAlias is:  {}'.format(cuGroupRes.cuResources[i].kernelAlias))
            print('   instanceName is:  {}'.format(cuGroupRes.cuResources[i].instanceName))
            print('   cuName is:  {}'.format(cuGroupRes.cuResources[i].cuName))
            print('   deviceId is:  {}'.format(cuGroupRes.cuResources[i].deviceId))
            print('   cuId is:  {}'.format(cuGroupRes.cuResources[i].cuId))
            print('   channelId is:  {}'.format(cuGroupRes.cuResources[i].channelId))
            print('   cuType is:  {}'.format(cuGroupRes.cuResources[i].cuType))
            print('   baseAddr is:  {}'.format(cuGroupRes.cuResources[i].baseAddr))
            print('   membankId is:  {}'.format(cuGroupRes.cuResources[i].membankId))
            print('   membankType is:  {}'.format(cuGroupRes.cuResources[i].membankType))
            print('   membankSize is:  {}'.format(cuGroupRes.cuResources[i].membankSize))
            print('   membankBaseAddr is:  {}'.format(cuGroupRes.cuResources[i].membankBaseAddr))
            print('   allocServiceId is:  {}'.format(cuGroupRes.cuResources[i].allocServiceId))
            print('   poolId is:  {}'.format(cuGroupRes.cuResources[i].poolId))
            print('   channelLoad is:  {}'.format(cuGroupRes.cuResources[i].channelLoad))

    print('Test 13-8: release user defined cu group (any device)')
    if (xrmCuGroupRelease(ctx, cuGroupRes)):
        print('success to release user defined cu group (any device)')
    else:
        print('fail to release user defined cu group (any device)')

    print('Test 13-9: user defined cu group from same device undeclaration')
    udfCuGroupName = b"udfCuGroupSameDevice"
    ret = xrmUdfCuGroupUndeclare(ctx, udfCuGroupName)
    if (ret == xrmError.XRM_SUCCESS.value):
        print('xrmUdfCuGroupUndeclare(): user defined cu group from same device undeclaration success')
    else:
        print('xrmUdfCuGroupUndeclare(): user defined cu group from same device undeclaration fail')

    print('Test 13-10: user defined cu group from any device undeclaration')
    udfCuGroupName = b"udfCuGroupAnyDevice"
    ret = xrmUdfCuGroupUndeclare(ctx, udfCuGroupName)
    if (ret == xrmError.XRM_SUCCESS.value):
        print('xrmUdfCuGroupUndeclare(): user defined cu group from any device undeclaration success')
    else:
        print('xrmUdfCuGroupUndeclare(): user defined cu group from any device undeclaration fail')

    print('<<<<<<<==  end the xrm user defined cu group test ===>>>>>>>>')

if __name__ == '__main__':
    ctx = xrmCreateContext(1)
    if ctx:
      print('Test 0-1: create context success')
    else:
      print('Test 0-1: create context failed')

    xrmCheckDaemonTest(ctx)
    xrmCuAllocReleaseTest(ctx)
    xrmCuListAllocReleaseTest(ctx)
    xrmSoftCuAllocReleaseTest(ctx)
    xrmCuListAllocReleaseFromSameDevTest(ctx)
    xrmCuAllocReleaseUsingAliasTest(ctx)
    xrmCuAllocReleaseUsingKernelNameAndAliasTest(ctx)
    xrmCuAllocQueryReleaseUsingAliasTest(ctx)
    xrmCheckCuListAvailableNumUsingAliasTest(ctx)
    xrmCuPoolReserveAllocReleaseRelinquishTest(ctx)
    xrmLoadUnloadXclbinTest(ctx)
    xrmCheckDaemonTest(ctx)
    xrmPluginTest(ctx)
    xrmUdfCuGroupTest(ctx)

    print('Test 0-2: destroy context')
    if xrmDestroyContext(ctx) != xrmError.XRM_SUCCESS.value:
        print('Test 0-2: destroy context failed')
    else:
        print('Test 0-2: destroy context success')
