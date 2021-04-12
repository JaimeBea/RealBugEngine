#pragma once

#include "Panel.h"

#include "Math/float2.h"
#include "imgui.h"

struct ImDrawList;

class PanelScene : public Panel {
public:
	PanelScene();

	void Update() override;

	bool IsUsing2D() const;

	// Getters
	float2 GetMousePosOnScene() const;
	float2 GetSceneWindowSize() const;
	const char* GetCurrentShadingMode() const;

private:
	float2 framebufferSize = {0.0f, 0.0f};
	float2 mousePosOnScene = {0.0f, 0.0f};

	const char* currentShadingMode = "Shaded";

	bool view2D = false;
};
