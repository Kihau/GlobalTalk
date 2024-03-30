cmake_minimum_required(VERSION 3.5)
project(GlobalTalk)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")

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

if (UNIX AND NOT APPLE)
    set(os_dir linux)
elseif (WIN32)
    set(os_dir windows)
else ()
    message(SEND_ERROR "OS not supported yet!")
endif()

add_executable(
    GlobalTalk 
    src/main.cpp
    src/os/${os_dir}/input.cpp
    src/os/${os_dir}/audio.cpp
    src/gui/config_menu.cpp
)

target_link_libraries(GlobalTalk glfw)
target_link_libraries(GlobalTalk OpenGL::GL)
target_link_libraries(GlobalTalk ImGUI)

if (UNIX AND NOT APPLE)
    target_link_libraries(GlobalTalk X11 Xi asound)
elseif (WIN32)
    target_link_libraries(GlobalTalk ole32)
else ()
    message(SEND_ERROR "OS not supported yet!")
endif()

target_include_directories(GlobalTalk PRIVATE lib)
target_include_directories(GlobalTalk PRIVATE src)
