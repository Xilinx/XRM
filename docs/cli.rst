..
   Copyright (C) 2023, Advanced Micro Devices, Inc. All rights reserved.

XRM Command Line Tool
---------------------

XRM provides command line tool ``xrmadm`` to configure the FPGA hardware.
``xrmadm`` will handle the xclbin and plugin load/unload. It will take JSON file as input.

FPGA Device Operation
~~~~~~~~~~~~~~~~~~~~~

Load xclbin to devices
......................

.. code-block:: bash

    source /opt/xilinx/xrm/setup.sh
    cd /opt/xilinx/xrm/test
    xrmadm load_devices_cmd.json

Input JSON file example ``load_devices_cmd.json``:

.. code-block:: json

    {
        "request": {
            "name": "load",
            "requestId": 1,
            "parameters": [
                {
                "device": 0,
                "xclbin": "/tmp/xclbins/test_xrm.xclbin"
                },
                {
                "device": 1,
                "xclbin": "/tmp/xclbins/test_xrm.xclbin"
                },
                {
                "device": 2,
                "xclbin": "/tmp/xclbins/test_xrm.xclbin"
                },
                {
                "device": 3,
                "xclbin": "/tmp/xclbins/test_xrm.xclbin"
                },
                {
                "device": 4,
                "xclbin": "/tmp/xclbins/test_xrm.xclbin"
                },
                {
                "device": 5,
                "xclbin": "/tmp/xclbins/test_xrm.xclbin"
                }
            ]
        }
    }

Output example:

.. code-block:: json

    {
        "response": {
            "name": "load",
            "requestId": "1",
            "status": "ok"
        }
    }


Unload xclbin from devices
..........................

.. code-block:: bash

    source /opt/xilinx/xrm/setup.sh
    cd /opt/xilinx/xrm/test
    xrmadm unload_devices_cmd.json

Input JSON file example ``unload_devices_cmd.json``:

.. code-block:: json

    {
        "request": {
            "name": "unload",
            "requestId": 1,
            "parameters": {
                "device": [0,1,2,3,4,5]
            }
        }
    }

Output example:

.. code-block:: json

    {
        "response": {
            "name": "unload",
            "requestId": "1",
            "status": "ok",
            "data": {
                "ok": "unload completed"
            }
        }
    }


Disable devices with XRM
.........................

.. code-block:: bash

    source /opt/xilinx/xrm/setup.sh
    cd /opt/xilinx/xrm/test
    xrmadm disable_devices_cmd.json

Input JSON file example ``disable_devices_cmd.json``:

.. code-block:: json

    {
        "request": {
            "name": "disableDevices",
            "requestId": 1,
            "parameters": {
                "device": [0,1]
            }
        }
    }

Output example:

.. code-block:: json

    {
        "response": {
            "name": "disableDevices",
            "requestId": "1",
            "status": "ok",
            "data": {
                "ok": "disable devices completed"
            }
        }
    }


Enable devices with XRM
........................

.. code-block:: bash

    source /opt/xilinx/xrm/setup.sh
    cd /opt/xilinx/xrm/test
    xrmadm enable_devices_cmd.json

Input JSON file example ``enable_devices_cmd.json``:

.. code-block:: json

    {
        "request": {
            "name": "enableDevices",
            "requestId": 1,
            "parameters": {
                "device": [0,1]
            }
        }
    }

Output example:

.. code-block:: json

    {
        "response": {
            "name": "enableDevices",
            "requestId": "1",
            "status": "ok",
            "data": {
                "ok": "enable devices completed"
            }
        }
    }

XRM Plugin Operation
~~~~~~~~~~~~~~~~~~~~

Load XRM plugins
................

.. code-block:: bash

    source /opt/xilinx/xrm/setup.sh
    cd /opt/xilinx/xrm/test
    xrmadm load_xrm_plugins_cmd.json

Input JSON file example ``load_xrm_plugins_cmd.json``:

.. code-block:: json

    {
        "request": {
            "name": "loadXrmPlugins",
            "requestId": 1,
            "parameters": [
                {
                "xrmPluginFileName": "/opt/xilinx/xrm/plugin/libxrmpluginexample.so",
                "xrmPluginName": "xrmPluginExample"
                }
            ]
        }
    }

Output example:

.. code-block:: json

    {
        "response": {
            "name": "loadXrmPlugins",
            "requestId": "1",
            "status": "ok"
        }
    }


Unload XRM plugins
..................

.. code-block:: bash

    source /opt/xilinx/xrm/setup.sh
    cd /opt/xilinx/xrm/test
    xrmadm unload_xrm_plugins_cmd.json

Input JSON file example ``unload_xrm_plugins_cmd.json``:

.. code-block:: json

    {
        "request": {
            "name": "unloadXrmPlugins",
            "requestId": 1,
            "parameters": [
                {
                "xrmPluginName": "xrmPluginExample"
                }
            ]
        }
    }

Output example:

.. code-block:: json

    {
        "response": {
            "name": "unloadXrmPlugins",
            "requestId": "1",
            "status": "ok"
        }
    }


Resource List Operation
~~~~~~~~~~~~~~~~~~~~~~~

List Devices and Plugins
........................

.. code-block:: bash

    source /opt/xilinx/xrm/setup.sh
    cd /opt/xilinx/xrm/test
    xrmadm list_cmd.json

Input JSON file example ``list_cmd.json``:

.. code-block:: json

    {
        "request": {
            "name": "list",
            "requestId": 1
        }
    }

Output example:

.. code-block:: json

    {
        "response": {
            "name": "list",
            "requestId": "1",
            "status": "ok",
            "data": {
                "deviceNumber": "6",

                ......

                "device_2": {
                    "dsaName    ": "xilinx_u30_gen3x4_base_1",
                    "xclbinName ": "/tmp/xclbins/test_xrm.xclbin",
                    "uuid       ": "d1e0415e27d349a29f0fdfa92e69f8ee",
                    "isExclusive": "false",
                    "cuNumber   ": "70",

                    ......

                    "cu_2": {
                        "cuId         ": "2",
                        "cuType       ": "IP Kernel",
                        "kernelName   ": "lookahead",
                        "kernelAlias  ": "LOOKAHEAD_MPSOC",
                        "instanceName ": "lookahead_2",
                        "cuName       ": "lookahead:lookahead_2",
                        "kernelPlugin ": "/opt/xilinx/xma_plugins/libxlookahead.so",
                        "maxCapacity  ": "497664000",
                        "numChanInuse ": "0",
                        "usedLoad     ": "0 of 1000000",
                        "reservedLoad ": "0 of 1000000",
                        "resrvUsedLoad": "0 of 1000000"
                    },

                    ......

                },

                ......

                "xrmPluginNumber": "1",
                "xrmPlugin_0": {
                    "xrmPluginName    ": "xrmPluginExample",
                    "xrmPluginFileName": "/opt/xilinx/xrm/plugin/libxrmpluginexample.so"
                }
            }
        }
    }

