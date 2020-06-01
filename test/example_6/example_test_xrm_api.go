package main

//#cgo CFLAGS: -I/opt/xilinx/xrm/include
//#cgo LDFLAGS: -L/opt/xilinx/xrm/lib -lxrm
//
//#include <stdbool.h>
//#include "xrm.h"
import "C"
import "fmt"

func xrmCuAllocReleaseTest (ctx _Ctype_xrmContext){
  scalerCuProp :=&C.struct_xrmCuProperty{}
  scalerCuRes  :=&C.struct_xrmCuResource{}
  scalerCuStat :=&C.struct_xrmCuStat{}

  scalerKernelName := [256]C.char{}
  scalerKernelNameString := "v_abrscaler_top"
  for i := 0; i < len(scalerKernelNameString); i++ {
    scalerKernelName[i] = C.char(scalerKernelNameString[i])
  }
  scalerCuProp.kernelName = scalerKernelName
  scalerCuProp.devExcl = false
  scalerCuProp.requestLoad = 45

  ret := C.xrmCuAlloc(ctx, scalerCuProp, scalerCuRes)
  if ret == 0 {
    fmt.Println("Allocated scaler cu:")
    fmt.Println("   xclbinFileName is: ", C.GoString(&scalerCuRes.xclbinFileName[0]))
    fmt.Println("   kernelPluginFileName is: ", C.GoString(&scalerCuRes.kernelPluginFileName[0]))
    fmt.Println("   kernelName is: ", C.GoString(&scalerCuRes.kernelName[0]))
    fmt.Println("   instanceName is: ", C.GoString(&scalerCuRes.instanceName[0]))
    fmt.Println("   cuName is: ", C.GoString(&scalerCuRes.cuName[0]))
    fmt.Println("   kernelAlias is: ", C.GoString(&scalerCuRes.kernelAlias[0]))
    fmt.Println("   deviceId is: ", scalerCuRes.deviceId)
    fmt.Println("   cuId is: ", scalerCuRes.cuId)
    fmt.Println("   channelId is: ", scalerCuRes.channelId)
    fmt.Println("   cuType is: ", scalerCuRes.cuType)
    fmt.Println("   baseAddr is: ", scalerCuRes.baseAddr)
    fmt.Println("   membankId is: ", scalerCuRes.membankId)
    fmt.Println("   membankType is: ", scalerCuRes.membankType)
    fmt.Println("   membankSize is: ", scalerCuRes.membankSize)
    fmt.Println("   membankBaseAddr is: ", scalerCuRes.membankBaseAddr)
    fmt.Println("   allocServiceId is: ", scalerCuRes.allocServiceId)
    fmt.Println("   poolId is: ", scalerCuRes.poolId)
    fmt.Println("   channelLoad is: ", scalerCuRes.channelLoad)
  } else {
    fmt.Println("xrmCuAlloc: fail to alloc scaler cu")
  }
  maxCapacity := C.xrmCuGetMaxCapacity(ctx, scalerCuProp)
  fmt.Println("v_abrscaler_top cu max capacity is: ", maxCapacity)
  ret = C.xrmCuCheckStatus(ctx, scalerCuRes, scalerCuStat)
  if ret == 0 {
    if (scalerCuStat.isBusy){
        fmt.Println("   isBusy:  true")
    } else {
        fmt.Println("   isBusy:  false")
    }
    fmt.Println("   usedLoad: ", scalerCuStat.usedLoad)
  } else {
    fmt.Println("xrmCuCheckStatus: fail to check status of scaler cu")
  }

  encCuProp :=&C.struct_xrmCuProperty{}
  encCuRes  :=&C.struct_xrmCuResource{}
  encKernelName := [256]C.char{}
  encKernelNameString := "krnl_ngcodec_pistachio_enc"
  for i := 0; i < len(encKernelNameString); i++ {
    encKernelName[i] = C.char(encKernelNameString[i])
  }
  encCuProp.kernelName = encKernelName
  encCuProp.devExcl = false
  encCuProp.requestLoad = 55
  maxCapacity = C.xrmCuGetMaxCapacity(ctx, encCuProp)
  fmt.Println("krnl_ngcodec_pistachio_enc cu max capacity is: ", maxCapacity)
  ret = C.xrmCuAlloc(ctx, encCuProp, encCuRes)
  if ret == 0 {
    fmt.Println("Allocated ENCODER cu:")
    fmt.Println("   xclbinFileName is: ", C.GoString(&encCuRes.xclbinFileName[0]))
    fmt.Println("   kernelPluginFileName is: ", C.GoString(&encCuRes.kernelPluginFileName[0]))
    fmt.Println("   kernelName is: ", C.GoString(&encCuRes.kernelName[0]))
    fmt.Println("   instanceName is: ", C.GoString(&encCuRes.instanceName[0]))
    fmt.Println("   cuName is: ", C.GoString(&encCuRes.cuName[0]))
    fmt.Println("   kernelAlias is: ", C.GoString(&encCuRes.kernelAlias[0]))
    fmt.Println("   deviceId is: ", encCuRes.deviceId)
    fmt.Println("   cuId is: ", encCuRes.cuId)
    fmt.Println("   channelId is: ", encCuRes.channelId)
    fmt.Println("   cuType is: ", encCuRes.cuType)
    fmt.Println("   baseAddr is: ", encCuRes.baseAddr)
    fmt.Println("   membankId is: ", encCuRes.membankId)
    fmt.Println("   membankType is: ", encCuRes.membankType)
    fmt.Println("   membankSize is: ", encCuRes.membankSize)
    fmt.Println("   membankBaseAddr is: ", encCuRes.membankBaseAddr)
    fmt.Println("   allocServiceId is: ", encCuRes.allocServiceId)
    fmt.Println("   poolId is: ", scalerCuRes.poolId)
    fmt.Println("   channelLoad is: ", scalerCuRes.channelLoad)
  } else {
    fmt.Println("xrmCuAlloc: fail to alloc encoder cu")
  }

  if (C.xrmCuRelease(ctx, scalerCuRes)){
    fmt.Println("success to  release scaler cu")
  } else {
    fmt.Println("fail to release scaler cu")
  }

  if (C.xrmCuRelease(ctx, encCuRes)){
    fmt.Println("success to  release encoder cu")
  } else {
    fmt.Println("fail to release encoder cu")
  }
}

