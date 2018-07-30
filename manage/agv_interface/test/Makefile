TARGET=test_interface

build=release
arch=x86_64

CP=cp

CPPFLAGS=-Wall -std=c++11 
INCLUDES=-I ../../../private/proto/ \
		-I ../../../sdk/rapidxml/ \
        -I ../../../sdk/nsp/icom/ \
        -I ../../../sdk/nsp/ \
        -I ../../../sdk/var/ \
        -I ../../../sdk/misc/ \
        -I ../ \
		-I./
        
SRCS+=$(wildcard ./*.cpp) $(wildcard ../../../sdk/nsp/*.cpp)
OBJS:=$(patsubst %.cpp,%.o,$(SRCS))


ifeq ($(build),debug)
	CPPFLAGS+=-g
else
	CPPFLAGS+=-O2
endif

ifeq ($(arch),arm)
	CC=arm-linux-gnueabihf-g++ 
	LDFLAGS:=/usr/local/lib/nshost.so -Wl,-rpath=/usr/local/lib -L /usr/local/lib/
else
	CC=g++
	LDFLAGS:=/usr/local/lib64/nshost.so -Wl,-rpath=/usr/local/lib64/  -L /usr/local/lib64/
endif

LDFLAGS+=../libagv_interface.a -lm -lpthread -ldl -lrt 

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

all:		
	$(TARGET)

.cpp.o:
	$(CC) -c $< $(CPPFLAGS) $(INCLUDES) -o $@

.PHONY:clean all install
clean:
	$(RM) $(OBJS) $(TARGET)
