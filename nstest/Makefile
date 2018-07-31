TARGET=nstest

build=release
arch=x86_64
CC=g++

CFLAGS=-Wall -std=c++11

INCLUDES+=-I ../sdk/misc/ -I ../sdk/var/ -I ../sdk/nsp/
#INCLUDES+=-I libnsp/
        
SRCS+=$(wildcard *.cpp) 
SRCS+=$(wildcard ../sdk/nsp/*.cpp)

OBJS:=$(patsubst %.cpp,%.o,$(SRCS))

LDFLAGS+=-lm -lpthread -ldl -lrt ./motion_net.so -Wl,-rpath=./

ifeq ($(build),debug)
	CFLAGS+=-g
else
	CFLAGS+=-O2
endif

ifeq ($(lang),g++)
	CFLAGS+=-std=c++11
endif

ifeq ($(arch),arm)
	CC=arm-linux-gnueabihf-g++
	LDFLAGS+=/usr/local/lib/nshost.so -Wl,-rpath=/usr/local/lib/
else
	LDFLAGS+=/usr/local/lib64/nshost.so -Wl,-rpath=/usr/local/lib64/
endif

LINKMNLIB=`ln -sf ../motion_net/motion_net.so ./motion_net.so`
LDFLAGS+=./motion_net.so -Wl,-rpath=./ -lm -lpthread -ldl -lrt

$(TARGET): $(OBJS)
	@echo get motion_net.so $(LINKMNLIB)
	$(CC) -o  $@ $^ $(LDFLAGS)

all:		
	$(TARGET)

.cpp.o:
	$(CC) -c $< $(CFLAGS) $(INCLUDES) -o $@

clean:
	rm -f $(OBJS)
	rm -f $(TARGET)

.PHONY:clean all install
