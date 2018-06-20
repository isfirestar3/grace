TARGET=conv_xml2bin

build=release
arch=x86_64

SRCS=$(wildcard ./convert_xml.c) $(wildcard ./xml_loaded.c) $(wildcard ./map_test.c) $(wildcard ./map.c) 
OBJS=$(patsubst %.c,%.o,$(SRCS))

CFLAGS+=-I ./ -I ../../../../sdk/nsp/icom/ -I ../../../../sdk/var/ -I ../../../../private/libxml2-linux/ -Wall -std=gnu99

ARCH_DIR=

ifeq ($(build),debug)
	CFLAGS+=-g
else
	CFLAGS+=-O2
endif

ifeq ($(arch),arm)
	CC=arm-linux-gnueabihf-gcc
	LDFLAGS:=/usr/local/lib/nshost.so -Wl,-rpath=/usr/local/lib -L /usr/local/lib/
	ARCH_DIR:=arm/
	CFLAGS+=-mfloat-abi=hard -mfpu=neon
else
	CC=gcc
	LDFLAGS:=/usr/local/lib64/nshost.so -Wl,-rpath=/usr/local/lib64/ -L /usr/local/lib64/
endif


LDFLAGS+=-lrt -lm -lpthread -ldl -lxml2

all:$(TARGET)

$(TARGET):$(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o:%.c
	$(CC) -c $< $(CFLAGS) -o $@

clean:
	$(RM) $(OBJS) $(TARGET)

.PHONY:clean all install