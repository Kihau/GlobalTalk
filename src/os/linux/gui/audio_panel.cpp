#include "gui/config_menu.h"
#include "os/linux/internal/audio/pulseaudio.h"

struct Audio_Gui_Data {
    pulseaudio::Audio *audio;

    int selected_playback;
    int selected_capture;
    int selected_output;
    int selected_input;

    bool entry_selected;
    pulseaudio::Entry_Data entry_data;

    std::vector<pulseaudio::Entry_Data> playbacks;
    std::vector<pulseaudio::Entry_Data> captures;
    std::vector<pulseaudio::Entry_Data> output_devices;
    std::vector<pulseaudio::Entry_Data> input_devices;
};

static void display_list_entries(Audio_Gui_Data *data, const std::vector<pulseaudio::Entry_Data> &entries, int *selected_entry) {
    for (int i = 0; i < entries.size(); i++) {
        auto &entry = entries[i];
        bool temp = i == *selected_entry;

        char id_buffer[64];
        sprintf(id_buffer, "playback_%d", i);
        ImGui::PushID(id_buffer);

        if (ImGui::Checkbox(entry.display_name, &temp)) {
            data->selected_playback = -1;
            data->selected_capture  = -1;
            data->selected_output   = -1;
            data->selected_input    = -1;

            data->entry_data     = entry;
            data->entry_selected = true;

            *selected_entry = i;
        }

        ImGui::PopID();
    }
}

static void refresh_list_entries(Audio_Gui_Data *data, pulseaudio::Audio *audio) {
    data->selected_playback = -1;
    data->selected_capture  = -1;
    data->selected_output   = -1;
    data->selected_input    = -1;

    data->entry_data     = {};
    data->entry_selected = false;

    data->playbacks      = pulseaudio::get_playback_streams(audio);
    data->captures       = pulseaudio::get_capture_streams(audio);
    data->output_devices = pulseaudio::get_output_devices(audio);
    data->input_devices  = pulseaudio::get_input_devices(audio);
}

void render_audio_panel(Audio_Gui_Data *data) {
    ImGui::BeginGroup(); {
        if (ImGui::BeginChild("alsa_pane", ImVec2(250, 0), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeX)) {
            ImGui::BeginDisabled();
            bool alsa_enabled = false;
            ImGui::Checkbox("Alsa",  &alsa_enabled);
            ImGui::SameLine();
            if (ImGui::Button("Refresh##alsa_refresh")) { }
            ImGui::Separator();
            ImGui::CollapsingHeader("Simple Mixer Playback");
            ImGui::CollapsingHeader("Simple Mixer Capture");
            ImGui::EndDisabled();
        } 
        ImGui::EndChild();

        ImGui::SameLine();

        if (ImGui::BeginChild("pulseaudio_pane", ImVec2(0, 0), ImGuiChildFlags_Border)) {
            bool pulseaudio_enabled = true;
            ImGui::Checkbox("PulseAudio",  &pulseaudio_enabled);
            ImGui::SameLine();
            if (ImGui::Button("Refresh##pulseaudio_refresh")) { 
                refresh_list_entries(data, data->audio);
            }

            ImGui::Separator();

            if (ImGui::CollapsingHeader("Playback programs")) {
                display_list_entries(data, data->playbacks, &data->selected_playback);
            }

            if (ImGui::CollapsingHeader("Capture programs")) {
                display_list_entries(data, data->captures, &data->selected_capture);
            }

            if (ImGui::CollapsingHeader("Output devices")) {
                display_list_entries(data, data->output_devices, &data->selected_output);
            }

            if (ImGui::CollapsingHeader("Input devices")) {
                display_list_entries(data, data->input_devices, &data->selected_input);
            }
        } 
        ImGui::EndChild();
    } 
    ImGui::EndGroup();
}

Audio_Gui_Data* create_audio_gui_data() {
    pulseaudio::Audio *audio = pulseaudio::initialize_audio();
    assert(audio);

    Audio_Gui_Data *data = new Audio_Gui_Data;
    data->audio = audio;

    data->selected_playback = -1;
    data->selected_capture  = -1;
    data->selected_output   = -1;
    data->selected_input    = -1;

    data->entry_selected = false;
    data->entry_data = {};

    data->playbacks      = pulseaudio::get_playback_streams(audio);
    data->captures       = pulseaudio::get_capture_streams(audio);
    data->output_devices = pulseaudio::get_output_devices(audio);
    data->input_devices  = pulseaudio::get_input_devices(audio);

    return data;
}

Audio_Stream get_audio_stream_selection(Audio_Gui_Data *data) {
    Audio_Stream stream = {};

    if (!data->entry_selected) {
        stream.has_audio_stream = false;
        return stream;
    }

    stream.has_audio_stream = true;
    // NOTE: realloc
    stream.name = data->entry_data.display_name;

    if (data->selected_playback != -1) {
        stream.type = Stream_Type::PLAYBACK_STREAM;
    } else if (data->selected_capture != -1) {
        stream.type = Stream_Type::CAPTURE_STREAM;
    } else if (data->selected_input != -1) {
        stream.type = Stream_Type::INPUT_DEVICE;
    } else if (data->selected_output != -1) {
        stream.type = Stream_Type::OUTPUT_DEVICE;
    } else {
        assert(false && "Unreachable");
    }

    return stream;
}

