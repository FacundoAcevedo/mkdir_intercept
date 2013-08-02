#!/bin/bash

gcc -Wall -std=c99 -fPIC  -ldl -shared -o wrapper.so wrapper.c

sudo cp wrapper.so /tmp/wrapper.so -fv

