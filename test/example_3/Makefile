# target and command definitions
CXX = gcc
RM = rm -f

# Host settings for XRT API
CXXFLAGS = -O0 -g -std=c++14 -fPIC -fopenmp -Wextra -Wall -Wno-ignored-attributes -Wno-unused-parameter -Wno-unused-variable
CXXFLAGS += -I$(XILINX_XRT)/include -I./src
LDFLAGS = -L$(XILINX_XRT)/lib -lz -lstdc++ -lrt -pthread -lxrt_core -ldl -luuid 

# Host files
TARGET = example_xrm_host_xcl
SRC = src/example_xrm_host_xcl.cpp

# Host rules
.PHONY: all
all: ${TARGET}

$(TARGET): $(SRC)
	$(CXX) $+ $(CXXFLAGS) -I/opt/xilinx/xrm/include -o $(TARGET) $(LDFLAGS) -lxrm -Wl,-rpath,'/opt/xilinx/xrm/lib' -L/opt/xilinx/xrm/lib
	@echo "INFO: Compiled Host Executable: $(TARGET)"
	
clean:
	$(RM) $(TARGET)
