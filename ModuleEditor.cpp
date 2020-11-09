#include "ModuleEditor.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "SDL.h"

ModuleEditor::ModuleEditor()
{
}

ModuleEditor::~ModuleEditor()
{
}

bool ModuleEditor::Init()
{
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    return true;
}

bool ModuleEditor::PostInit()
{
    ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer->context);
    ImGui_ImplOpenGL3_Init(GLSL_VERSION);

    return true;
}

update_status ModuleEditor::PreUpdate()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(App->window->window);
    ImGui::NewFrame();

    return UPDATE_CONTINUE;
}

update_status ModuleEditor::Update()
{
    // Main menu bar
    ImGui::BeginMainMenuBar();
    if (ImGui::BeginMenu("Help"))
    {
        if (ImGui::MenuItem("GUI Demo"));

        if (ImGui::MenuItem("Documentation"))
            App->RequestBrowser("https://github.com/JaimeBea/UPCMasterEngine/wiki");

        if (ImGui::MenuItem("Download latest"))
            App->RequestBrowser("https://github.com/JaimeBea/UPCMasterEngine/releases");

        if (ImGui::MenuItem("Report a bug"))
            App->RequestBrowser("https://github.com/JaimeBea/UPCMasterEngine/issues");

        if (ImGui::MenuItem("About"));

        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();

    // Console
    ImGui::SetNextWindowSize(ImVec2(600, 200), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Console"))
    {
        for (char* log_line : log_lines)
        {
            ImGui::TextUnformatted(log_line);
        }
    }
    ImGui::End();

    // Configuration
    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Configuration"))
    {
        // Application
        if (ImGui::CollapsingHeader("Application"))
        {
            if (ImGui::InputText("App name", APP_NAME.GetChars(), APP_NAME.GetMaxSize()))
            {
                SDL_SetWindowTitle(App->window->window, APP_NAME.GetChars());
            }
            if (ImGui::InputText("Organization", ORGANIZATION_NAME.GetChars(), ORGANIZATION_NAME.GetMaxSize()))
            {
                SDL_SetWindowTitle(App->window->window, ORGANIZATION_NAME.GetChars());
            }
            ImGui::SliderInt("Max FPS", &MAX_FPS, 1, 240);
            ImGui::Checkbox("Limit framerate", &LIMIT_FRAMERATE);
            if (ImGui::Checkbox("VSync", &VSYNC))
            {
                SDL_GL_SetSwapInterval(VSYNC);
            }

            // FPS Graph
            char title[25];
            sprintf_s(title, 25, "Framerate %.1f", fps_log[fps_log_index]);
            ImGui::PlotHistogram("##framerate", &fps_log[0], FPS_LOG_SIZE, fps_log_index, title, 0.0f, 100.0f, ImVec2(310, 100));
            sprintf_s(title, 25, "Milliseconds %0.1f", ms_log[fps_log_index]);
            ImGui::PlotHistogram("##milliseconds", &ms_log[0], FPS_LOG_SIZE, fps_log_index, title, 0.0f, 40.0f, ImVec2(310, 100));
        }

        // Window
        if (ImGui::CollapsingHeader("Window"))
        {
            // Screen mode combo box
            const char* items[] = { "Windowed", "Borderless", "Fullscreen", "Fullscreen desktop" };
            const char* item_current = items[SCREEN_MODE];
            if (ImGui::BeginCombo("Screen mode", item_current))
            {
                for (int n = 0; n < IM_ARRAYSIZE(items); ++n)
                {
                    bool is_selected = (item_current == items[n]);
                    if (ImGui::Selectable(items[n], is_selected))
                    {
                        SCREEN_MODE = n;
                        App->window->SetScreenMode(SCREEN_MODE);
                    }
                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            if (ImGui::SliderFloat("Brightness", &BRIGHTNESS, 0.25f, 1.0f))
            {
                App->window->SetBrightness(BRIGHTNESS);
            }

            if (SCREEN_MODE != SM_FULLSCREEN_DESKTOP)
            {
                if (ImGui::Checkbox("Resizable", &RESIZABLE))
                {
                    App->window->SetResizable(RESIZABLE);
                }
                if (RESIZABLE)
                {
                    if (ImGui::SliderInt("Width", &SCREEN_WIDTH, 640, 4096))
                    {
                        App->window->SetSize(SCREEN_WIDTH, SCREEN_HEIGHT);
                    }
                    if (ImGui::SliderInt("Height", &SCREEN_HEIGHT, 480, 2160))
                    {
                        App->window->SetSize(SCREEN_WIDTH, SCREEN_HEIGHT);
                    }
                }
            }
        }
    }
    ImGui::End();

    return UPDATE_CONTINUE;
}

update_status ModuleEditor::PostUpdate()
{
    // Render main window
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Handle and render other windows
    SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
    SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    SDL_GL_MakeCurrent(backup_current_window, backup_current_context);

    return UPDATE_CONTINUE;
}

bool ModuleEditor::CleanUp()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    return true;
}
