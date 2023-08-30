#!/bin/bash
# Script to setup environment for XRM

# Check OS version requirement
OSDIST=`cat /etc/os-release | grep -i "^ID=" | awk -F= '{print $2}'`
if [[ $OSDIST == "centos" ]]; then
    OSREL=`cat /etc/redhat-release | awk '{print $4}' | tr -d '"' | awk -F. '{print $1*100+$2}'`
else
    OSREL=`cat /etc/os-release | grep -i "^VERSION_ID=" | awk -F= '{print $2}' | tr -d '"' | awk -F. '{print $1*100+$2}'`
fi

if [[ $OSDIST == "ubuntu" ]]; then
    if (( $OSREL < 1604 )); then
        echo "ERROR: Ubuntu release version must be 16.04 or later"
        return 1
    fi
fi

if [[ $OSDIST == "centos" ]] || [[ $OSDIST == "rhel"* ]]; then
    if (( $OSREL < 704 )); then
        echo "ERROR: Centos or RHEL release version must be 7.4 or later"
        return 1
    fi
fi

XILINX_XRM="/opt/xilinx/xrm"

export XILINX_XRM
export LD_LIBRARY_PATH=$XILINX_XRM/lib:$LD_LIBRARY_PATH
export PATH=$XILINX_XRM/bin:$PATH

echo "XILINX_XRM      : $XILINX_XRM"
echo "PATH            : $PATH"
echo "LD_LIBRARY_PATH : $LD_LIBRARY_PATH"
