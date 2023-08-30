#!/bin/bash

#
# Copyright (C) 2023, Advanced Micro Devices, Inc. All rights reserved.
#

echo "systemctl stop xrmd"
systemctl stop xrmd

echo "systemctl start xrmd"
systemctl start xrmd
