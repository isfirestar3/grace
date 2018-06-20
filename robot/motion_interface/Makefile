TARGET=libmotion_interface.a

build=release
arch=x86_64

RM=rm
CP=cp

SRCS=./docks_mgr.cpp \
	./motion_interface.cpp

OBJS=$(patsubst %.cpp,%.o,$(SRCS))

CPPFLAGS+=-I ../../private/proto/ -I ../../sdk/rapidxml/ -I ../../sdk/nsp/icom/ -I ../../sdk/nsp/ -I ../../sdk/var/ -I ../../sdk/misc/ -Wall -std=c++11 

ifeq ($(build),debug)
	CPPFLAGS+=-g
else
	CPPFLAGS+=-O2
endif

ifeq ($(arch),arm)
	CC=arm-linux-gnueabihf-g++
	LDFLAGS:= 
else
	CC=g++
	LDFLAGS:= 
endif

##LDFLAGS+=-lrt -lm -lpthread -ldl -lxml2

all:$(TARGET)

$(TARGET):$(OBJS)
	ar crv  $@ $^ $(LDFLAGS)

%.o:%.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@

clean:
	$(RM) -f $(OBJS) $(TARGET)

.PHONY:clean all install
