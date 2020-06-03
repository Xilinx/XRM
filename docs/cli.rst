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
                "device_0": {
                    "dsaName    ": "xilinx_twitch_dynamic_5_1",
                    "xclbinName ": "\/tmp\/xclbins\/test_xrm.xclbin",
                    "uuid       ": "6b7b13fbff2649048d744307dd711466",
                    "isExclusive": "false",
                    "cuNumber   ": "2",
                    "cu_0": {
                        "cuId         ": "0",
                        "cuType       ": "IP Kernel",
                        "kernelName   ": "krnl_ngcodec_pistachio_enc",
                        "kernelAlias  ": "ENCODER_VP9_NGCodec",
                        "instanceName ": "krnl_0",
                        "cuName       ": "krnl_ngcodec_pistachio_enc:krnl_0",
                        "kernelPlugin ": "libngcvp9.so",
                        "maxCapacity  ": "124416000",
                        "numChanInuse ": "0",
                        "usedLoad     ": "0",
                        "reservedLoad ": "0"
                    },
                    "cu_1": {
                        "cuId         ": "1",
                        "cuType       ": "IP Kernel",
                        "kernelName   ": "v_abrscaler_top",
                        "kernelAlias  ": "",
                        "instanceName ": "krnl_1",
                        "cuName       ": "v_abrscaler_top:krnl_1",
                        "kernelPlugin ": "",
                        "maxCapacity  ": "",
                        "numChanInuse ": "0",
                        "usedLoad     ": "0",
                        "reservedLoad ": "0"
                    }
                },
                "device_1": {
                    "dsaName    ": "xilinx_twitch_dynamic_5_1",
                    "xclbinName ": "\/tmp\/xclbins\/test_xrm.xclbin",
                    "uuid       ": "6b7b13fbff2649048d744307dd711466",
                    "isExclusive": "false",
                    "cuNumber   ": "2",
                    "cu_0": {
                        "cuId         ": "0",
                        "cuType       ": "IP Kernel",
                        "kernelName   ": "krnl_ngcodec_pistachio_enc",
                        "kernelAlias  ": "ENCODER_VP9_NGCodec",
                        "instanceName ": "krnl_0",
                        "cuName       ": "krnl_ngcodec_pistachio_enc:krnl_0",
                        "kernelPlugin ": "libngcvp9.so",
                        "maxCapacity  ": "124416000",
                        "numChanInuse ": "0",
                        "usedLoad     ": "0",
                        "reservedLoad ": "0"
                    },
                    "cu_1": {
                        "cuId         ": "1",
                        "cuType       ": "IP Kernel",
                        "kernelName   ": "v_abrscaler_top",
                        "kernelAlias  ": "",
                        "instanceName ": "krnl_1",
                        "cuName       ": "v_abrscaler_top:krnl_1",
                        "kernelPlugin ": "",
                        "maxCapacity  ": "",
                        "numChanInuse ": "0",
                        "usedLoad     ": "0",
                        "reservedLoad ": "0"
                    }
                },
                "device_2": {
                    "dsaName    ": "xilinx_twitch_dynamic_5_1",
                    "xclbinName ": "\/tmp\/xclbins\/test_xrm.xclbin",
                    "uuid       ": "6b7b13fbff2649048d744307dd711466",
                    "isExclusive": "false",
                    "cuNumber   ": "2",
                    "cu_0": {
                        "cuId         ": "0",
                        "cuType       ": "IP Kernel",
                        "kernelName   ": "krnl_ngcodec_pistachio_enc",
                        "kernelAlias  ": "ENCODER_VP9_NGCodec",
                        "instanceName ": "krnl_0",
                        "cuName       ": "krnl_ngcodec_pistachio_enc:krnl_0",
                        "kernelPlugin ": "libngcvp9.so",
                        "maxCapacity  ": "124416000",
                        "numChanInuse ": "0",
                        "usedLoad     ": "0",
                        "reservedLoad ": "0"
                    },
                    "cu_1": {
                        "cuId         ": "1",
                        "cuType       ": "IP Kernel",
                        "kernelName   ": "v_abrscaler_top",
                        "kernelAlias  ": "",
                        "instanceName ": "krnl_1",
                        "cuName       ": "v_abrscaler_top:krnl_1",
                        "kernelPlugin ": "",
                        "maxCapacity  ": "",
                        "numChanInuse ": "0",
                        "usedLoad     ": "0",
                        "reservedLoad ": "0"
                    }
                },
                "device_3": {
                    "dsaName    ": "xilinx_twitch_dynamic_5_1",
                    "xclbinName ": "\/tmp\/xclbins\/test_xrm.xclbin",
                    "uuid       ": "6b7b13fbff2649048d744307dd711466",
                    "isExclusive": "false",
                    "cuNumber   ": "2",
                    "cu_0": {
                        "cuId         ": "0",
                        "cuType       ": "IP Kernel",
                        "kernelName   ": "krnl_ngcodec_pistachio_enc",
                        "kernelAlias  ": "ENCODER_VP9_NGCodec",
                        "instanceName ": "krnl_0",
                        "cuName       ": "krnl_ngcodec_pistachio_enc:krnl_0",
                        "kernelPlugin ": "libngcvp9.so",
                        "maxCapacity  ": "124416000",
                        "numChanInuse ": "0",
                        "usedLoad     ": "0",
                        "reservedLoad ": "0"
                    },
                    "cu_1": {
                        "cuId         ": "1",
                        "cuType       ": "IP Kernel",
                        "kernelName   ": "v_abrscaler_top",
                        "kernelAlias  ": "",
                        "instanceName ": "krnl_1",
                        "cuName       ": "v_abrscaler_top:krnl_1",
                        "kernelPlugin ": "",
                        "maxCapacity  ": "",
                        "numChanInuse ": "0",
                        "usedLoad     ": "0",
                        "reservedLoad ": "0"
                    }
                },
                "device_4": {
                    "dsaName    ": "xilinx_twitch_dynamic_5_1",
                    "xclbinName ": "\/tmp\/xclbins\/test_xrm.xclbin",
                    "uuid       ": "6b7b13fbff2649048d744307dd711466",
                    "isExclusive": "false",
                    "cuNumber   ": "2",
                    "cu_0": {
                        "cuId         ": "0",
                        "cuType       ": "IP Kernel",
                        "kernelName   ": "krnl_ngcodec_pistachio_enc",
                        "kernelAlias  ": "ENCODER_VP9_NGCodec",
                        "instanceName ": "krnl_0",
                        "cuName       ": "krnl_ngcodec_pistachio_enc:krnl_0",
                        "kernelPlugin ": "libngcvp9.so",
                        "maxCapacity  ": "124416000",
                        "numChanInuse ": "0",
                        "usedLoad     ": "0",
                        "reservedLoad ": "0"
                    },
                    "cu_1": {
                        "cuId         ": "1",
                        "cuType       ": "IP Kernel",
                        "kernelName   ": "v_abrscaler_top",
                        "kernelAlias  ": "",
                        "instanceName ": "krnl_1",
                        "cuName       ": "v_abrscaler_top:krnl_1",
                        "kernelPlugin ": "",
                        "maxCapacity  ": "",
                        "numChanInuse ": "0",
                        "usedLoad     ": "0",
                        "reservedLoad ": "0"
                    }
                },
                "device_5": {
                    "dsaName    ": "xilinx_twitch_dynamic_5_1",
                    "xclbinName ": "\/tmp\/xclbins\/test_xrm.xclbin",
                    "uuid       ": "6b7b13fbff2649048d744307dd711466",
                    "isExclusive": "false",
                    "cuNumber   ": "2",
                    "cu_0": {
                        "cuId         ": "0",
                        "cuType       ": "IP Kernel",
                        "kernelName   ": "krnl_ngcodec_pistachio_enc",
                        "kernelAlias  ": "ENCODER_VP9_NGCodec",
                        "instanceName ": "krnl_0",
                        "cuName       ": "krnl_ngcodec_pistachio_enc:krnl_0",
                        "kernelPlugin ": "libngcvp9.so",
                        "maxCapacity  ": "124416000",
                        "numChanInuse ": "0",
                        "usedLoad     ": "0",
                        "reservedLoad ": "0"
                    },
                    "cu_1": {
                        "cuId         ": "1",
                        "cuType       ": "IP Kernel",
                        "kernelName   ": "v_abrscaler_top",
                        "kernelAlias  ": "",
                        "instanceName ": "krnl_1",
                        "cuName       ": "v_abrscaler_top:krnl_1",
                        "kernelPlugin ": "",
                        "maxCapacity  ": "",
                        "numChanInuse ": "0",
                        "usedLoad     ": "0",
                        "reservedLoad ": "0"
                    }
                },
                "xrmPluginNumber": "1",
                "xrmPlugin_0": {
                    "xrmPluginName    ": "xrmPluginExample",
                    "xrmPluginFileName": "\/opt\/xilinx\/xrm\/plugin\/libxrmpluginexample.so"
                }
            }
        }
    }

