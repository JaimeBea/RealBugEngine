#include "PanelConfiguration.h"

#include "Application.h"
#include "Utils/Logging.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleTime.h"
#include "Modules/ModuleHardwareInfo.h"
#include "Modules/ModuleWindow.h"
#include "Modules/ModuleScene.h"
#include "Modules/ModuleRender.h"
#include "Modules/ModuleCamera.h"
#include "Modules/ModuleResources.h"

#include "GL/glew.h"
#include "imgui.h"

#include "Utils/Leaks.h"

PanelConfiguration::PanelConfiguration()
	: Panel("Configuration", true) {}

void PanelConfiguration::Update()
{
	ImGui::SetNextWindowDockID(App->editor->dock_left_id, ImGuiCond_FirstUseEver);
	if (ImGui::Begin(name, &enabled))
	{
		// Application
		if (ImGui::CollapsingHeader("Application"))
		{
			if (ImGui::InputText("App name", App->app_name, IM_ARRAYSIZE(App->app_name)))
			{
				App->window->SetTitle(App->app_name);
			}
			ImGui::InputText("Organization", App->organization, IM_ARRAYSIZE(App->organization));
		}

		// Time
		if (ImGui::CollapsingHeader("Time"))
		{
			ImGui::SliderInt("Max FPS", &App->time->max_fps, 1, 240);
			ImGui::Checkbox("Limit framerate", &App->time->limit_framerate);
			if (ImGui::Checkbox("VSync", &App->time->vsync))
			{
				App->renderer->SetVSync(App->time->vsync);
			}
			ImGui::SliderInt("Step delta time (MS)", &App->time->step_delta_time_ms, 1, 1000);

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
			ImGui::Text("GLEW version:");
			ImGui::SameLine();
			ImGui::TextColored(App->editor->text_color, App->hardware->glew_version);
			ImGui::Text("SDL version:");
			ImGui::SameLine();
			ImGui::TextColored(App->editor->text_color, App->hardware->sdl_version);
			ImGui::Text("Assimp version:");
			ImGui::SameLine();
			ImGui::TextColored(App->editor->text_color, App->hardware->assimp_version);
			ImGui::Text("DeviL version:");
			ImGui::SameLine();
			ImGui::TextColored(App->editor->text_color, App->hardware->devil_version);

			ImGui::Separator();

			ImGui::Text("CPUs:");
			ImGui::SameLine();
			ImGui::TextColored(App->editor->text_color, "%i (Cache: %i kb)", App->hardware->cpu_count, App->hardware->cache_size_kb);
			ImGui::Text("System RAM:");
			ImGui::SameLine();
			ImGui::TextColored(App->editor->text_color, "%.1f Gb", App->hardware->ram_gb);
			ImGui::Text("Caps:");
			const char* items[] = {"3DNow", "ARMSIMD", "AVX", "AVX2", "AVX512F", "AltiVec", "MMX", "NEON", "RDTSC", "SSE", "SSE2", "SSE3", "SSE41", "SSE42"};
			for (int i = 0; i < IM_ARRAYSIZE(items); ++i)
			{
				if (App->hardware->caps[i])
				{
					ImGui::SameLine();
					ImGui::TextColored(App->editor->text_color, items[i]);
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
			ImGui::TextColored(App->editor->text_color, "%s", App->hardware->gpu_vendor);
			ImGui::Text("GPU Renderer:");
			ImGui::SameLine();
			ImGui::TextColored(App->editor->text_color, "%s", App->hardware->gpu_renderer);
			ImGui::Text("GPU OpenGL Version:");
			ImGui::SameLine();
			ImGui::TextColored(App->editor->text_color, "%s", App->hardware->gpu_opengl_version);
			ImGui::Text("VRAM Budget:");
			ImGui::SameLine();
			ImGui::TextColored(App->editor->text_color, "%.1f Mb", App->hardware->vram_budget_mb);
			ImGui::Text("VRAM Usage:");
			ImGui::SameLine();
			ImGui::TextColored(App->editor->text_color, "%.1f Mb", App->hardware->vram_usage_mb);
			ImGui::Text("VRAM Available:");
			ImGui::SameLine();
			ImGui::TextColored(App->editor->text_color, "%.1f Mb", App->hardware->vram_available_mb);
			ImGui::Text("VRAM Reserved:");
			ImGui::SameLine();
			ImGui::TextColored(App->editor->text_color, "%.1f Mb", App->hardware->vram_reserved_mb);
		}

		// Window
		if (ImGui::CollapsingHeader("Window"))
		{
			// Window mode combo box
			const char* items[] = {"Windowed", "Borderless", "Fullscreen", "Fullscreen desktop"};
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

		// Scene
		if (ImGui::CollapsingHeader("Scene"))
		{
			// TODO: Change the Skybox images
			ImGui::TextColored(App->editor->title_color, "Gizmos");
			ImGui::Checkbox("Draw Bounding Boxes", &App->renderer->draw_all_bounding_boxes);
			ImGui::Checkbox("Draw Quadtree", &App->renderer->draw_quadtree);
			ImGui::Separator();
			ImGui::InputFloat2("Min Point", App->scene->quadtree_bounds.minPoint.ptr());
			ImGui::InputFloat2("Max Point", App->scene->quadtree_bounds.maxPoint.ptr());
			ImGui::InputScalar("Max Depth", ImGuiDataType_U32, &App->scene->quadtree_max_depth);
			ImGui::InputScalar("Elements Per Node", ImGuiDataType_U32, &App->scene->quadtree_elements_per_node);
			if (ImGui::Button("Clear Quadtree"))
			{
				App->scene->ClearQuadtree();
			}
			ImGui::SameLine();
			if (ImGui::Button("Rebuild Quadtree"))
			{
				App->scene->RebuildQuadtree();
			}
			ImGui::Separator();

			ImGui::Checkbox("Skybox", &App->renderer->skybox_active);
			ImGui::ColorEdit3("Background", App->renderer->clear_color.ptr());
			ImGui::ColorEdit3("Ambient Color", App->renderer->ambient_color.ptr());
		}

		// Camera
		if (ImGui::CollapsingHeader("Engine Camera"))
		{
			Frustum& frustum = App->camera->GetEngineFrustum();
			vec front = frustum.Front();
			vec up = frustum.Up();
			ImGui::TextColored(App->editor->title_color, "Frustum");
			ImGui::InputFloat3("Front", front.ptr(), "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat3("Up", up.ptr(), "%.3f", ImGuiInputTextFlags_ReadOnly);

			float near_plane = frustum.NearPlaneDistance();
			float far_plane = frustum.FarPlaneDistance();
			if (ImGui::DragFloat("Near Plane", &near_plane, 0.1f, 0.0f, far_plane, "%.2f"))
			{
				App->camera->engine_camera_frustum.SetViewPlaneDistances(near_plane, far_plane);
			}
			if (ImGui::DragFloat("Far Plane", &far_plane, 1.0f, near_plane, inf, "%.2f"))
			{
				App->camera->engine_camera_frustum.SetViewPlaneDistances(near_plane, far_plane);
			}
		}
	}
	ImGui::End();
}
