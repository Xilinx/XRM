.. _build.rst:

Building and Installing XRM
---------------------------

XRM
~~~

XRM requires C++14 compiler, a few development libraries and Xilinx
Runtime (XRT). Please install the necessary tools and dependencies using the
provided ``tools/xrmdeps.sh``.

The ``xrmdeps.sh`` script installs the standard distribution packages
for the tools and libraries XRM depends on. If any system libraries
XRM depends on (for example XRT, Boost libraries) are updated then XRM must be
rebuilt.

On RHEL7.x/CentOS7.x use devtoolset to switch to C++14 development
environment. This step is not applicable to Ubuntu which already has
C++14 capable GCC.

::

   scl enable devtoolset-6 bash

Build XRM
.........

::

   source /opt/xilinx/xrt/setup.sh
   ./build.sh

``build.sh`` script builds for both Debug and Release profiles.  On
RHEL/CentOS, if ``build.sh`` was accidentally run prior to enabling
the devtoolset, then it is necessary to clean stale files makefiles by
running ``build.sh clean`` prior to the next build.

Build RPM package on RHEL/CentOS or DEB package on Ubuntu
.........................................................

The package is automatically built for the ``Release``
version but not for the ``Debug`` version. To build Debug package ::

   cd ./Debug
   make package

Install the XRM RPM package
............................

Install by providing a full path to the RPM package, for example, from
inside either the ``Release`` or ``Debug`` directory according to
purpose with (the actual package name might differ) ::

   sudo yum reinstall ./Release/xrm_version.rpm


Install the XRM DEB package
...........................

Install by providing a full path to the DEB package, for example, from
inside either the ``Release`` or ``Debug`` directory according to
purpose with (the actual package name might differ) ::

   sudo apt install --reinstall ./Release/xrm_version.deb
