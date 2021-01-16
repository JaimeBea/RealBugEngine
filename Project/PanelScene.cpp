#include "PanelScene.h"

#include "Globals.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentBoundingBox.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleEditor.h"
#include "ModuleCamera.h"
#include "ModuleRender.h"
#include "ModuleTime.h"

#include "imgui.h"
#include "ImGuizmo.h"
#include "Math/float3x3.h"
#include "Math/float2.h"
#include "Geometry/OBB.h"
#include "SDL_mouse.h"
#include "SDL_scancode.h"
#include "Logging.h"
#include <algorithm>

#include "Leaks.h"

PanelScene::PanelScene()
	: Panel("Scene", true) {}

void PanelScene::Update()
{
	int imguizmo_size = 100;

	ImGui::SetNextWindowDockID(App->editor->dock_main_id, ImGuiCond_FirstUseEver);
	if (ImGui::Begin(name, &enabled))
	{
		// Play / Pause / Step buttons
		if (App->time->HasGameStarted())
		{
			if (ImGui::Button("Stop"))
			{
				App->time->StopGame();
			}
			ImGui::SameLine();
			if (App->time->IsGameRunning())
			{
				if (ImGui::Button("Pause"))
				{
					App->time->PauseGame();
				}
			}
			else
			{
				if (ImGui::Button("Resume"))
				{
					App->time->ResumeGame();
				}
			}
		}
		else
		{
			if (ImGui::Button("Play"))
			{
				App->time->StartGame();
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Step"))
		{
			App->time->StepGame();
		}

		// Update viewport size
		ImVec2 size = ImGui::GetContentRegionAvail();
		if (App->renderer->viewport_width != size.x || App->renderer->viewport_height != size.y)
		{
			App->camera->ViewportResized((int) size.x, (int) size.y);
			App->renderer->ViewportResized((int) size.x, (int) size.y);
			framebuffer_size = {
				size.x,
				size.y,
			};
		}

		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImDrawVert* last_vertex = draw_list->VtxBuffer.end();
		framebuffer_position = {
			last_vertex->pos.x,
			last_vertex->pos.y,
		};

		// Draw
		ImGui::Image((void*) App->renderer->render_texture, size, ImVec2(0, 1), ImVec2(1, 0));

		// Capture input
		if (ImGui::IsWindowFocused())
		{
			if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) || App->input->GetKey(SDL_SCANCODE_LALT))
			{
				ImGuizmo::Enable(false);
			}
			else
			{
				ImGuizmo::Enable(true);
			}

			ImGui::CaptureKeyboardFromApp(false);

			if (ImGui::IsMouseClicked(0) && ImGui::IsItemHovered() && !ImGuizmo::IsOver())
			{
				ImGui::CaptureMouseFromApp(true);
				ImGuiIO& io = ImGui::GetIO();
				float2 mouse_pos_normalized;
				mouse_pos_normalized.x = -1 + 2 * std::max(-1.0f, std::min((io.MousePos.x - framebuffer_position.x) / (size.x), 1.0f));
				mouse_pos_normalized.y = 1 - 2 * std::max(-1.0f, std::min((io.MousePos.y - framebuffer_position.y) / (size.y), 1.0f));
				App->camera->CalculateFrustumNearestObject(mouse_pos_normalized);
			}
			ImGui::CaptureMouseFromApp(false);
		}

		float view_manipulate_right = framebuffer_position.x + framebuffer_size.x;
		float view_manipulate_top = framebuffer_position.y;
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(framebuffer_position.x, framebuffer_position.y, framebuffer_size.x, framebuffer_size.y);

		Frustum& engine_frustum = App->camera->GetEngineFrustum();
		float4x4 camera_view = float4x4(engine_frustum.ViewMatrix()).Transposed();
		float4x4 camera_projection = engine_frustum.ProjectionMatrix().Transposed();

		GameObject* selected_object = App->editor->panel_hierarchy.selected_object;
		if (selected_object)
		{
			ComponentTransform* transform = selected_object->GetComponent<ComponentTransform>();
			GameObject* parent = selected_object->GetParent();
			float4x4 inverse_parent_matrix = float4x4::identity;
			if (parent != nullptr)
			{
				ComponentTransform* parent_transform = parent->GetComponent<ComponentTransform>();
				inverse_parent_matrix = parent_transform->GetGlobalMatrix().Inverted();
			}
			float4x4 global_matrix = transform->GetGlobalMatrix().Transposed();

			if (ImGuizmo::Manipulate(camera_view.ptr(), camera_projection.ptr(), transform->GetGizmoOperation(), transform->GetGizmoMode(), global_matrix.ptr(), NULL, transform->GetUseSnap() ? transform->GetSnap().ptr() : NULL))
			{
				float4x4 local_matrix = inverse_parent_matrix * global_matrix.Transposed();

				float3 translation;
				Quat rotation;
				float3 scale;
				local_matrix.Decompose(translation, rotation, scale);

				transform->SetPosition(translation);
				transform->SetScale(scale);
				transform->SetRotation(rotation);
				selected_object->OnTransformUpdate();
			}
		}

		ImGuizmo::ViewManipulate(camera_view.ptr(), 4, ImVec2(view_manipulate_right - imguizmo_size, view_manipulate_top), ImVec2(imguizmo_size, imguizmo_size), 0x10101010);

		float4x4 new_camera_view = camera_view.InverseTransposed();
		App->camera->engine_camera_frustum.SetFrame(new_camera_view.Col(3).xyz(), -new_camera_view.Col(2).xyz(), new_camera_view.Col(1).xyz());

		ImGui::End();
	}
}
