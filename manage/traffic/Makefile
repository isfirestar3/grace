TARGET=libtraffic_manage.a

build=debug
arch=x86_64

SRCS=$(wildcard ./*.cpp) $(wildcard ../../sdk/nsp/*.cpp)
OBJS=$(patsubst %.cpp,%.o,$(SRCS))

CPPFLAGS+=-I ../../sdk/nsp/icom/ -I ../../sdk/nsp/ -I ../../sdk/var/ -I ../../sdk/misc/ -Wall -std=c++11 -O2 

ifeq ($(build),debug)
	CPPFLAGS+=-g
else
	CPPFLAGS+=-O2
endif

ifeq ($(arch),arm)
	CC=arm-linux-gnueabihf-gcc
	LDFLAGS:= 
else
	CC=g++
	LDFLAGS:= 
endif

target_path=../../RTM/
ifeq ($(build),debug)
	target_path=../../sdk/lib/
else
	target_path=../../RTM/
endif

##LDFLAGS+=-lrt -lm -lpthread -ldl -lxml2

all:$(TARGET)

$(TARGET):$(OBJS)
	ar crv  $@ $^ $(LDFLAGS)

%.o:%.c
	$(CC) $(CPPFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS) $(TARGET) $(target_path)$(TARGET)

install:
#	cp -f $(TARGET) $(target_path)

.PHONY:clean all install
