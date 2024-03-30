#!/bin/sh

set -xe

base_dir=$(dirname "$0")
project_root=$(realpath $base_dir)

#
# Compiler
#
compiler="g++"
# compiler="clang++"
# compiler="x86_64-w64-mingw32-g++"

#
# Libraries
#
# project_libs="-L $project_root/build/glfw/lib64 -l:libglfw3.a  -L $project_root/build/imgui -l:imgui.a"
system_libs="-lGL -lX11 -lXi -lasound -lglfw"
# system_libs="-lole32"

libs="$system_libs $project_libs"


#
# Flags
#
include="-I $project_root/include -I $project_root/src"

# Release flags
# flags="-O3 -march=native"

# Debug flags
flags="$include -Wall -Wextra -Winit-self -g"


function build_glfw {
    mkdir -p $project_root/build/glfw/
    pushd $project_root/build/glfw

    cmake -DCMAKE_INSTALL_PREFIX=. -DCMAKE_BUILD_TYPE=Release $project_root/lib/glfw/
    make -j 12
    make install

    popd
}

function build_imgui {
    imgui_dir="$project_root/lib/imgui"

    mkdir -p $project_root/build/
    pushd $project_root/build/

    include="-I $project_root/include -I $imgui_dir" 
    compiler="g++"
    sources="$(ls $imgui_dir/*.cpp) $imgui_dir/backends/imgui_impl_opengl3.cpp $imgui_dir/backends/imgui_impl_glfw.cpp"
    for source in $sources; do
        $compiler -c $include $source 
    done

    # ar -rcs imgui.a *

    popd
}

function copy_headers {
    mkdir -p $project_root/include/ImGUI/
    cp $project_root/lib/imgui/imgui.h $project_root/include/ImGUI 
    cp $project_root/lib/imgui/imconfig.h $project_root/include/ImGUI 
    cp $project_root/lib/imgui/backends/imgui_impl_glfw.h    $project_root/include/ImGUI
    cp $project_root/lib/imgui/backends/imgui_impl_opengl3.h $project_root/include/ImGUI

    # mkdir -p $project_root/include/GLFW
    # cp -r include/GLFW/* $project_root/include/GLFW
}

function build_libs {
    copy_headers
    # build_glfw
    build_imgui
}

# function build_windows {
#     $compiler -c $flags src/main.cpp             -o build/main.o
#     $compiler -c $flags src/os/audio_windows.cpp -o build/audio.o
#     $compiler -c $flags src/os/input_windows.cpp -o build/input.o
#     $compiler -c $flags src/gui/config_menu.cpp  -o build/config_menu.o
# }

function build_linux {
    $compiler -c $flags src/main.cpp            -o build/main.o
    $compiler -c $flags src/os/audio_linux.cpp  -o build/audio.o
    $compiler -c $flags src/os/input_linux.cpp  -o build/input.o
    $compiler -c $flags src/gui/config_menu.cpp -o build/config_menu.o
}

mkdir -p $project_root/build/

# build_libs
# build_windows
build_linux

$compiler $flags $(ls $project_root/build/*.o) $libs -o build/global-talk

if [ "$1" == "" ]; then
    exit 0
fi

if [ "$1" == "--" ]; then
    shift 1
fi

./build/global-talk $*
