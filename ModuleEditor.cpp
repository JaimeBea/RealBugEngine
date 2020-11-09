#include "ModuleEditor.h"
#include "Application.h"
#include "ModuleConfig.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "SDL_video.h"

static ImVec4 yellow = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);

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
            if (ImGui::InputText("App name", App->config->app_name.GetChars(), App->config->app_name.GetMaxSize()))
            {
                App->window->SetTitle(App->config->app_name.GetChars());
            }
            ImGui::InputText("Organization", App->config->organization.GetChars(), App->config->organization.GetMaxSize());
            ImGui::SliderInt("Max FPS", &App->config->max_fps, 1, 240);
            ImGui::Checkbox("Limit framerate", &App->config->limit_framerate);
            if (ImGui::Checkbox("VSync", &App->config->vsync))
            {
                App->renderer->SetVSync(App->config->vsync);
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
            const char* item_current = items[App->config->screen_mode];
            if (ImGui::BeginCombo("Screen mode", item_current))
            {
                for (int n = 0; n < IM_ARRAYSIZE(items); ++n)
                {
                    bool is_selected = (item_current == items[n]);
                    if (ImGui::Selectable(items[n], is_selected))
                    {
                        App->config->screen_mode = n;
                        App->window->SetScreenMode(App->config->screen_mode);
                    }
                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            if (ImGui::SliderFloat("Brightness", &App->config->brightness, 0.25f, 1.0f))
            {
                App->window->SetBrightness(App->config->brightness);
            }

            if (App->config->screen_mode != SM_FULLSCREEN_DESKTOP)
            {
                if (ImGui::Checkbox("Resizable", &App->config->resizable))
                {
                    App->window->SetResizable(App->config->resizable);
                }
                if (App->config->resizable)
                {
                    if (ImGui::SliderInt("Width", &App->config->screen_width, 640, 4096))
                    {
                        App->window->SetSize(App->config->screen_width, App->config->screen_height);
                    }
                    if (ImGui::SliderInt("Height", &App->config->screen_height, 480, 2160))
                    {
                        App->window->SetSize(App->config->screen_width, App->config->screen_height);
                    }
                }
            }
        }

        // Hardware
        if (ImGui::CollapsingHeader("Hardware"))
        {
            ImGui::Text("SDL version:");
            ImGui::SameLine();
            ImGui::TextColored(yellow, App->config->sdl_version.GetChars());

            ImGui::Separator();

            ImGui::Text("CPUs:");
            ImGui::SameLine();
            ImGui::TextColored(yellow, "%i (Cache: %i kb)", App->config->cpu_count, App->config->cache_size_kb);
            ImGui::Text("System RAM:");
            ImGui::SameLine();
            ImGui::TextColored(yellow, "%.1f Gb", App->config->ram_gb);
            ImGui::Text("Caps:");
            const char* items[] =
            {
                "3DNow", "ARMSIMD", "AVX", "AVX2", "AVX512F", "AltiVec", "MMX",
                "NEON", "RDTSC", "SSE", "SSE2", "SSE3", "SSE41", "SSE42"
            };
            for (int i = 0; i < IM_ARRAYSIZE(items); ++i)
            {
                if (App->config->caps[i])
                {
                    ImGui::SameLine();
                    ImGui::TextColored(yellow, items[i]);
                }

                // Line break to avoid too many items in the same line
                if (i == 6)
                {
                    ImGui::Text("");
                }
            }

            ImGui::Separator();

            ImGui::Text("GPU Vendor:");
            ImGui::SameLine();
            ImGui::TextColored(yellow, "%s", App->config->gpu_vendor);
            ImGui::Text("GPU Renderer:");
            ImGui::SameLine();
            ImGui::TextColored(yellow, "%s", App->config->gpu_renderer);
            ImGui::Text("GPU OpenGL Version:");
            ImGui::SameLine();
            ImGui::TextColored(yellow, "%s", App->config->gpu_opengl_version);
            ImGui::Text("VRAM Budget:");
            ImGui::SameLine();
            ImGui::TextColored(yellow, "%.1f Mb", App->config->vram_budget_mb);
            ImGui::Text("VRAM Usage:");
            ImGui::SameLine();
            ImGui::TextColored(yellow, "%.1f Mb", App->config->vram_usage_mb);
            ImGui::Text("VRAM Available:");
            ImGui::SameLine();
            ImGui::TextColored(yellow, "%.1f Mb", App->config->vram_available_mb);
            ImGui::Text("VRAM Reserved:");
            ImGui::SameLine();
            ImGui::TextColored(yellow, "%.1f Mb", App->config->vram_reserved_mb);
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
