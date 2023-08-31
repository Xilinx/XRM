..
   Copyright (C) 2019-2022, Xilinx Inc - All rights reserved

   Copyright (C) 2023, Advanced Micro Devices, Inc. All rights reserved.
  
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
  
       http://www.apache.org/licenses/LICENSE-2.0
  
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

.. meta::
   :keywords: Compute Unit, Kernel, Resource Manager, FPGA Resource Manager
   :description: Xilinx FPGA Resource Manager (XRM) Release Notes.
   :xlnxdocumentclass: Document
   :xlnxdocumenttype: Tutorials


Release Note
============

Version 1.7
-----------

XRM - Xilinx FPGA Resource manager is the software to manage all the FPGA hardware on the system. All the Kernels (IP Kernel or Soft Kernel) on FPGA board are abstracted as one CU resource in XRM. XRM providers interface to allocate and release CU. The smallest allocation unit is channel, which is percentage of one CU. XRM providers command line tool (xrmadm) to download xclbin to device. During the download processing, XRM builds up the resource database. XRM daemon (xrmd) is running at background as core engine to support resource reservation, relinquish, allocation and release.

