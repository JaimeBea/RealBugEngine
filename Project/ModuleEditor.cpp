#include "ModuleEditor.h"

#include "Globals.h"
#include "Application.h"
#include "Logging.h"
#include "ModuleConfig.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "ModuleModels.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "SDL_video.h"

static ImVec4 yellow = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);

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

UpdateStatus ModuleEditor::PreUpdate()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(App->window->window);
    ImGui::NewFrame();

    return UpdateStatus::CONTINUE;
}

UpdateStatus ModuleEditor::Update()
{
    // Main menu bar
    ImGui::BeginMainMenuBar();
    if (ImGui::BeginMenu("View"))
    {
        ImGui::MenuItem("Configuration", "", &show_config);
        ImGui::MenuItem("Console", "", &show_console);
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
        ImGui::MenuItem("About", "", &show_about);
        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();

    // Console
    if (show_console)
    {
        ImGui::SetNextWindowSize(ImVec2(600, 200), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Console", &show_console))
        {
            for (char* log_line : log_lines)
            {
                ImGui::TextUnformatted(log_line);
            }
        }
        ImGui::End();
    }

    // Configuration
    if (show_config) {
        ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Configuration", &show_config))
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
                // Window mode combo box
                const char* items[] = { "Windowed", "Borderless", "Fullscreen", "Fullscreen desktop" };
                const char* item_current = items[int(App->config->window_mode)];
                if (ImGui::BeginCombo("Window mode", item_current))
                {
                    for (int n = 0; n < IM_ARRAYSIZE(items); ++n)
                    {
                        bool is_selected = (item_current == items[n]);
                        if (ImGui::Selectable(items[n], is_selected))
                        {
                            App->config->window_mode = WindowMode(n);
                            App->window->SetWindowMode(App->config->window_mode);
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

                if (App->config->window_mode != WindowMode::FULLSCREEN_DESKTOP)
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

            // Camera
            if (ImGui::CollapsingHeader("Camera"))
            {
                vec front = App->camera->GetFront();
                vec up = App->camera->GetUp();
                ImGui::InputFloat3("Front", front.ptr(), "%.3f", ImGuiInputTextFlags_ReadOnly);
                ImGui::InputFloat3("Up", up.ptr(), "%.3f", ImGuiInputTextFlags_ReadOnly);
                vec position = App->camera->GetPosition();
                if (ImGui::InputFloat3("Position", position.ptr()))
                {
                    App->camera->SetPosition(position);
                }
                ImGui::InputFloat("Mov Speed", &App->camera->movement_speed);
                ImGui::InputFloat("Rot Speed", &App->camera->rotation_speed);
                ImGui::InputFloat("Zoom Speed", &App->camera->zoom_speed);
                float near_plane = App->camera->GetNearPlane();
                float far_plane = App->camera->GetFarPlane();
                if (ImGui::InputFloat("Near Plane", &near_plane))
                {
                    App->camera->SetPlaneDistances(near_plane, far_plane);
                }
                if (ImGui::InputFloat("Far Plane", &far_plane))
                {
                    App->camera->SetPlaneDistances(near_plane, far_plane);
                }
                float fov = App->camera->GetFOV();
                if (ImGui::InputFloat("Field of View", &fov))
                {
                    App->camera->SetFOV(fov);
                }
                float aspect_ratio = App->camera->GetAspectRatio();
                if (ImGui::InputFloat("Aspect Ratio", &aspect_ratio))
                {
                    App->camera->SetAspectRatio(aspect_ratio);
                }
                ImGui::ColorPicker3("Background", App->renderer->clear_color.ptr());
            }

            // Textures
            if (ImGui::CollapsingHeader("Textures"))
            {
                // Min filter combo box
                const char* min_filter_items[] = { "Nearest", "Linear", "Nearest Mipmap Nearest", "Linear Mipmap Nearest", "Nearest Mipmap Linear", "Linear Mipmap Linear" };
                const char* min_filter_item_current = min_filter_items[int(App->config->min_filter)];
                if (ImGui::BeginCombo("Min filter", min_filter_item_current))
                {
                    for (int n = 0; n < IM_ARRAYSIZE(min_filter_items); ++n)
                    {
                        bool is_selected = (min_filter_item_current == min_filter_items[n]);
                        if (ImGui::Selectable(min_filter_items[n], is_selected))
                        {
                            App->config->min_filter = TextureFilter(n);
                            App->textures->SetMinFilter(App->config->min_filter);
                        }
                        if (is_selected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }

                // Mag filter combo box
                const char* mag_filter_items[] = { "Nearest", "Linear", "Nearest Mipmap Nearest", "Linear Mipmap Nearest", "Nearest Mipmap Linear", "Linear Mipmap Linear" };
                const char* mag_filter_item_current = mag_filter_items[int(App->config->mag_filter)];
                if (ImGui::BeginCombo("Mag filter", mag_filter_item_current))
                {
                    for (int n = 0; n < IM_ARRAYSIZE(mag_filter_items); ++n)
                    {
                        bool is_selected = (mag_filter_item_current == mag_filter_items[n]);
                        if (ImGui::Selectable(mag_filter_items[n], is_selected))
                        {
                            App->config->mag_filter = TextureFilter(n);
                            App->textures->SetMagFilter(App->config->mag_filter);
                        }
                        if (is_selected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }

                // Texture wrap combo box
                const char* wrap_items[] = { "Repeat", "Clamp to Edge", "Clamp to Border", "Mirrored Repeat", "Mirrored Clamp to Edge" };
                const char* wrap_item_current = wrap_items[int(App->config->texture_wrap)];
                if (ImGui::BeginCombo("Wrap", wrap_item_current))
                {
                    for (int n = 0; n < IM_ARRAYSIZE(wrap_items); ++n)
                    {
                        bool is_selected = (wrap_item_current == wrap_items[n]);
                        if (ImGui::Selectable(wrap_items[n], is_selected))
                        {
                            App->config->texture_wrap = TextureWrap(n);
                            App->textures->SetWrap(App->config->texture_wrap);
                        }
                        if (is_selected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }

                // Texture images
                for (unsigned texture : App->textures->textures)
                {
                    char texture_name[128];
                    sprintf(texture_name, "Texture %i", texture);
                    if (ImGui::TreeNode(texture_name))
                    {
                        ImGui::Image((void*)texture, ImVec2(200, 200));

                        ImGui::TreePop();
                    }
                }
            }

            // Models
            if (ImGui::CollapsingHeader("Models"))
            {
                unsigned model_index = 0;
                for (Model& model : App->models->models)
                {
                    char model_name[128];
                    sprintf(model_name, "Model %i", model_index);
                    if (ImGui::TreeNode(model_name))
                    {
                        unsigned mesh_index = 0;
                        for (Mesh& mesh : model.meshes)
                        {
                            char mesh_name[128];
                            sprintf(mesh_name, "Mesh %i", mesh_index);
                            if (ImGui::TreeNode(mesh_name))
                            {
                                ImGui::Text("Vertices:");
                                ImGui::SameLine();
                                ImGui::TextColored(yellow, "%i", mesh.num_vertices);
                                ImGui::Text("Triangles:");
                                ImGui::SameLine();
                                ImGui::TextColored(yellow, "%i", mesh.num_indices / 3);

                                ImGui::TreePop();
                            }

                            mesh_index += 1;
                        }
                        ImGui::Text("Diffuse:");
                        ImGui::Image((void*)model.materials[0], ImVec2(200, 200));

                        ImGui::TreePop();
                    }

                    model_index += 1;
                }
            }
        }
        ImGui::End();
    }

    // About
    if (show_about) {
        ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("About", &show_about))
        {
            ImGui::Text("Engine Name:");
            ImGui::SameLine();
            ImGui::TextColored(yellow, App->config->engine_name);
            ImGui::Text("Description:");
            ImGui::SameLine();
            ImGui::TextColored(yellow, App->config->engine_description);
            ImGui::Text("Authors:");
            ImGui::SameLine();
            ImGui::TextColored(yellow, App->config->engine_authors);
            ImGui::Text("Libraries:");
            ImGui::SameLine();
            ImGui::TextColored(yellow, App->config->engine_libraries);
            ImGui::Text("License:");
            ImGui::SameLine();
            ImGui::TextColored(yellow, App->config->engine_license);
        }
        ImGui::End();
    }
    return UpdateStatus::CONTINUE;
}

UpdateStatus ModuleEditor::PostUpdate()
{
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
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    return true;
}
