#!/bin/sh

set -xe

# Linux compile
libs="-lX11 -lXi -lasound"
c++ -c -g src/main.cpp           -o build/main.o
c++ -c -g src/os/audio_linux.cpp -o build/audio.o
c++ -c -g src/os/input_linux.cpp -o build/input.o

# Windows compile
# libs="-lole32"
# alias c++="x86_64-w64-mingw32-c++"
# c++ -c -g src/main.cpp           -o build/main.o
# c++ -c -g src/os/audio_windows.cpp -o build/audio.o
# c++ -c -g src/os/input_windows.cpp -o build/input.o

c++ $libs -I src/ build/main.o build/audio.o build/input.o -o build/global-talk
./build/global-talk