func xrmCuListAllocReleaseTest(ctx _Ctype_xrmContext){
  scalerCuListProp :=&C.struct_xrmCuListProperty{}
  scalerCuListRes :=&C.struct_xrmCuListResource{}
  scalerCuListProp.cuNum = 4
  kernelName := [256]C.char{}
  kernelNameString := "v_abrscaler_top"
  for i := 0; i < len(kernelNameString); i++ {
    kernelName[i] = C.char(kernelNameString[i])
  }
  for i := 0; i < 4; i++ {
    scalerCuListProp.cuProps[i].kernelName = kernelName
    scalerCuListProp.cuProps[i].devExcl = false
    scalerCuListProp.cuProps[i].requestLoad = 45
  }
  ret := C.xrmCuListAlloc(ctx, scalerCuListProp, scalerCuListRes)
  if ret == 0 {
    for i := 0; i < 4; i++ {
      fmt.Println("Allocated scaler cu list: cu ", i)
      fmt.Println("   xclbinFileName is: ", C.GoString(&scalerCuListRes.cuResources[i].xclbinFileName[0]))
      fmt.Println("   kernelPluginFileName is: ", C.GoString(&scalerCuListRes.cuResources[i].kernelPluginFileName[0]))
      fmt.Println("   kernelName is: ", C.GoString(&scalerCuListRes.cuResources[i].kernelName[0]))
      fmt.Println("   instanceName is: ", C.GoString(&scalerCuListRes.cuResources[i].instanceName[0]))
      fmt.Println("   cuName is: ", C.GoString(&scalerCuListRes.cuResources[i].cuName[0]))
      fmt.Println("   kernelAlias is: ", C.GoString(&scalerCuListRes.cuResources[i].kernelAlias[0]))
      fmt.Println("   deviceId is: ", scalerCuListRes.cuResources[i].deviceId)
      fmt.Println("   cuId is: ", scalerCuListRes.cuResources[i].cuId)
      fmt.Println("   channelId is: ", scalerCuListRes.cuResources[i].channelId)
      fmt.Println("   cuType is: ", scalerCuListRes.cuResources[i].cuType)
      fmt.Println("   baseAddr is: ", scalerCuListRes.cuResources[i].baseAddr)
      fmt.Println("   membankId is: ", scalerCuListRes.cuResources[i].membankId)
      fmt.Println("   membankType is: ", scalerCuListRes.cuResources[i].membankType)
      fmt.Println("   membankSize is: ", scalerCuListRes.cuResources[i].membankSize)
      fmt.Println("   poolId is: ", scalerCuListRes.cuResources[i].poolId)
      fmt.Println("   allocServiceId is: ", scalerCuListRes.cuResources[i].allocServiceId)
      fmt.Println("   channelLoad is: ", scalerCuListRes.cuResources[i].channelLoad)
    }
  } else {
    fmt.Println("xrmCuListAlloc: fail to alloc scaler cu list")
  }
  if (C.xrmCuListRelease(ctx, scalerCuListRes)){
    fmt.Println("success to release scaler cu list")
  } else {
    fmt.Println("fail to release scaler cu list")
  }
}

