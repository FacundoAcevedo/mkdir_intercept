CC=gcc
CFLAGS=-Wall -std=c99 -O2 -fPIC -lconfig -ldl -shared -ftracer 

.PHONY: all clean

all: wrapper

wrapper: 
	$(CC) $(CFLAGS)  mkdir_wrapper.c -o mkdir_wrapper.so

clean:
		rm -f mkdir_wrapper.so
