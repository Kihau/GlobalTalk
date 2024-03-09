#!/bin/sh

set -xe

c++ -c -g linux-talk.cpp -o build/linux-talk.o
cc  -c -g query_state.c  -o build/query_state.o

libs="-lX11 -lXi -lasound"
c++ $libs build/query_state.o build/linux-talk.o -o global-talk

./global-talk
