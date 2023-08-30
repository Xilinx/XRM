..
   Copyright (C) 2023, Advanced Micro Devices, Inc. All rights reserved.

XRM Application Examples
------------------------

XRM provides following examples to show how to use XRM APIs for application development.

Example 1: XRM APIs usage
~~~~~~~~~~~~~~~~~~~~~~~~~
This is an example to test APIs provided by XRM. The source code and Makefile can be found from XRM git repo ``./test/example_1``.

Example 2: XRM resource blocking allocate wrapper
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This is an example to demo how to develop blocking function for resource allocation. With this implementation, it provides the interface to allocate resource with blocking which means the caller will wait until resource is available. The source code and Makefile can be found from XRM git repo ``./test/example_2``.

Example 3: XRM application using xcl API
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This is an example to demo how to develop an XRM application which will use xcl API. The source code and Makefile can be found from XRM git repo ``./test/example_3``.

Example 4: XRM application using OpenCL API
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This is an example to demo how to develop an XRM application which will use OpenCL API. The source code and Makefile can be found from XRM git repo ``./test/example_4``.

Example 5: XRM Python APIs and test
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This is an example to demo how to develop an XRM application with Python language. The source code can be found from XRM git repo ``./test/example_5``. ``python example_test_xrm_api.py`` can run the test example.

Example 6: XRM Go APIs and test
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This is an example to demo how to develop an XRM application with Go language. The source code can be found from XRM git repo ``./test/example_6``. ``go run example_test_xrm_api.go`` can run the test example.

Example 7: XRM cu allocate and release performance test
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This is an example to demo how to measure the performance of cu allocate and release. The source code and Makefile can be found from XRM git repo ``./test/example_7``.

Plugin Example: How to build one XRM plugin
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Developers are welcome to reference to the XRM plugin example and  build more specified XRM plugin based on requirement. The plugin example can be found from XRM git repo ``./src/plugin``.

