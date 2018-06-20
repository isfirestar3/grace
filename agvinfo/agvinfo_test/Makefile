TARGET=agvinfo_test

build=release
arch=x86_64

CP=cp

CPPFLAGS=-Wall -std=c++11 
INCLUDES=-I../../sdk/nsp/ \
        -I../../sdk/misc/ \
	-I ../../sdk/nsp/icom/
        
SRCS+=$(wildcard *.cpp) 
OBJS:=$(patsubst %.cpp,%.o,$(SRCS))

LDFLAGS:=-lm -lpthread -ldl -lrt -Wl,-rpath=. -L ./ -lagvinfo

ifeq ($(build),debug)
	CPPFLAGS+=-g
	PLATFORM_DIR=sdk/bin/
else
	CPPFLAGS+=-O2
	PLATFORM_DIR=RTM/
endif

ifeq ($(arch),arm)
	CC=arm-linux-gnueabihf-g++ 
	ARCH_DIR:=arm/
	LDFLAGS+=/usr/local/lib/nshost.so -Wl,-rpath=/usr/local/lib -L /usr/local/lib/
else
	CC=g++
	LDFLAGS+=/usr/local/lib64/nshost.so -Wl,-rpath=/usr/local/lib64/  -L /usr/local/lib64/
endif

$(TARGET): $(OBJS)
	$(CC) $^ -o $@  $(LDFLAGS)

all:		
	$(TARGET)

.cpp.o:
	$(CC) -c $< $(CPPFLAGS) $(INCLUDES) -o $@

.PHONY:clean all install
clean:
	$(RM) $(OBJS)  $(TARGET)
