Test Instructions
-----------------
Once XRM package is installed, the related files will be installed under
``/opt/xilinx/xrm/``. Please follow the instructions to run the test.

How to Start XRM and Run Test
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

XRM provides a daemon xrmd which services the request from xrm command or host applications. Once
the daemon is started, xrmd will use XRT API to operate FPGA devices on the system. So please refer
to the following instructions to stop xrmd if user wants to install / reinstall / update XRT after
xrm daemon started.

Step 1: To set the running environment
......................................
xrt (related files will be installed under ``/opt/xilinx/xrt/``) and xrm running
environment need to be set before test.
::
    source /opt/xilinx/xrt/setup.sh
    source /opt/xilinx/xrm/setup.sh

Step 2: To start the xrmd
.........................
Daemon xrmd need to be started for test.

To stop xrmd if it's started before test:
::
    sudo /opt/xilinx/xrm/tools/stop_xrmd.sh

To start xrmd:
::
    sudo /opt/xilinx/xrm/tools/start_xrmd.sh

Or to restart xrmd which will stop then start xrmd:
::
    sudo /opt/xilinx/xrm/tools/restart_xrmd.sh

To check status of xrmd:
::
    sudo systemctl status xrmd

Step 3: To load/unload xclbin
.............................
The input is from json file. Please refer to example json file under ``/opt/xilinx/xrm/test/``
on how to specify the xclbin file and device for load/unload operation. The right xclbin file
need to be prepared before loading operation.
::
    cd /opt/xilinx/xrm/test/
    xrmadm list_cmd.json (To check the system result)
    xrmadm load_devices_cmd.json (To load xclbin files to devices)
    xrmadm list_cmd.json (To check the load result)
    xrmadm unload_devices_cmd.json (To unload xclbin from devices)
    xrmadm list_cmd.json (To check the unload result)

NOTE: Please refer to following session if you need to pad extra meta data into the xclbin file.

Step 4: To run XRM host application example
...........................................
There are some examples under ``/opt/xilinx/xrm/test/`` for user reference. The example_1 is
to show how to use the XRM APIs. The detailed definition of the APIs can be found in
``/opt/xilinx/xrm/include/xrm.h``
::
    cd /opt/xilinx/xrm/test/example_1
    source /opt/xilinx/xrm/setup.sh
    make (To build the binary)
    ./example_test_xrm_api (to run the test)

How to Pad Meta Data into Xclbin File
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Please use/refer the ``/opt/xilinx/xrm/tools/example_add_key_values_to_xclbin.sh`` to pad meta data into
the target xclbin file.
