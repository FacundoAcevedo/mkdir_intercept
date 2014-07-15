CC=gcc
CFLAGS=-Wall -std=c99 -O2 -fPIC -lconfig -ldl -shared -ftracer  -g -pedantic
PRINCIPAL=mkdir_wrapper
DEPS_CORE=$(SRC_CORE)/validaciones.c $(SRC_CORE)/$(PRINCIPAL).c
SRC_CORE=src/core

.PHONY: all clean tests

all: wrapper

wrapper: validaciones
	$(CC) $(CFLAGS) validaciones.o  $(SRC_CORE)/$(PRINCIPAL).c  src/logger/logger.c -o mkdir_wrapper.so 

clean:
		rm -f mkdir_wrapper.so 

validaciones:
	$(CC) $(CFLAGS) -c $(SRC_CORE)/validaciones.c 

tests: 
	$(CC)  -Wall -std=c99 -lconfig -g  -c $(SRC_CORE)/validaciones.c 
	$(CC)   -lcunit -Wall -std=c99 -lconfig -g  validaciones.o tests/test.c src/logger/logger.c  -o tests.out 
	$(CC)    -Wall -std=c99 -lconfig -g  validaciones.o tests/test_noCUnit.c src/logger/logger.c  -o tests_noCUunit.out

