#include "PanelConfiguration.h"

#include "Application.h"
#include "Logging.h"
#include "ModuleHardwareInfo.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"
#include "ModuleTextures.h"
#include "ModuleModels.h"

#include "imgui.h"

static ImVec4 yellow = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);

PanelConfiguration::PanelConfiguration() : Panel("Configuration", 520, 600) {}

void PanelConfiguration::Update()
{
    // Application
    if (ImGui::CollapsingHeader("Application"))
    {
        if (ImGui::InputText("App name", App->app_name, IM_ARRAYSIZE(App->app_name)))
        {
            App->window->SetTitle(App->app_name);
        }
        ImGui::InputText("Organization", App->organization, IM_ARRAYSIZE(App->organization));
        ImGui::SliderInt("Max FPS", &App->max_fps, 1, 240);
        ImGui::Checkbox("Limit framerate", &App->limit_framerate);
        if (ImGui::Checkbox("VSync", &App->vsync))
        {
            App->renderer->SetVSync(App->vsync);
        }

        // FPS Graph
        char title[25];
        sprintf_s(title, 25, "Framerate %.1f", fps_log[fps_log_index]);
        ImGui::PlotHistogram("##framerate", &fps_log[0], FPS_LOG_SIZE, fps_log_index, title, 0.0f, 100.0f, ImVec2(310, 100));
        sprintf_s(title, 25, "Milliseconds %0.1f", ms_log[fps_log_index]);
        ImGui::PlotHistogram("##milliseconds", &ms_log[0], FPS_LOG_SIZE, fps_log_index, title, 0.0f, 40.0f, ImVec2(310, 100));
    }

    // Hardware
    if (ImGui::CollapsingHeader("Hardware"))
    {
        ImGui::Text("SDL version:");
        ImGui::SameLine();
        ImGui::TextColored(yellow, App->hardware->sdl_version);

        ImGui::Separator();

        ImGui::Text("CPUs:");
        ImGui::SameLine();
        ImGui::TextColored(yellow, "%i (Cache: %i kb)", App->hardware->cpu_count, App->hardware->cache_size_kb);
        ImGui::Text("System RAM:");
        ImGui::SameLine();
        ImGui::TextColored(yellow, "%.1f Gb", App->hardware->ram_gb);
        ImGui::Text("Caps:");
        const char* items[] =
        {
            "3DNow", "ARMSIMD", "AVX", "AVX2", "AVX512F", "AltiVec", "MMX",
            "NEON", "RDTSC", "SSE", "SSE2", "SSE3", "SSE41", "SSE42"
        };
        for (int i = 0; i < IM_ARRAYSIZE(items); ++i)
        {
            if (App->hardware->caps[i])
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
        ImGui::TextColored(yellow, "%s", App->hardware->gpu_vendor);
        ImGui::Text("GPU Renderer:");
        ImGui::SameLine();
        ImGui::TextColored(yellow, "%s", App->hardware->gpu_renderer);
        ImGui::Text("GPU OpenGL Version:");
        ImGui::SameLine();
        ImGui::TextColored(yellow, "%s", App->hardware->gpu_opengl_version);
        ImGui::Text("VRAM Budget:");
        ImGui::SameLine();
        ImGui::TextColored(yellow, "%.1f Mb", App->hardware->vram_budget_mb);
        ImGui::Text("VRAM Usage:");
        ImGui::SameLine();
        ImGui::TextColored(yellow, "%.1f Mb", App->hardware->vram_usage_mb);
        ImGui::Text("VRAM Available:");
        ImGui::SameLine();
        ImGui::TextColored(yellow, "%.1f Mb", App->hardware->vram_available_mb);
        ImGui::Text("VRAM Reserved:");
        ImGui::SameLine();
        ImGui::TextColored(yellow, "%.1f Mb", App->hardware->vram_reserved_mb);
    }

    // Window
    if (ImGui::CollapsingHeader("Window"))
    {
        // Window mode combo box
        const char* items[] = { "Windowed", "Borderless", "Fullscreen", "Fullscreen desktop" };
        const char* item_current = items[int(App->window->GetWindowMode())];
        if (ImGui::BeginCombo("Window mode", item_current))
        {
            for (int n = 0; n < IM_ARRAYSIZE(items); ++n)
            {
                bool is_selected = (item_current == items[n]);
                if (ImGui::Selectable(items[n], is_selected))
                {
                    App->window->SetWindowMode(WindowMode(n));
                }
                if (is_selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        float brightness = App->window->GetBrightness();
        if (ImGui::SliderFloat("Brightness", &brightness, 0.25f, 1.0f))
        {
            App->window->SetBrightness(brightness);
        }

        if (App->window->GetWindowMode() == WindowMode::BORDERLESS || App->window->GetWindowMode() == WindowMode::WINDOWED)
        {
            bool resizable = App->window->GetResizable();
            if (ImGui::Checkbox("Resizable", &resizable))
            {
                App->window->SetResizable(resizable);
            }
            if (resizable)
            {
                bool size_changed = false;
                bool size_changing = false;
                ImGui::SliderInt("Width", &window_width, 640, 4096);
                if (ImGui::IsItemDeactivatedAfterEdit())
                {
                    size_changed = true;
                }
                if (ImGui::IsItemActive())
                {
                    size_changing = true;
                }
                ImGui::SliderInt("Height", &window_height, 480, 2160);
                if (ImGui::IsItemDeactivatedAfterEdit())
                {
                    size_changed = true;
                }
                if (ImGui::IsItemActive())
                {
                    size_changing = true;
                }

                if (size_changed)
                {
                    App->window->SetSize(window_width, window_height);
                }
                else if (!size_changing)
                {
                    window_width = App->window->GetWidth();
                    window_height = App->window->GetHeight();
                }
            }
        }
        else
        {
            int current_display_mode_index = App->window->GetCurrentDisplayMode();
            const SDL_DisplayMode& current_display_mode = App->window->display_modes[current_display_mode_index];
            char current_display_mode_label[40];
            sprintf_s(current_display_mode_label, " %i bpp\t%i x %i @ %iHz", SDL_BITSPERPIXEL(current_display_mode.format), current_display_mode.w, current_display_mode.h, current_display_mode.refresh_rate);
            
            if (ImGui::BeginCombo("Display Modes", current_display_mode_label))
            {
                int display_mode_index = 0;
                for (const SDL_DisplayMode& display_mode : App->window->display_modes)
                {
                    bool is_selected = (current_display_mode_index == display_mode_index);
                    char display_mode_label[40];
                    sprintf_s(display_mode_label, " %i bpp\t%i x %i @ %iHz", SDL_BITSPERPIXEL(display_mode.format), display_mode.w, display_mode.h, display_mode.refresh_rate);

                    if (ImGui::Selectable(display_mode_label, is_selected))
                    {
                        App->window->SetCurrentDisplayMode(display_mode_index);
                    }

                    display_mode_index += 1;
                }
                ImGui::EndCombo();
            }
        }
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
        ImGui::InputFloat("Shift Multiplier", &App->camera->shift_multiplier);
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
        ImGui::ColorEdit3("Background", App->renderer->clear_color.ptr());
    }

    // Textures
    if (ImGui::CollapsingHeader("Textures"))
    {
        // Min filter combo box
        const char* min_filter_items[] = { "Nearest", "Linear", "Nearest Mipmap Nearest", "Linear Mipmap Nearest", "Nearest Mipmap Linear", "Linear Mipmap Linear" };
        const char* min_filter_item_current = min_filter_items[int(App->textures->GetMinFilter())];
        if (ImGui::BeginCombo("Min filter", min_filter_item_current))
        {
            for (int n = 0; n < IM_ARRAYSIZE(min_filter_items); ++n)
            {
                bool is_selected = (min_filter_item_current == min_filter_items[n]);
                if (ImGui::Selectable(min_filter_items[n], is_selected))
                {
                    App->textures->SetMinFilter(TextureFilter(n));
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
        const char* mag_filter_item_current = mag_filter_items[int(App->textures->GetMagFilter())];
        if (ImGui::BeginCombo("Mag filter", mag_filter_item_current))
        {
            for (int n = 0; n < IM_ARRAYSIZE(mag_filter_items); ++n)
            {
                bool is_selected = (mag_filter_item_current == mag_filter_items[n]);
                if (ImGui::Selectable(mag_filter_items[n], is_selected))
                {
                    App->textures->SetMagFilter(TextureFilter(n));
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
        const char* wrap_item_current = wrap_items[int(App->textures->GetWrap())];
        if (ImGui::BeginCombo("Wrap", wrap_item_current))
        {
            for (int n = 0; n < IM_ARRAYSIZE(wrap_items); ++n)
            {
                bool is_selected = (wrap_item_current == wrap_items[n]);
                if (ImGui::Selectable(wrap_items[n], is_selected))
                {
                    App->textures->SetWrap(TextureWrap(n));
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
        for (const Model& current_model : App->models->models)
        {
            char model_name[128];
            sprintf(model_name, "Model %i", model_index);
            if (ImGui::TreeNode(model_name))
            {
                unsigned mesh_index = 0;
                for (const Mesh& mesh : current_model.meshes)
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
                        ImGui::Text("Material:");
                        ImGui::SameLine();
                        ImGui::TextColored(yellow, "%i", mesh.material_index);

                        ImGui::TreePop();
                    }

                    mesh_index += 1;
                }

                unsigned material_index = 0;
                for (unsigned material : current_model.materials)
                {
                    ImGui::Text("Material %i:", material_index);
                    ImGui::Image((void*)current_model.materials[material_index], ImVec2(200, 200));

                    material_index += 1;
                }

                ImGui::TreePop();
            }

            model_index += 1;
        }
    }
}
