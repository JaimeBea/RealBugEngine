#pragma once

#include "Panel.h"

#include "Math/float2.h"
#include "imgui.h"
#include "ImGuizmo.h"

struct ImDrawList;

class PanelScene : public Panel {
public:
	PanelScene();

	void Update() override;

private:
	float2 framebufferSize = {0.0f, 0.0f};

	//ImGuizmo
	ImGuizmo::OPERATION currentGuizmoOperation = ImGuizmo::TRANSLATE;
	ImGuizmo::MODE currentGuizmoMode = ImGuizmo::WORLD;

	bool useSnap = false;
	float snap[3] = {1.f, 1.f, 1.f};
};
