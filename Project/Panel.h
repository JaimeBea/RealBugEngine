#pragma once

#include "imgui.h"

static ImVec4 title_color = ImVec4(0.35f, 0.69f, 0.87f, 1.0f);
static ImVec4 text_color = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
static float drag_speed1f = 0.5f;
static float drag_speed2f = 0.05f;
static float drag_speed3f = 0.005f;

class Panel
{
public:
	Panel(const char* name, bool enabled);

	virtual void Update();
	virtual void HelpMarker(const char* desc);

public:
	const char* name = "";
	bool enabled = true;
};

