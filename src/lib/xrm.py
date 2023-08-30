#
# Copyright (C) 2019-2020, Xilinx Inc - All rig#ts reserved
#
# Copyright (C) 2023, Advanced Micro Devices, Inc. All rights reserved.
#
# Xilinx Resouce Managemen#
#
# Licensed under the Apache License, Version 2.0 (the "License"). You may
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

from ctypes import *
from enum import Enum
import os

xrmLib = cdll.LoadLibrary(os.environ['XILINX_XRM']+"/lib/libxrm.so")

class xrmError(Enum):
    XRM_SUCCESS = 0
    XRM_ERROR = -1
    XRM_ERROR_INVALID = -2
    XRM_ERROR_NO_DEV = -3
    XRM_ERROR_NO_KERNEL = -4
    XRM_ERROR_NO_CHAN = -5

class xrmCuProperty(Structure):
  _fields_ = [ ("kernelName", c_char*256),
              ("kernelAlias", c_char*256),
              ("devExcl", c_bool),
              ("requestLoad", c_int32),
              ("poolId", c_uint64),
              ("extData", c_uint8*64)
            ]
  
class xrmCuListProperty(Structure):
  _fields_ = [ ("cuProps", xrmCuProperty*16),
              ("cuNum", c_int32),
              ("sameDevice", c_bool),
              ("extData", c_uint8*64)
            ]

class xrmCuType:
    XRM_CU_NULL         = 0
    XRM_CU_IPKERNEL     = 1
    XRM_CU_SOFTKERNEL   = 2

class xrmCuPoolProperty(Structure):
  _fields_ = [ ("cuListProp", xrmCuListProperty),
              ("cuListNum", c_int32),
              ("xclbinUuid", c_uint8*16),
              ("xclbinNum", c_int32),
              ("extData", c_uint8*64)
            ]

class xrmUdfCuProperty(Structure):
  _fields_ = [ ("cuName", c_char*256),
              ("devExcl", c_bool),
              ("requestLoad", c_int32),
              ("extData", c_uint8*64)
            ]

class xrmUdfCuListProperty(Structure):
  _fields_ = [ ("udfCuProps", xrmUdfCuProperty*16),
              ("cuNum", c_int32),
              ("sameDevice", c_bool),
              ("extData", c_uint8*64)
            ]

class xrmUdfCuGroupProperty(Structure):
  _fields_ = [ ("optionUdfCuListProps", xrmUdfCuListProperty*8),
              ("optionUdfCuListNum", c_int32),
              ("extData", c_uint8*64)
            ]

class xrmCuGroupProperty(Structure):
  _fields_ = [ ("udfCuGroupName", c_char*256),
              ("poolId", c_uint64),
              ("extData", c_uint8*64)
            ]

class xrmCuResource(Structure):
    _fields_ = [ ("xclbinFileName", c_char*256),
                ("kernelPluginFileName", c_char*256),
                ("kernelName", c_char*256),
                ("kernelAlias", c_char*256),
                ("instanceName", c_char*256),
                ("cuName", c_char*256),
                ("uuid", c_ubyte*16),
                ("deviceId", c_int32),
                ("cuId", c_int32),
                ("channelId", c_int32),
                ("cuType", c_uint32),
                ("baseAddr", c_uint64),
                ("membankId", c_uint32),
                ("membankType", c_uint32),
                ("membankSize", c_uint64),
                ("membankBaseAddr", c_uint64),
                ("allocServiceId", c_uint64),
                ("channelLoad", c_int32),
                ("poolId", c_uint64),
                ("extData", c_uint8*64)
              ]

class xrmCuListResource(Structure):
  _fields_ = [ ("cuResources", xrmCuResource*16),
              ("cuNum", c_int32),
              ("extData", c_uint8*64)
            ]

class xrmCuGroupResource(Structure):
  _fields_ = [ ("cuResources", xrmCuResource*16),
              ("cuNum", c_int32),
              ("extData", c_uint8*64)
            ]

class xrmCuPoolResource(Structure):
  _fields_ = [ ("cuResources", xrmCuResource*128),
              ("cuNum", c_int32),
              ("extData", c_uint8*64)
            ]

