cmake_minimum_required(VERSION 3.5)
project(GlobalTalk)

# set(CMAKE_C_COMPILER   "clang")
# set(CMAKE_CXX_COMPILER "clang++")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR "GNU")
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -O0 -ggdb")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "/Wall /Zc:preprocessor")
# elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
endif()

add_subdirectory(lib/glfw)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

add_library(
    ImGUI 
    STATIC 
    
    lib/imgui/imgui.cpp
    lib/imgui/imgui_demo.cpp
    lib/imgui/imgui_widgets.cpp
    lib/imgui/imgui_draw.cpp
    lib/imgui/imgui_tables.cpp
    lib/imgui/backends/imgui_impl_opengl3.cpp
    lib/imgui/backends/imgui_impl_glfw.cpp
)

target_include_directories(ImGUI PUBLIC lib/imgui)
target_link_libraries(ImGUI glfw)

if (UNIX AND NOT APPLE)
    # set(os_dir linux)
    set(
        os_src
        # src/os/linux/input_xorg.cpp
        # src/os/linux/audio_alsa.cpp


        # src/os/linux/input.cpp
        src/os/linux/input_xorg.cpp
        # src/os/linux/input_wayland.cpp
        src/os/linux/audio.cpp
        # src/os/linux/audio_alsa.cpp
        src/os/linux/audio_pulseaudio.cpp
        # src/os/linux/audio_pipewire.cpp
        
        # src/os/audio/linux/audio.cpp
        # src/os/audio/linux/internal/alsa.cpp
        # src/os/audio/linux/internal/pulseaudio.cpp
        # src/os/input/linux/input.cpp
    )
elseif (WIN32)
    # set(os_dir windows)
    set(
        os_src
        src/os/windows/input.cpp
        src/os/windows/audio.cpp
    )
else ()
    message(SEND_ERROR "OS not supported yet!")
endif ()

# set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
# set(BUILD_SHARED_LIBS OFF)
# set(CMAKE_EXE_LINKER_FLAGS "-static")

add_executable(
    GlobalTalk 
    src/main.cpp
    # src/os/${os_dir}/input.cpp
    # src/os/${os_dir}/audio.cpp
    ${os_src}
    src/gui/config_menu.cpp
)

target_link_libraries(GlobalTalk OpenGL::GL)
target_link_libraries(GlobalTalk glfw)
target_link_libraries(GlobalTalk ImGUI)

if (UNIX AND NOT APPLE)
    target_link_libraries(GlobalTalk X11 Xi asound dl)
elseif (WIN32)
    target_link_libraries(GlobalTalk ole32)
else ()
    message(SEND_ERROR "OS not supported yet!")
endif ()

target_include_directories(GlobalTalk PRIVATE lib)
target_include_directories(GlobalTalk PRIVATE src)
