#include "gui/config_menu.h"
#include "imgui.h"

// This menu is supposed to be used for the configuration purposes as an alternative to starting the program.
// The layout part could be reused in the runtime menu, so the user could use the 
// configuration menu while the backgroud program logic is running.

static void glfw_error_callback(int error, const char *description) {
    log_error("GLFW Error %d: %s\n", error, description);
}

static void add_bind_to_config(Menu_Data *data) {
    Button button;
    if (data->on_press) {
        button.state = BUTTON_PRESS;
    } else {
        button.state = BUTTON_RELEASE;
    }

    button.type = (ButtonType)data->selected_button;

    Audio_Action action;
    if (data->mute) {
        action = Audio_Action::MUTE;
    } else if (data->unmute) {
        action = Audio_Action::UNMUTE;
    } else {
        action = Audio_Action::TOGGLE;
    }

    Audio_Stream stream = get_audio_stream_selection(data->audio_data);

    Audio_Bind bind = {
        .button = button,
        .action = action,
        .stream = stream,
    };

    data->config.binds.push_back(bind);
}

static void display_menu_bar(Menu_Data *data) {
    if (ImGui::BeginMenuBar()) {
        if (ImGui::MenuItem("Save", "CTRL+s")) {
            save_config(data->config);
        }

        if (ImGui::MenuItem("Load", "CTRL+l")) {
            Config config = load_config();
            data->config = config;
        }

        if (ImGui::MenuItem("Exit", "CTRL+e")) {
            exit(0);
        }

        if (ImGui::MenuItem("Debug", "CTRL+d")) {
            data->demo ^= 1;
        }

        ImGui::EndMenuBar();
    }
}

static void display_keybinds_list_panel(Menu_Data *data) {
    ImGui::Text("Added binds");
    ImGui::Separator();

    static i64 selected = -1;
    for (i64 i = 0; i < (i64)data->config.binds.size(); i++) {
        char id_buffer[64];
        sprintf(id_buffer, "removebtn_%li", i);
        ImGui::PushID(id_buffer);

        if (ImGui::Button("Remove")) {
            ImGui::PopID();
            log_info("Binding with index %li was removed.", i);
            data->config.binds.erase(data->config.binds.begin() + i);
            if (selected == i) {
                selected = -1;
            }

            save_config(data->config);
            break;
        }
        ImGui::PopID();

        ImGui::SameLine();

        char buffer[32];
        sprintf(buffer, "Bind %li", i);
        if (ImGui::Selectable(buffer, selected == i)) {
            selected = i;
        }
    }
}

static void display_configuration_panel(Menu_Data *data) {
    ImGui::BeginGroup(); {
        if (ImGui::BeginChild("keybind_pane", ImVec2(0, 210), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeY)) {
            if (ImGui::Button("Add")) {
                add_bind_to_config(data);
                save_config(data->config);
            }
            ImGui::SameLine();
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
                }
                ImGui::EndChild();

                ImGui::SameLine();

                if (ImGui::BeginChild("button_select_pane", ImVec2(0, 0), ImGuiChildFlags_Border)) {
                    ImGui::Text("Select button binding:");
                    ImGui::PushItemWidth(ImGui::GetColumnWidth());
                    ImGui::Combo("##button_bind_combo", &data->selected_button, button_table, button_table_len);
                }
                ImGui::EndChild();
            } 
            ImGui::EndGroup();
        }
        ImGui::EndChild();


        // NOTE: This panel should be part of the keybind_pane, except the render_audio_panel function which should be
        //       its own unrelated audio source picker.
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
                ImGui::PushItemWidth(ImGui::GetColumnWidth());
                ImGui::InputText("##file_picker", buffer, IM_ARRAYSIZE(buffer));
                ImGui::EndDisabled();
            }
            // File selection picker + other stuff goes here.

            // TODO: Show selected audio source name here.

            ImGui::SeparatorText("Audio sources");
            render_audio_panel(data->audio_data);
        } 
        ImGui::EndChild();
    } 
    ImGui::EndGroup();

}

static void render_menu_layout(Menu_Data *data) {
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

    display_menu_bar(data);

    ImGui::BeginGroup(); {
        if (ImGui::BeginChild("keybinds_list_panel", ImVec2(100, 0), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeX)) {
            display_keybinds_list_panel(data);
        }
        ImGui::EndChild();

        ImGui::SameLine();

        if (ImGui::BeginChild("configuration_panel")) {
            display_configuration_panel(data);
        }
        ImGui::EndChild();
    }
    ImGui::EndGroup();

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
    GLFWwindow *window = glfwCreateWindow(1000, 700, "GlobalTalk Configuration Menu", nullptr, nullptr);
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

    Menu_Data data = {
        .config = {},

        .demo = false,

        .on_release = true,
        .on_press   = false,

        .selected_button = 0,

        .mute   = true,
        .unmute = false,
        .toggle = false,

        .audio_data = create_audio_gui_data(),
    };

    Config config = load_config();
    data.config = config;

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
