#include "gui/config_menu.h"

// This menu is supposed to be used for the configuration purposes as an alternative to starting the program.
// The layout part could be reused in the runtime menu, so the user could use the 
// configuration menu while the backgroud program logic is running.

static void glfw_error_callback(int error, const char *description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

static void create_menu_layout()
{
    bool open_flag = true;
    bool *p_open = &open_flag;

    ImGui::SetNextWindowBgAlpha(1.0f);
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    bool always_on = true;
    auto root_window_flags = 
        ImGuiWindowFlags_NoDecoration | 
        ImGuiWindowFlags_NoResize     | 
        ImGuiWindowFlags_NoBringToFrontOnFocus;
    ImGui::Begin("Something", &always_on, root_window_flags);
    ImGui::PopStyleVar(1);


    bool show_demo_window = true;
    ImGui::ShowDemoWindow(&show_demo_window);

    { // Left
        ImGui::BeginGroup();
        ImGui::Text("Your bindings");
        ImGui::BeginChild("left pane", ImVec2(150, 0), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeX);
        for (int i = 0; i < 10; i++)
        {
            char label[128];
            sprintf(label, "Binding %d", i);


            if (ImGui::CollapsingHeader(label))
            {
                ImGui::SeparatorText("Assigned keys:");
                ImGui::Text("Ctrl + a");

                ImGui::SeparatorText("Assigned action:");
                ImGui::Text("Mute microphone");

                ImGui::Separator();
                if (ImGui::Button("Remove")) {}

                ImGui::SameLine();

                if (ImGui::Button("Edit")) {}
                ImGui::Separator();
            }
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }

    ImGui::SameLine();
    
    { // Right
        ImGui::BeginGroup();
        ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
        ImGui::Text("Create binding");
        ImGui::Separator();
        if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
        {
            if (ImGui::BeginTabItem("Key mapping"))
            {
                ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. ");
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Bind action"))
            {
                ImGui::Text("ID: 0123456789");
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::EndChild();

        if (ImGui::Button("Add")) {}

        ImGui::SameLine();

        if (ImGui::Button("Reset")) {}

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
    GLFWwindow *window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
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
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImVec4 clear_color = ImVec4(1.0, 1.0, 1.0, 1.0);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Begin frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        create_menu_layout();

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
