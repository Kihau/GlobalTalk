#include "gui/config_menu.h"

struct AudioGuiData {
    int dummy;
};

void render_audio_panel(AudioGuiData *data) {
    ImGui::Text("TODO");
    mark_unused(data);
}

AudioGuiData* create_audio_gui_data() {
    return NULL;
}

Audio_Stream get_audio_stream_selection(Audio_Gui_Data *data) {
    Audio_Stream stream = { .has_audio_stream = false };
    return stream;
}
