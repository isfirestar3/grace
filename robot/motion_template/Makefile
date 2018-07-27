TARGET=motion_template

build=release
arch=x86_64

RM=rm
CP=cp

SRCS=$(wildcard ./*.c) $(wildcard ./var/*.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

CFLAGS+=-I ./ -I ../../sdk/nsp/icom/ -I ../../sdk/var/ -I ../../private/libxml2-linux/ -I ../../private/ -Wall -std=gnu99

ifeq ($(build),debug)
	CFLAGS+=-g
else
	ifeq ($(build),gdb)
		CFLAGS+=-ggdb3
	else
		CFLAGS+=-O2
	endif
endif

ifeq ($(arch),arm)
	CC=arm-linux-gnueabihf-gcc
	LDFLAGS:=/usr/local/lib/nshost.so -Wl,-rpath=/usr/local/lib -L /usr/local/lib/
	CFLAGS+=-mfloat-abi=hard -mfpu=neon
else
	ifeq ($(arch), i686)
		CC=gcc
		CFLAGS+=-m32
		LDFLAGS+=-m32
		LDFLAGS:=/usr/local/lib/nshost.so -Wl,-rpath=/usr/local/lib/ -L /usr/local/lib/
	else
		CC=gcc
		LDFLAGS:=/usr/local/lib64/nshost.so -Wl,-rpath=/usr/local/lib64/ -L /usr/local/lib64/
	endif
endif


LDFLAGS+=-lrt -lm -lpthread -ldl -lxml2

all:$(TARGET)

$(TARGET):$(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o:%.c
	$(CC) -c $< $(CFLAGS) -o $@

clean:
	$(RM) -f $(OBJS) $(TARGET)

.PHONY:clean all install
