TARGET=libgzfts.so

build=release
arch=x86_64

RM=rm
CP=cp

CPPFLAGS=-Wall -std=c++11 -fPIC 
INCLUDES=-I./proto \
        -I../sdk/misc/ \
        -I../sdk/var \
        -I../sdk/nsp \
		-I../sdk/nsp/icom/ \
        -I./
        
SRCS+=$(wildcard ./*.cpp) $(wildcard ../sdk/nsp/*.cpp) $(wildcard ./proto/*.cpp) 
OBJS:=$(patsubst %.cpp,%.o,$(SRCS))
LDFLAGS:=-shared

ifeq ($(build),debug)
	CPPFLAGS+=-g
else
	CPPFLAGS+=-O2
endif

ifeq ($(arch),arm)
	CC=arm-linux-gnueabihf-g++
else
	CC=g++
endif

$(TARGET): $(OBJS)
	$(CC) $^ -o $@  $(LDFLAGS)

all:		
	$(TARGET)

%.o:%.cpp
	$(CC) -c $< $(CPPFLAGS) $(INCLUDES) -o $@

install:
	cp -f $(TARGET) $(target_path)
	
clean:
	rm -f $(OBJS)
	
.PHONY:clean all install
