CC=gcc
CFLAGS=-Wall -std=c99 -O2 -fPIC -lconfig -ldl -shared -ftracer 


.PHONY: all clean

all: wrapper

wrapper: 
	$(CC) $(CFLAGS)  src/core/mkdir_wrapper.c src/logger/logger.c -o mkdir_wrapper.so 

clean:
		rm -f mkdir_wrapper.so 

