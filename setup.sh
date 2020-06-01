#!/bin/bash
# Script to setup environment for XRM

# Check OS version requirement
OSDIST=`lsb_release -i |awk -F: '{print tolower($2)}' | tr -d ' \t'`
OSREL=`lsb_release -r |awk -F: '{print tolower($2)}' |tr -d ' \t' | awk -F. '{print $1*100+$2}'`

if [[ $OSDIST == "ubuntu" ]]; then
    if (( $OSREL < 1604 )); then
        echo "ERROR: Ubuntu release version must be 16.04 or later"
        return 1
    fi
fi

if [[ $OSDIST == "centos" ]] || [[ $OSDIST == "redhat"* ]]; then
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
