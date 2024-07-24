#include "gui/config_menu.h"
#include "imgui.h"

#include "core/audio.h"
#include "core/linux/audio/pulseaudio.h"

// This menu is supposed to be used for the configuration purposes as an alternative to starting the program.
// The layout part could be reused in the runtime menu, so the user could use the 
// configuration menu while the backgroud program logic is running.

static void glfw_error_callback(int error, const char *description) {
    log_error("GLFW Error %d: %s\n", error, description);
}

struct MenuData {
    Audio *audio;
    bool demo;

    int selected_playback;
    int selected_capture;
    int selected_output;
    int selected_input;

    bool on_release;
    bool on_press;

    bool mute;
    bool unmute;
    bool toggle;

    bool entry_selected;
    pulseaudio::Entry_Data entry_data;

    std::vector<pulseaudio::Entry_Data> playbacks;
    std::vector<pulseaudio::Entry_Data> captures;
    std::vector<pulseaudio::Entry_Data> output_devices;
    std::vector<pulseaudio::Entry_Data> input_devices;
};

static void render_menu_layout(MenuData *data) {
    if (data->demo) {
        ImGui::ShowDemoWindow(&data->demo);
    }

    ImGui::SetNextWindowBgAlpha(1.0f);
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    bool always_on = true;
    auto root_window_flags = 
        ImGuiWindowFlags_NoDecoration | 
        ImGuiWindowFlags_NoResize     | 
        ImGuiWindowFlags_MenuBar      | 
        ImGuiWindowFlags_NoBringToFrontOnFocus;
    ImGui::Begin("root_window", &always_on, root_window_flags);
    ImGui::PopStyleVar(1);


    if (ImGui::BeginMenuBar()) {
        if (ImGui::MenuItem("Save", "CTRL+s")) {}
        if (ImGui::MenuItem("Load", "CTRL+l")) {}
        if (ImGui::MenuItem("Exit", "CTRL+e")) {
            exit(0);
        }

        if (ImGui::MenuItem("Demo", "CTRL+d")) {
            data->demo ^= 1;
        }

        ImGui::EndMenuBar();
    }

    ImGui::BeginGroup(); {
        if (ImGui::BeginChild("keybind_pane", ImVec2(0, 210), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeY)) {
            ImGui::Text("Key mapping");
            ImGui::Separator();

            ImGui::SeparatorText("Select key action");
            if (ImGui::Checkbox("On press",  &data->on_press)) {
                data->on_press   = true;
                data->on_release = false;
            }

            ImGui::SameLine();

            if (ImGui::Checkbox("On release", &data->on_release)) {
                data->on_press   = false;
                data->on_release = true;
            }

            ImGui::SeparatorText("Select key bind");

            ImGui::BeginGroup(); {
                if (ImGui::BeginChild("controls_pane", ImVec2(150, 0), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeX)) {
                    ImGui::BeginDisabled();
                    ImGui::Text("Select modifiers:");
                    bool temp = false;
                    if (ImGui::Checkbox("Control", &temp)) { }
                    if (ImGui::Checkbox("Alt",     &temp)) { }
                    if (ImGui::Checkbox("Shift",   &temp)) { }
                    ImGui::EndDisabled();

                    ImGui::EndChild();
                }

                ImGui::SameLine();

                if (ImGui::BeginChild("button_select_pane", ImVec2(0, 0), ImGuiChildFlags_Border)) {
                    ImGui::Text("Select button binding:");
                    const char* items[] = { "INSERT", "MOUSE 4", "MOUSE 5", "OTHER" };
                    static int item_current = 0;
                    ImGui::Combo("##button_bind_combo", &item_current, items, IM_ARRAYSIZE(items));

                    ImGui::EndChild();
                }
                ImGui::EndGroup();
            } 
            ImGui::EndChild();
        }

        if (ImGui::BeginChild("action_pane", ImVec2(0, 0), ImGuiChildFlags_Border)) {
            ImGui::Text("Audio options");
            ImGui::Separator();

            ImGui::SeparatorText("Audio action");

            if (ImGui::Checkbox("Mute", &data->mute)) {
                data->mute = true;
                data->unmute = data->toggle = false;
            }
            ImGui::SameLine();
            if (ImGui::Checkbox("Unmute", &data->unmute)) {
                data->unmute = true;
                data->mute = data->toggle = false;
            }
            ImGui::SameLine();
            if (ImGui::Checkbox("Toggle", &data->toggle)) {
                data->toggle = true;
                data->mute = data->unmute = false;
            }

            ImGui::SeparatorText("Audio feedback"); {
                ImGui::BeginDisabled();
                bool temp = false;
                ImGui::Checkbox("Use audio feedback", &temp);
                ImGui::SameLine();
                if (ImGui::Button("Pick file:")) { }
                char buffer[] = "C:/Users/Test/my_audio_file.wav";
                ImGui::SameLine();
                ImGui::InputText("##file_picker", buffer, IM_ARRAYSIZE(buffer));
                ImGui::EndDisabled();
            }
            // File selection picker + other stuff goes here.

            ImGui::SeparatorText("Audio sources");
            ImGui::BeginGroup(); {
                if (ImGui::BeginChild("alsa_pane", ImVec2(200, 0), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeX)) {
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

                        data->selected_playback = -1;
                        data->selected_capture  = -1;
                        data->selected_output   = -1;
                        data->selected_input    = -1;

                        data->entry_selected = false;

                        data->playbacks      = pulseaudio::get_playback_streams((pulseaudio::Audio *)data->audio);
                        data->captures       = pulseaudio::get_capture_streams((pulseaudio::Audio *)data->audio);
                        data->output_devices = pulseaudio::get_output_devices((pulseaudio::Audio *)data->audio);
                        data->input_devices  = pulseaudio::get_input_devices((pulseaudio::Audio *)data->audio);
                    }
                    // ImGui::Text("PulseAudio");
                    ImGui::Separator();

                    if (ImGui::CollapsingHeader("Playback programs")) {
                        for (int i = 0; i < data->playbacks.size(); i++) {
                            auto &entry = data->playbacks[i];
                            bool temp = i == data->selected_playback;

                            char id_buffer[64];
                            sprintf(id_buffer, "playback_%d", i);
                            ImGui::PushID(id_buffer);

                            if (ImGui::Checkbox(entry.display_name, &temp)) {
                                data->entry_data = entry;
                                data->entry_selected = true;

                                data->selected_playback =  i;
                                data->selected_capture  = -1;
                                data->selected_output   = -1;
                                data->selected_input    = -1;
                            }

                            ImGui::PopID();
                        }
                    }

                    if (ImGui::CollapsingHeader("Capture programs")) {
                        for (int i = 0; i < data->captures.size(); i++) {
                            auto &entry = data->captures[i];
                            bool temp = i == data->selected_capture;

                            char id_buffer[64];
                            sprintf(id_buffer, "capture_%d", i);
                            ImGui::PushID(id_buffer);

                            if (ImGui::Checkbox(entry.display_name, &temp)) {
                                data->entry_data = entry;
                                data->entry_selected = true;

                                data->selected_playback = -1;
                                data->selected_capture  =  i;
                                data->selected_output   = -1;
                                data->selected_input    = -1;
                            }

                            ImGui::PopID();
                        }
                    }

                    if (ImGui::CollapsingHeader("Output devices")) {
                        for (int i = 0; i < data->output_devices.size(); i++) {
                            auto &entry = data->output_devices[i];
                            bool temp = i == data->selected_output;

                            char id_buffer[64];
                            sprintf(id_buffer, "output_device_%d", i);
                            ImGui::PushID(id_buffer);

                            if (ImGui::Checkbox(entry.display_name, &temp)) {
                                data->entry_data = entry;
                                data->entry_selected = true;

                                data->selected_playback = -1;
                                data->selected_capture  = -1;
                                data->selected_output   =  i;
                                data->selected_input    = -1;
                            }

                            ImGui::PopID();
                        }
                    }

                    if (ImGui::CollapsingHeader("Input devices")) {
                        for (int i = 0; i < data->input_devices.size(); i++) {
                            auto &entry = data->input_devices[i];
                            bool temp = i == data->selected_input;

                            char id_buffer[64];
                            sprintf(id_buffer, "input_device_%d", i);
                            ImGui::PushID(id_buffer);

                            if (ImGui::Checkbox(entry.display_name, &temp)) {
                                data->entry_data = entry;
                                data->entry_selected = true;

                                data->selected_playback = -1;
                                data->selected_capture  = -1;
                                data->selected_output   = -1;
                                data->selected_input    =  i;
                            }

                            ImGui::PopID();
                        }
                    }
                    ImGui::EndChild();
                } 
                ImGui::EndGroup();
            } 
            ImGui::EndChild();
        } 
        ImGui::EndGroup();
    } 

    ImGui::End();
}

