#
# Copyright (C) 2023, Advanced Micro Devices, Inc. All rights reserved.
#
CC = gcc
CFLAGS = $(shell pkg-config --cflags libxrm)
CFLAGS += -Wall -O2 -g -I./src
LDFLAGS = $(shell pkg-config --libs libxrm)
RM = rm -f
TARGET = example_test_xrm_api
SRCS = src/example_test_xrm_api.c

.PHONY: all
all: ${TARGET}

$(TARGET): $(SRCS)
	$(CC) $(SRCS) $(CFLAGS) -o $(TARGET) $(LDFLAGS) -Wl,-rpath,'/opt/xilinx/xrm/lib'

.PHONY: clean
clean:
	-${RM} ${TARGET}