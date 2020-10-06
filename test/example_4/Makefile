# target and command definitions
CXX = gcc
RM = rm -f

# Host settings for OPENCL API
TARGET_FLAGS = -DTARGET_FLOW_HW=1 -DTARGET_FLOW_HW_EMU=0 -DTARGET_FLOW_SW_EMU=0
HOST_CFLAGS =  -I./xcl2 -DDSA64
HOST_CFLAGS += $(TARGET_FLAGS) -DTARGET_DEVICE=\"$(DEVICE)\"
HOST_CFLAGS += -O0 -g -Wall -fmessage-length=0 -std=c++14 -I${GITHUB_COMMON_REPO}/libs/xcl2 -lxilinxopencl -lpthread -lrt -lstdc++ -I${XILINX_XRT}/include -L${XILINX_XRT}/lib -I${XILINX_SCOUT}/lnx64/tools/opencv -I${XILINX_SCOUT}/include -L${XILINX_SCOUT}/lib/lnx64.o

# Host files
TARGET = example_xrm_host_opencl
SRC = src/example_xcl2.cpp src/example_xrm_host_opencl.cpp

# Host rules
.PHONY: all
all: ${TARGET}

$(TARGET): $(SRC)
	$(CXX) $+ $(HOST_CFLAGS) -I/opt/xilinx/xrm/include -o $(TARGET) -lxrm -Wl,-rpath,'/opt/xilinx/xrm/lib' -L/opt/xilinx/xrm/lib
	@echo "INFO: Compiled Host Executable: $(TARGET)"

clean:
	$(RM) $(TARGET)
