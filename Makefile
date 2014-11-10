
CROSS_COMPILE=
STRIP	= $(CROSS_COMPILE)strip
CC	= $(CROSS_COMPILE)gcc
TARGET=arm-linux

IFLAGS = -I../include 
CFLAGS +=-g -O2
LDFLAGS += -L./


BIN_PATH = bin

OBJ=  tcpclient.o communicate.o getAllChannelSignal.o getOutRate.o

.PHONY: all clean distclean

all:build install

build:
	$(CC) $(CFLAGS) $(IFLAGS) -c tcpclient.c
	$(CC) $(CFLAGS) $(IFLAGS) -c communicate.c
	$(CC) $(CFLAGS) $(IFLAGS) -c getAllChannelSignal.c
	$(CC) $(CFLAGS) $(IFLAGS) -c getOutRate.c
	$(CC) $(OBJ) -o tcpclient $(LDFLAGS) $(DBS_LIB) $(BOARDAPI_LD_FLAGS)
	$(STRIP) tcpclient 

install:
	mkdir -p $(BIN_PATH)
	cp tcpclient $(BIN_PATH)/

clean:
	rm -f *.o tcpclient
	rm -rf $(BIN_PATH)

distclean:
	rm -f *.o tcpclient
	rm -rf $(BIN_PATH)

