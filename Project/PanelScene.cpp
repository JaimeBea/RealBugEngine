#include "PanelScene.h"

#include "Globals.h"
#include "GameObject.h"
#include "ComponentTransform.h"
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
#include "SDL_mouse.h"
#include "SDL_scancode.h"
#include "Logging.h"
#include <algorithm>

#include "Leaks.h"

PanelScene::PanelScene()
	: Panel("Scene", true) {}

void PanelScene::Update()
{
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

			int count = 0;
			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			ImDrawVert* last_vertex = draw_list->VtxBuffer.end();
			framebuffer_position = {
				last_vertex->pos.x,
				last_vertex->pos.y,
			};
			framebuffer_size = {
				size.x,
				size.y,
			};
		}

		//LOG("x: %f , y: %f", frame_buffer_position.x, frame_buffer_position.y);

		// Draw
		ImGui::Image((void*) App->renderer->render_texture, size, ImVec2(0, 1), ImVec2(1, 0));

		// Capture input
		if (ImGui::IsWindowFocused())
		{
			ImGui::CaptureKeyboardFromApp(false);
			ImGui::CaptureMouseFromApp(true);
			if (ImGui::IsMouseClicked(0) && ImGui::IsItemHovered())
			{
				ImGuiIO& io = ImGui::GetIO();
				float2 mouse_pos_normalized;
				mouse_pos_normalized.x = -1 + 2 * std::max(-1.0f, std::min((io.MousePos.x - framebuffer_position.x) / (size.x), 1.0f));
				mouse_pos_normalized.y = 1 - 2 * std::max(-1.0f, std::min((io.MousePos.y - framebuffer_position.y) / (size.y), 1.0f));
				LOG("ImGui x: %f , y: %f", mouse_pos_normalized.x, mouse_pos_normalized.y);
				App->camera->CalculateFrustumNearestObject(mouse_pos_normalized);
			}
			ImGui::CaptureMouseFromApp(false);
		}
		float viewManipulateRight = framebuffer_position.x + size.x;
		float viewManipulateTop = framebuffer_position.y;

		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(framebuffer_position.x, framebuffer_position.y, size.x, size.y);

		Frustum& default_frustum = App->camera->engine_camera_frustum;
		float4x4 camera_view = default_frustum.ViewMatrix();
		float4x4 camera_projection = default_frustum.ProjectionMatrix();

		GameObject* selected_object = App->editor->panel_hierarchy.selected_object;
		if (selected_object)
		{
			ComponentTransform* transform = selected_object->GetComponent<ComponentTransform>();
			transform->CalculateGlobalMatrix();
			float4x4 matrix = transform->GetGlobalMatrix();
			ImGuizmo::DrawCubes(camera_view.ptr(), camera_projection.ptr(), matrix.ptr(), 0);
			ImGuizmo::Manipulate(camera_view.ptr(), camera_projection.ptr(), transform->GetGizmoOperation(), transform->GetGizmoMode(), matrix.ptr(), NULL); //, useSnap ? &snap[0] : NULL); // boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);

			// Update Transform values
			//float matrixTranslation[3], matrixRotation[3], matrixScale[3];
			//ImGuizmo::DecomposeMatrixToComponents(matrix.ptr(), matrixTranslation, matrixRotation, matrixScale);
			//transform->SetPosition(float3(matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]));
			//transform->SetRotation(Quat::FromEulerXYZ(matrixRotation[0] * DEGTORAD, matrixRotation[1] * DEGTORAD, matrixRotation[2] * DEGTORAD));
			//transform->SetScale(float3(matrixScale[0], matrixScale[1], matrixScale[2]));
		}
		ImGuizmo::ViewManipulate(camera_view.ptr(), 0, ImVec2(viewManipulateRight - 100, viewManipulateTop), ImVec2(100, 100), 0x10101010);

		ImGui::End();
	}
}
