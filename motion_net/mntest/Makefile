TARGET=test

build=release
arch=x86_64
lang=gcc
CC=$(lang)

CFLAGS=-Wall -std=c++11

#INCLUDES=
INCLUDES+=-I ../../sdk/nsp/ -I ../../sdk/misc/ -I ../../sdk/var/
        
SRCS+=$(wildcard *.cpp) 
#SRCS+=$(wildcard libnsp/*.cpp)

OBJS:=$(patsubst %.cpp,%.o,$(SRCS))

LDFLAGS=-lm -lpthread -ldl -lrt ../motion_net.so
LDFLAGS+=-lrt /usr/local/lib64/nshost.so -Wl,-rpath=/usr/local/lib64/

ifeq ($(build),debug)
	CFLAGS+=-g
else
	CFLAGS+=-O2
endif

ifeq ($(lang),g++)
	CFLAGS+=-std=c++11
endif

ifeq ($(arch),arm)
	CC=arm-linux-gnueabihf-$(lang)
else
	ifeq ($(arch), i686)
		CFLAGS+=-m32
		LDFLAGS+=-m32
	endif
endif

$(TARGET): $(OBJS)
	$(CC) $^ -o $@  $(LDFLAGS)

all:		
	$(TARGET)

.cpp.o:
	$(CC) -c $< $(CFLAGS) $(INCLUDES) -o $@

clean:
	rm -f $(OBJS)
	rm -f *.o
	rm -f $(TARGET)

.PHONY:clean all install
