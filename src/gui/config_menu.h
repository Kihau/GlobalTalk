#ifndef CONFIG_MENU_H
#define CONFIG_MENU_H

#include <stdlib.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "utils.h"
#include "config.h"

struct Audio_Gui_Data;

struct Menu_Data {
    Config config;

    bool demo;

    bool on_release;
    bool on_press;

    int selected_button;

    bool mute;
    bool unmute;
    bool toggle;

    Audio_Gui_Data *audio_data;
};

i32 run_config_menu();
void render_audio_panel(Audio_Gui_Data *data);
Audio_Gui_Data* create_audio_gui_data();
Audio_Stream get_audio_stream_selection(Audio_Gui_Data *data);

#endif // CONFIG_MENU_H