i32 run_config_menu() {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        return 1;
    }

    const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

    // Create window with graphics context
    GLFWwindow *window = glfwCreateWindow(1280, 720, "GlobalTalk Configuration Menu", nullptr, nullptr);
    if (window == nullptr) {
        return 1;
    }

    glfwMakeContextCurrent(window);

    // Enable vsync
    glfwSwapInterval(1);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    mark_unused(io);
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImVec4 clear_color = ImVec4(1.0, 1.0, 1.0, 1.0);

    Audio *audio = initialize_audio();
    defer { destroy_audio(audio); };
    MenuData data = {
        .audio = audio,
        .demo = false,

        .selected_playback = -1,
        .selected_capture  = -1,
        .selected_output   = -1,
        .selected_input    = -1,

        .on_release = true,
        .on_press   = false,

        .mute   = true,
        .unmute = false,
        .toggle = false,

        .entry_selected = false,
        .entry_data = {},

        .playbacks      = pulseaudio::get_playback_streams((pulseaudio::Audio *)audio),
        .captures       = pulseaudio::get_capture_streams((pulseaudio::Audio *)audio),
        .output_devices = pulseaudio::get_output_devices((pulseaudio::Audio *)audio),
        .input_devices  = pulseaudio::get_input_devices((pulseaudio::Audio *)audio),
    };

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Begin frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        render_menu_layout(&data);

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(
            clear_color.x * clear_color.w, clear_color.y * clear_color.w,
            clear_color.z * clear_color.w, clear_color.w
        );
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
