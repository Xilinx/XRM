CC = gcc
CFLAGS = $(shell pkg-config --cflags libxrm)
CFLAGS += -Wall -O2 -g -I./src
LDFLAGS = $(shell pkg-config --libs libxrm)
RM = rm -f
TARGET = example_xrm_api_block_wrapper
SRCS = src/example_xrm_api_block_wrapper.c

.PHONY: all
all: ${TARGET}

$(TARGET): $(SRCS)
	$(CC) $(SRCS) $(CFLAGS) -o $(TARGET) $(LDFLAGS)

.PHONY: clean
clean:
	-${RM} ${TARGET}