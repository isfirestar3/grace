TARGET=PathSearch.so

build=release
arch=x86_64

RM=rm
CP=cp

CPPFLAGS=-Wall -std=c++11 -fPIC

INCLUDES=-I ../sdk/var/  -I ../private/ -I ../sdk/nsp/icom -I ../sdk/nsp -I ../sdk/misc/ -I ../sdk/rapidxml/ -I ./ -I ../private/proto/
SRCS+=$(wildcard ./*.cpp) 
#SRCS+=$(wildcard ./*.cpp) $(wildcard ../sdk/nsp/*.cpp)
OBJS:=$(patsubst %.cpp,%.o,$(SRCS))
LDFLAGS:=-shared

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

.PHONY:clean all install
