#!/bin/sh

set -xe

# Release flags
flags="-O3 -march=native"

# Debug flags
# flags="-Wall -Wextra -Winit-self -g"

function build_windows {
    compiler="x86_64-w64-mingw32-g++"

    libs="-lole32"
    $compiler -c $flags src/main.cpp             -o build/main.o
    $compiler -c $flags src/os/audio_windows.cpp -o build/audio.o
    $compiler -c $flags src/os/input_windows.cpp -o build/input.o
}

function build_linux {
    compiler="g++"
    # compiler="clang++"

    libs="-lX11 -lXi -lasound"
    $compiler -c $flags src/main.cpp           -o build/main.o
    $compiler -c $flags src/os/audio_linux.cpp -o build/audio.o
    $compiler -c $flags src/os/input_linux.cpp -o build/input.o
}

# build_windows
build_linux
$compiler $flags -I src/ build/main.o build/audio.o build/input.o $libs -o build/global-talk
./build/global-talk
