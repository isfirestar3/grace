TARGET=libagvinfo.so

build=Release
arch=x86_64

CPPFLAGS+=-fPIC -Wall -std=c++11 -fpermissive
#CPPFLAGS+=-Wall -std=c++11 -fpermissive
INCLUDES+=-I ../../../sdk/nsp/ \
		-I ../../../sdk/nsp/icom/ \
		-I ../../../sdk/misc/ \
		-I ../../../sdk/rapidxml/ \
		-I ../../../private/proto/ \
		-I ../../../private/ \
		-I ../include/ \
		-I ../../common/ \
		-I ./ \
		-I ../
        
SRCS+=$(wildcard ../source/*.cpp) $(wildcard ../*.cpp) $(wildcard ../../../sdk/nsp/*.cpp)
OBJS:=$(patsubst %.cpp,%.o,$(SRCS))

LDFLAGS:=-lm -lpthread -ldl -lrt 

ifeq ($(build),debug)
	CPPFLAGS+=-g
else
	CPPFLAGS+=-O2
endif

ifeq ($(arch),arm)
	CC=arm-linux-gnueabihf-g++ 
	CPPFLAGS+=-mfloat-abi=hard -mfpu=neon
	LDFLAGS:=/usr/local/lib/nshost.so -Wl,-rpath=/usr/local/lib -L /usr/local/lib/
else
	CC=g++
	LDFLAGS:=/usr/local/lib64/nshost.so -Wl,-rpath=/usr/local/lib64/:. -L /usr/local/lib64/
endif

#LDFLAGS+=-shared -fpic -lm -lpthread -ldl -lrt 
LDFLAGS+=-shared -lm -lpthread -ldl -lrt 


$(TARGET): $(OBJS)
	$(CC) $^ $(LDFLAGS) -o $@ 
all:		
	$(TARGET)
.cpp.o:
	$(CC) -c $< $(CPPFLAGS) $(INCLUDES) -o $@

.PHONY:clean all
clean:
	$(RM) -f $(OBJS) $(TARGET) $(target_path)$(TARGET)