func xrmCheckCuListAvailableNum(ctx _Ctype_xrmContext){
  scalerCuListProp :=&C.struct_xrmCuListProperty{}
  scalerCuListProp.cuNum = 4
  scalerCuListProp.sameDevice = false
  kernelName := [256]C.char{}
  kernelNameString := "v_abrscaler_top"
  for i := 0; i < len(kernelNameString); i++ {
    kernelName[i] = C.char(kernelNameString[i])
  }
  for i := 0; i < 4; i++ {
    scalerCuListProp.cuProps[i].kernelName = kernelName
    scalerCuListProp.cuProps[i].devExcl = false
    scalerCuListProp.cuProps[i].requestLoad = 45
  }
  ret := C.xrmCheckCuListAvailableNum(ctx, scalerCuListProp)
  if ret < 0 {
    fmt.Println("xrmCuListAlloc: fail to alloc scaler cu list")
  } else {
    fmt.Println("xrmCheckCuListAvailableNum: scaler cu list available num with kernel name = ", ret)
  }

  scalerCuListProp.cuNum = 7
  scalerCuListProp.sameDevice = false
  for i := 0; i < 7; i++ {
    scalerCuListProp.cuProps[i].kernelName = kernelName
    scalerCuListProp.cuProps[i].devExcl = false
    scalerCuListProp.cuProps[i].requestLoad = 65
  }
  ret = C.xrmCheckCuListAvailableNum(ctx, scalerCuListProp)
  if ret < 0 {
    fmt.Println("xrmCuListAlloc: fail to alloc scaler cu list")
  } else {
    fmt.Println("xrmCheckCuListAvailableNum: scaler cu list available num with kernel name = ", ret)
  }
}

func main(){
  fmt.Println("xrmCreateContext: create xrm context")
  ctx := C.xrmCreateContext(1)
  xrmCuAllocReleaseTest(ctx)
  xrmCuListAllocReleaseTest(ctx)
  xrmCheckCuListAvailableNum(ctx)
  C.xrmDestroyContext(ctx)
  fmt.Println("xrmDestroyContext: destroy xrm context")
}
