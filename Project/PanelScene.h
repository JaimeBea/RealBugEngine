#pragma once

#include "Panel.h"

#include "Math/float2.h"

struct ImDrawList;

class PanelScene : public Panel
{
public:
	PanelScene();

	void Update() override;

private:
	float2 framebuffer_position = {0.0f, 0.0f};
	float2 framebuffer_size = {0.0f, 0.0f};
};
