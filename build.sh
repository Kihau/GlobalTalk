#!/bin/sh

set -xe

base_dir=$(dirname "$0")
project_root=$(realpath $base_dir)

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

    mkdir -p $project_root/include/GLFW
    cp -r include/GLFW/* $project_root/include/GLFW

    popd
}

function build_imgui {
    imgui_dir="$project_root/lib/imgui"

    mkdir -p $project_root/include/ImGUI/
    cp $imgui_dir/imgui.h $project_root/include/ImGUI 
    cp $imgui_dir/imconfig.h $project_root/include/ImGUI 
    cp $imgui_dir/backends/imgui_impl_glfw.h    $project_root/include/ImGUI
    cp $imgui_dir/backends/imgui_impl_opengl3.h $project_root/include/ImGUI

    mkdir -p $project_root/build/imgui/
    pushd $project_root/build/imgui

    include="-I $project_root/include -I $imgui_dir" 
    compiler="g++"
    sources="$(ls $imgui_dir/*.cpp) $imgui_dir/backends/imgui_impl_opengl3.cpp $imgui_dir/backends/imgui_impl_glfw.cpp"
    for source in $sources; do
        $compiler -c $include $source 
    done

    ar -rcs imgui.a *

    popd
}

function build_libs {
    build_glfw
    build_imgui
}

# function build_windows {
#     compiler="x86_64-w64-mingw32-g++"
#
#     libs="-lole32"
#     $compiler -c $flags src/main.cpp             -o build/main.o
#     $compiler -c $flags src/os/audio_windows.cpp -o build/audio.o
#     $compiler -c $flags src/os/input_windows.cpp -o build/input.o
#     $compiler -c $flags src/gui/config_menu.cpp  -o build/config_menu.o
# }

function build_linux {
    compiler="g++"
    libs="-lGL -lX11 -lXi -lasound -L $project_root/build/glfw/lib64 -l:libglfw3.a  -L $project_root/build/imgui -l:imgui.a"

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
./build/global-talk $*
