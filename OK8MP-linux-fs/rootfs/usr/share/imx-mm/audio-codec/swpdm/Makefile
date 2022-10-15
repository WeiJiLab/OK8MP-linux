# Copyright 2020 NXP.
#

CFLAGS += -DLINUX -fPIC
INCLUDES = `pkg-config --cflags libimxswpdm`
LIBS = `pkg-config --libs libimxswpdm`
TARGET = imx-sw-pdm
TEST_OBJS = ./src/test.o \
	    ./src/wave_func.o

all: PDM2PCM

PDM2PCM: $(TEST_OBJS)
	$(CC) $(CCFLAGS) $(INCLUDES) $(TEST_OBJS) $(LIBS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

clean:
	rm -f ./src/*.o
	rm -f $(TARGET)

help:
	@echo "targets are:"
	@echo "\timx-sw-pdm - build unit test"
	@echo "\tall        - build the above"
