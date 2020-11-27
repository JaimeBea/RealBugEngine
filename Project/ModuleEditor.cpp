#include "ModuleEditor.h"

#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "GL/glew.h"
#include "SDL_video.h"

#include "Leaks.h"

bool ModuleEditor::Init()
{
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    return true;
}

bool ModuleEditor::Start()
{
    ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer->context);
    ImGui_ImplOpenGL3_Init(GLSL_VERSION);

    panels.push_back(&panel_scene);
    panels.push_back(&panel_console);
    panels.push_back(&panel_configuration);
    panels.push_back(&panel_properties);
    panels.push_back(&panel_about);

    return true;
}

UpdateStatus ModuleEditor::PreUpdate()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(App->window->window);
    ImGui::NewFrame();

    return UpdateStatus::CONTINUE;
}

UpdateStatus ModuleEditor::Update()
{
    ImGui::CaptureMouseFromApp(true);
    ImGui::CaptureKeyboardFromApp(true);

    // Main menu bar
    ImGui::BeginMainMenuBar();
    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::MenuItem("Quit"))
        {
            return UpdateStatus::STOP;
        }
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("View"))
    {
        ImGui::MenuItem(panel_scene.name, "", &panel_scene.enabled);
        ImGui::MenuItem(panel_console.name, "", &panel_console.enabled);
        ImGui::MenuItem(panel_properties.name, "", &panel_properties.enabled);
        ImGui::MenuItem(panel_configuration.name, "", &panel_configuration.enabled);
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Help"))
    {
        if (ImGui::MenuItem("Documentation"))
        {
            App->RequestBrowser("https://github.com/JaimeBea/UPCMasterEngine/wiki");
        }
        if (ImGui::MenuItem("Download latest"))
        {
            App->RequestBrowser("https://github.com/JaimeBea/UPCMasterEngine/releases");
        }
        if (ImGui::MenuItem("Report a bug"))
        {
            App->RequestBrowser("https://github.com/JaimeBea/UPCMasterEngine/issues");
        }
        ImGui::MenuItem(panel_about.name, "", &panel_about.enabled);
        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();

    // Docking
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGuiID dock_space_id = ImGui::GetID("DockSpace");

    if (!ImGui::DockBuilderGetNode(dock_space_id)) {
        ImGui::DockBuilderAddNode(dock_space_id);
        ImGui::DockBuilderSetNodeSize(dock_space_id, viewport->GetWorkSize());

        dock_main_id = dock_space_id;
        dock_right_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.25f, nullptr, &dock_main_id);
        dock_down_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.3f, nullptr, &dock_main_id);
    }

    ImGui::SetNextWindowPos(viewport->GetWorkPos());
    ImGui::SetNextWindowSize(viewport->GetWorkSize());

    ImGuiWindowFlags dock_space_window_flags = 0;
    dock_space_window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking;
    dock_space_window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", nullptr, dock_space_window_flags);
    ImGui::PopStyleVar(3);
    ImGui::DockSpace(dock_space_id);
    ImGui::End();

    // Panels
    for (Panel* panel : panels)
    {
        if (panel->enabled)
        {
            panel->Update();
        }
    }

    return UpdateStatus::CONTINUE;
}

UpdateStatus ModuleEditor::PostUpdate()
{
    // Draw to default frame buffer (main window)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Render main window
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Handle and render other windows
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    SDL_GL_MakeCurrent(App->window->window, App->renderer->context);

    return UpdateStatus::CONTINUE;
}

bool ModuleEditor::CleanUp()
{
    panels.clear();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    return true;
}
