TARGET=agv_shell

build=release
arch=x86_64

CP=cp

CPPFLAGS=-Wall -std=c++11 
INCLUDES=-I../../sdk/nsp/ \
		-I../../sdk/nsp/icom/ \
        -I../../sdk/misc/ \
        -I../../sdk/rapidxml/ \
		-I../../private/proto/ \
        -I./
        
SRCS+=$(wildcard *.cpp) 
OBJS:=$(patsubst %.cpp,%.o,$(SRCS))

LDFLAGS:=-lm -lpthread -ldl -lrt libgzfts.so -Wl,-rpath=.

ifeq ($(build),debug)
	CPPFLAGS+=-g
else
	CPPFLAGS+=-O2
endif

ifeq ($(arch),arm)
	CC=arm-linux-gnueabihf-g++ 
	LDFLAGS+=/usr/local/lib/nshost.so -Wl,-rpath=/usr/local/lib -L /usr/local/lib/
else
	CC=g++
	LDFLAGS+=/usr/local/lib64/nshost.so -Wl,-rpath=/usr/local/lib64/  -L /usr/local/lib64/
endif

GETFTSLIB=`ln -sf ../../gzfts/libgzfts.so ./libgzfts.so`

$(TARGET): $(OBJS)
	@echo get libgzfts.so $(GETFTSLIB)
	$(CC) $^ -o $@  $(LDFLAGS)

all:		
	$(TARGET)

.cpp.o:
	$(CC) -c $< $(CPPFLAGS) $(INCLUDES) -o $@

.PHONY:clean all install
clean:
	$(RM) $(OBJS) $(TARGET)
