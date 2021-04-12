#pragma once

#include "Panel.h"

#include "Math/float2.h"
#include "imgui.h"

struct ImDrawList;

class PanelScene : public Panel {
public:
	PanelScene();

	void Update() override;
	float2 GetMousePosOnScene() const;
	float2 GetSceneWindowSize() const;

	const bool IsUsing2D() const;

private:
	float2 framebufferSize = {0.0f, 0.0f};
	float2 mousePosOnScene = {0.0f, 0.0f};

	bool view2D = false;
};
