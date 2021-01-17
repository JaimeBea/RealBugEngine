#pragma once

#include "Panel.h"

#include "Math/float2.h"
#include "imgui.h"
#include "ImGuizmo.h"

struct ImDrawList;

class PanelScene : public Panel
{
public:
	PanelScene();

	void Update() override;

private:
	float2 framebuffer_position = {0.0f, 0.0f};
	float2 framebuffer_size = {0.0f, 0.0f};

	//ImGuizmo
	ImGuizmo::OPERATION current_guizmo_operation = ImGuizmo::TRANSLATE;
	ImGuizmo::MODE current_guizmo_mode = ImGuizmo::WORLD;

	bool use_snap = false;
	float snap[3] = {1.f, 1.f, 1.f};
};
