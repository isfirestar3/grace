TARGET=pynet.so

build=release
arch=x86_64

RM=rm
CP=cp

CPPFLAGS=-Wall -std=c++11 -fPIC

INCLUDES=-I ../../sdk/var/  -I ../../private/ -I ../../sdk/nsp -I ../../sdk/misc/ -I ./ -I ../../private/proto/
SRCS+=$(wildcard ./*.cpp) $(wildcard ../sdk/nsp/*.cpp)
OBJS:=$(patsubst %.cpp,%.o,$(SRCS))
LDFLAGS:=-shared /usr/local/lib64/nshost.so -Wl,-rpath=/usr/local/lib64/ ./motion_net.so -Wl,-rpath=./

ifeq ($(build),debug)
	CPPFLAGS+=-g
else
	CPPFLAGS+=-O2
endif

ifeq ($(arch),arm)
	CC=arm-linux-gnueabihf-g++
	CPPFLAGS+=-mfloat-abi=hard -mfpu=neon
else
	CC=g++
endif

all:$(TARGET)

$(TARGET): $(OBJS)
	$(CC) $^ -o $@  $(LDFLAGS)

.cpp.o:
	$(CC) -c $< $(CPPFLAGS) $(INCLUDES) -o $@

clean:
	rm -f $(OBJS)
	rm -f $(TARGET)

.PHONY:clean all install
