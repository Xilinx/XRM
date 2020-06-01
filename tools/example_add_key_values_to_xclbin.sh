#!/bin/bash

XCLBINUTIL=/opt/xilinx/xrt/bin/xclbinutil
$XCLBINUTIL -i test_orig.xclbin --key-value=USER:krnl_ngcodec_pistachio_enc:'{"alias" : "ENCODER_VP9_NGCodec", "plugin": "libngcvp9.so", "maxCapacity": "124416000"}' -o test_1.xclbin

$XCLBINUTIL -i test_1.xclbin --key-value=USER:v_abrscaler_top:'{"alias": "SCALER_yuv420p_Xilinx", "plugin": "libxabrscaler.so", "maxCapacity": "248832000"}' -o test_2.xclbin

mv test_2.xclbin test_xrm.xclbin
rm test_1.xclbin
