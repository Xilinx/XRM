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
	$(CC) $(SRCS) $(CFLAGS) -o $(TARGET) $(LDFLAGS)

.PHONY: clean
clean:
	-${RM} ${TARGET}