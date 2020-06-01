.. _Prerequisites.rst:

Prerequisites
-------------

Host Platform
~~~~~~~~~~~~~

Current supported host platform is x86_64.

OS Distributions
~~~~~~~~~~~~~~~~

XRM requires Linux kernel 3.10+ and GCC with C++14 features.

XRM has been tested on the following OS distributions:

1. RHEL/CentOS 7.4
2. RHEL/CentOS 7.6
3. Ubuntu 16.04.4 LTS
4. Ubuntu 18.04.1 LTS

CentOS/RHEL 7.4, 7.6 require additional steps to install C++14 tool set and a few dependent libraries. Please use the provided script ``tools/xrmdeps.sh`` to install the dependencies for both CentOS/RHEL and Ubuntu distributions.

To deploy XRM, simply install the proper RPM or DEB package obtained from Xilinx.

To build a develop version of XRM, please follow the instructions in ``docs/Build.rst``.
