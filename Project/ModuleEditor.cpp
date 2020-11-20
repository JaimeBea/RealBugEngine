#include "ModuleEditor.h"

#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "GL/glew.h"
#include "SDL_video.h"

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
    if (ImGui::BeginMenu("View")) {
        ImGui::MenuItem(panel_scene.name, "", &panel_scene.enabled);
        ImGui::MenuItem(panel_console.name, "", &panel_console.enabled);
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

    ImGui::ShowDemoWindow();

    // Docking
    ImGui::DockSpaceOverViewport();

    // Panels
    for (Panel* panel : panels)
    {
        if (panel->enabled)
        {
            ImGui::SetNextWindowSize(ImVec2(panel->width, panel->height), ImGuiCond_FirstUseEver);
            if (ImGui::Begin(panel->name, &panel->enabled))
            {
                panel->Update();
            }
            ImGui::End();
        }
    }

    return UpdateStatus::CONTINUE;
}

UpdateStatus ModuleEditor::PostUpdate()
{
    // Draw to default frame buffer (main window)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