class xrmAllocationQueryInfo(Structure):
  _fields_ = [ ("allocServiceId", c_uint64),
              ("kernelName", c_char*256),
              ("kernelAlias", c_char*256),
              ("extData", c_uint8*64)
            ]

class xrmCuStat(Structure):
  _fields_ = [ ("isBusy", c_bool),
              ("usedLoad", c_int32),
              ("extData", c_uint8*64)
            ]

class xrmPluginFuncParam(Structure):
  _fields_ = [ ("input", c_char*16384),
              ("output", c_char*16384)
            ]

def xrmCreateContext(xrmApiVersion):
    xrmLib.xrmCreateContext.argtypes = [c_uint32]
    xrmLib.xrmCreateContext.restype = c_void_p
    return xrmLib.xrmCreateContext(xrmApiVersion)

def xrmDestroyContext(context):
    xrmLib.xrmDestroyContext.argtypes = [c_void_p]
    xrmLib.xrmDestroyContext.restype = c_int32
    return xrmLib.xrmDestroyContext(context)

def xrmCuAlloc(context, cuProp, cuRes):
    xrmLib.xrmCuAlloc.argtypes = [c_void_p, POINTER(xrmCuProperty), POINTER(xrmCuResource)]
    xrmLib.xrmCuAlloc.restype = c_int32
    return xrmLib.xrmCuAlloc(context, cuProp, cuRes)

def xrmCuListAlloc(context, cuListProp, cuListRes):
    xrmLib.xrmCuListAlloc.argtypes = [c_void_p, POINTER(xrmCuListProperty), POINTER(xrmCuListResource)]
    xrmLib.xrmCuListAlloc.restype = c_int32
    return xrmLib.xrmCuListAlloc(context, cuListProp, cuListRes)

def xrmCuRelease(context, cuRes):
    xrmLib.xrmCuRelease.argtypes = [c_void_p, POINTER(xrmCuResource)]
    xrmLib.xrmCuRelease.restype = c_bool
    return xrmLib.xrmCuRelease(context, cuRes)

def xrmCuListRelease(context, cuListRes):
    xrmLib.xrmCuListRelease.argtypes = [c_void_p, POINTER(xrmCuListResource)]
    xrmLib.xrmCuListRelease.restype = c_bool  
    return xrmLib.xrmCuListRelease(context, cuListRes)

def xrmCuGetMaxCapacity(context, cuProp):
    xrmLib.xrmCuGetMaxCapacity.argtypes = [c_void_p, POINTER(xrmCuProperty)]
    xrmLib.xrmCuGetMaxCapacity.restype = c_uint64
    return xrmLib.xrmCuGetMaxCapacity(context, cuProp)

def xrmCuCheckStatus(context, cuRes, cuStat):
    xrmLib.xrmCuCheckStatus.argtypes = [c_void_p, POINTER(xrmCuResource), POINTER(xrmCuStat)]
    xrmLib.xrmCuCheckStatus.restype = c_uint64  
    return xrmLib.xrmCuCheckStatus(context, cuRes, cuStat)

def xrmAllocationQuery(context, allocQuery, cuListRes):
    xrmLib.xrmAllocationQuery.argtypes = [c_void_p, POINTER(xrmAllocationQueryInfo), POINTER(xrmCuListResource)]
    xrmLib.xrmAllocationQuery.restype = c_int32
    return xrmLib.xrmAllocationQuery(context, allocQuery, cuListRes)

def xrmCheckCuListAvailableNum(context, cuListProp):
    xrmLib.xrmCheckCuListAvailableNum.argtypes = [c_void_p, POINTER(xrmCuListProperty)]
    xrmLib.xrmCheckCuListAvailableNum.restype = c_int32
    return xrmLib.xrmCheckCuListAvailableNum(context, cuListProp)

def xrmCheckCuPoolAvailableNum(context, cuPoolProp):
    xrmLib.xrmCheckCuPoolAvailableNum.argtypes = [c_void_p, POINTER(xrmCuPoolProperty)]
    xrmLib.xrmCheckCuPoolAvailableNum.restype = c_int32
    return xrmLib.xrmCheckCuPoolAvailableNum(context, cuPoolProp)

