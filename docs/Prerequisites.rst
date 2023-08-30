..
   Copyright (C) 2023, Advanced Micro Devices, Inc. All rights reserved.

Prerequisites
-------------

Host Platform
~~~~~~~~~~~~~

Current supported host platform is x86_64.

OS Distributions
~~~~~~~~~~~~~~~~

XRM requires Linux kernel 3.10+ and GCC with C++14 features.

XRM has been tested on the following OS distributions:

1. RHEL/CentOS 7.x
2. AL2 (Amazon Linux 2)
3. Ubuntu 16.04 LTS
4. Ubuntu 18.04 LTS
5. Ubuntu 20.04 LTS
6. RHEL/CentOS 8.1

CentOS/RHEL 7.x require additional steps to install C++14 tool set and a few dependent libraries. Please use the provided script ``tools/xrmdeps.sh`` to install the dependencies for both CentOS/RHEL and Ubuntu distributions.

To deploy XRM, simply install the proper RPM or DEB package obtained from Xilinx.

To build a develop version of XRM, please follow the instructions in ``docs/Build.rst``.
