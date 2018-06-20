TARGET = libgzcamera.so

build=release
arch=x86_64

RM=rm
CP=cp

CROSS_COMPILE = arm-linux-gnueabihf-

CFLAGS   =-fPIC -std=gnu99  
LDFLAGS := -shared
INCLUDES  = -I ../../sdk/nsp/icom/ -I ./ -I ../../sdk/misc/

#.c .o
SRCS = $(wildcard *.c)
# SRCS += $(wildcard ./comm/*.c)
			
OBJS  = $(patsubst %.c, %.o, $(SRCS))


LIBS = -L /srv/libjpeg-turbo/libjpeg/lib

ifeq ($(build),debug)
	CFLAGS+=-g
else
	CFLAGS+=-O2
endif

ifeq ($(arch),arm)
	CC=$(CROSS_COMPILE)gcc
	CFLAGS+= -mfloat-abi=hard -mfpu=neon 
else
	CC=gcc
endif


LIB = $(TARGET)

$(LIB): $(OBJS)
	@echo \# generating : $@
	$(CC) -o $@  $(OBJS) $(LDFLAGS) $(LIBS)
	@echo "--------------------------------------------------------------"
	@echo "Build Completed for $(LIB)"
	@echo "Building time: $(shell date)"
	@echo "--------------------------------------------------------------"

$(OBJS): %.o: %.c 
	@echo \# Compiling : $<
	$(CC) -c $< $(INCLUDES) $(CFLAGS) $(LIBS) $(LDFLAGS) -o $@


all: $(LIB)
	
clean:
	rm -f $(OBJS)

.PHONY:clean all