def xrmCuPoolReserve(context, cuPoolProp):
    xrmLib.xrmCuPoolReserve.argtypes = [c_void_p, POINTER(xrmCuPoolProperty)]
    xrmLib.xrmCuPoolReserve.restype = c_uint64
    return xrmLib.xrmCuPoolReserve(context, cuPoolProp)

def xrmCuPoolRelinquish(context, reservationId):
    xrmLib.xrmCuPoolRelinquish.argtypes = [c_void_p, c_uint64]
    xrmLib.xrmCuPoolRelinquish.restype = c_bool
    return xrmLib.xrmCuPoolRelinquish(context, reservationId)

def xrmReservationQuery(context, reservationId, cuPoolRes):
    xrmLib.xrmReservationQuery.argtypes = [c_void_p, c_uint64, POINTER(xrmCuPoolResource)]
    xrmLib.xrmReservationQuery.restype = c_int32
    return xrmLib.xrmReservationQuery(context, reservationId, cuPoolRes)

def xrmIsDaemonRunning(context):
    xrmLib.xrmIsDaemonRunning.argtypes = [c_void_p]
    xrmLib.xrmIsDaemonRunning.restype = c_bool
    return xrmLib.xrmIsDaemonRunning(context)

def xrmLoadOneDevice(context, deviceId, xclbinFileName):
    xrmLib.xrmLoadOneDevice.argtypes = [c_void_p, c_int32, POINTER(c_char)]
    xrmLib.xrmLoadOneDevice.restype = c_int32
    return xrmLib.xrmLoadOneDevice(context, deviceId, xclbinFileName)

def xrmUnloadOneDevice(context, deviceId):
    xrmLib.xrmUnloadOneDevice.argtypes = [c_void_p, c_int32]
    xrmLib.xrmUnloadOneDevice.restype = c_int32
    return xrmLib.xrmUnloadOneDevice(context, deviceId)

def xrmUdfCuGroupDeclare(context, udfCuGroupProp, udfCuGroupName):
    xrmLib.xrmUdfCuGroupDeclare.argtypes = [c_void_p, POINTER(xrmUdfCuGroupProperty), POINTER(c_char)]
    xrmLib.xrmUdfCuGroupDeclare.restype = c_int32
    return xrmLib.xrmUdfCuGroupDeclare(context, udfCuGroupProp, udfCuGroupName)

def xrmUdfCuGroupUndeclare(context, udfCuGroupName):
    xrmLib.xrmUdfCuGroupUndeclare.argtypes = [c_void_p, POINTER(c_char)]
    xrmLib.xrmUdfCuGroupUndeclare.restype = c_int32
    return xrmLib.xrmUdfCuGroupUndeclare(context, udfCuGroupName)

def xrmCheckCuGroupAvailableNum(context, cuGroupProp):
    xrmLib.xrmCheckCuGroupAvailableNum.argtypes = [c_void_p, POINTER(xrmCuGroupProperty)]
    xrmLib.xrmCheckCuGroupAvailableNum.restype = c_int32
    return xrmLib.xrmCheckCuGroupAvailableNum(context, cuGroupProp)

def xrmCuGroupAlloc(context, cuGroupProp, cuGroupRes):
    xrmLib.xrmCuGroupAlloc.argtypes = [c_void_p, POINTER(xrmCuGroupProperty), POINTER(xrmCuGroupResource)]
    xrmLib.xrmCuGroupAlloc.restype = c_int32
    return xrmLib.xrmCuGroupAlloc(context, cuGroupProp, cuGroupRes)

def xrmCuGroupRelease(context, cuGroupRes):
    xrmLib.xrmCuGroupRelease.argtypes = [c_void_p, POINTER(xrmCuGroupResource)]
    xrmLib.xrmCuGroupRelease.restype = c_int32
    return xrmLib.xrmCuGroupRelease(context, cuGroupRes)

def xrmExecPluginFunc(context, xrmPluginName, funcId, param):
    xrmLib.xrmExecPluginFunc.argtypes = [c_void_p, POINTER(c_char), c_int32, POINTER(xrmPluginFuncParam)]
    xrmLib.xrmExecPluginFunc.restype = c_int32
    return xrmLib.xrmExecPluginFunc(context, xrmPluginName, funcId, param)
