
#CROSS_COMPILE=arm-linux-
CROSS_COMPILE=
STRIP	= $(CROSS_COMPILE)strip
CC	= $(CROSS_COMPILE)gcc
LD      = $(CROSS_COMPILE)ld
TARGET=arm-linux


#cross compile
#CFLAGS +="-I/opt/eldk-4.0/arm/usr/include/"
#LDFLAGS += "-Wl,-rpath,/opt/eldk-4.0/arm/lib/"


#host compile
IFLAGS = -I../include 
CFLAGS +=-g -O2
LDFLAGS += -L./

BUILD_PATH = build
BIN_PATH = $(BUILD_PATH)/bin
LIB_PATH = $(BUILD_PATH)/lib

OBJ=  tcpclient.o communicate.o getAllChannelSignal.o getOutRate.o getMultiMethod.o getMultiProgNum.o getMultiProgNum2.o  getPidMapSumCnt.o curCHNSearched.o getOutPidMapNum.o getPrgCnt.o getPrgramInfo.o getPrograms.o


LIB_OBJ=  communicate.o getAllChannelSignal.o getOutRate.o getMultiMethod.o getMultiProgNum.o getMultiProgNum2.o  getPidMapSumCnt.o curCHNSearched.o getOutPidMapNum.o getPrgCnt.o getPrgramInfo.o getPrograms.o

.PHONY: all clean distclean

all:build install

build:
	$(CC) $(CFLAGS) $(IFLAGS) -c tcpclient.c
	$(CC) $(CFLAGS) $(IFLAGS) -c communicate.c
	$(CC) $(CFLAGS) $(IFLAGS) -c getAllChannelSignal.c
	$(CC) $(CFLAGS) $(IFLAGS) -c getOutRate.c
	$(CC) $(CFLAGS) $(IFLAGS) -c getMultiMethod.c
	$(CC) $(CFLAGS) $(IFLAGS) -c getMultiProgNum.c
	$(CC) $(CFLAGS) $(IFLAGS) -c getMultiProgNum2.c
	$(CC) $(CFLAGS) $(IFLAGS) -c getPidMapSumCnt.c
	$(CC) $(CFLAGS) $(IFLAGS) -c curCHNSearched.c
	
	$(CC) $(CFLAGS) $(IFLAGS) -c getOutPidMapNum.c
	$(CC) $(CFLAGS) $(IFLAGS) -c getPrgCnt.c
	$(CC) $(CFLAGS) $(IFLAGS) -c getPrgramInfo.c
	$(CC) $(CFLAGS) $(IFLAGS) -c getPrograms.c

	
	
	$(CC) $(OBJ) -o tcpclient $(LDFLAGS) $(DBS_LIB) $(BOARDAPI_LD_FLAGS)

	$(CC) $(LIB_OBJ) -shared -Wall -fPIC -o libtcpclient.so

	#$(STRIP) tcpclient 

install:
	mkdir -p $(BIN_PATH)
	cp tcpclient $(BIN_PATH)/
	mkdir -p $(LIB_PATH)
	mv libtcpclient.so $(LIB_PATH)/
	mkdir -p $(LIB_PATH)/include
	cp -a *.h $(LIB_PATH)/include
	sudo cp -a $(BUILD_PATH) /home/nfs/source
        sudo chown -R root:root /home/nfs/source/build



clean:
	rm -f *.o tcpclient
	rm -rf $(BUILD_PATH)
	
	sudo rm -rf /home/nfs/source/$(BUILD_PATH)

distclean:
	rm -f *.o tcpclient
	rm -rf $(BUILD_PATH)
	sudo rm -rf /home/nfs/source/$(BUILD_PATH)

