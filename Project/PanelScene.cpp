#include "PanelScene.h"

#include "Globals.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleRender.h"

#include "imgui.h"
#include "Math/float3x3.h"
#include "SDL_mouse.h"
#include "SDL_scancode.h"

PanelScene::PanelScene() : Panel("Scene", 800, 600) {}

void PanelScene::Update()
{
	// Update viewport size
	ImVec2 size = ImGui::GetContentRegionAvail();
	if (App->renderer->viewport_width != size.x || App->renderer->viewport_height != size.y)
	{
		App->camera->ViewportResized(size.x, size.y);
		App->renderer->ViewportResized(size.x, size.y);
	}

	// Draw
	ImGui::Image((void*)App->renderer->render_texture, size, ImVec2(0, 1), ImVec2(1, 0));

	// Capture input
	if (ImGui::IsWindowFocused())
	{
		ImGui::CaptureKeyboardFromApp(false);
		if (ImGui::IsAnyMouseDown() || ImGui::IsItemHovered())
		{
			ImGui::CaptureMouseFromApp(false);
		}
	}
}
