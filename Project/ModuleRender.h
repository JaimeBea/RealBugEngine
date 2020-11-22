#pragma once

#include "Module.h"

#include "Model.h"

#include "Math/float3.h"

class ModuleRender : public Module
{
public:
	bool Init() override;
	bool Start() override;
	UpdateStatus PreUpdate() override;
	UpdateStatus Update() override;
	UpdateStatus PostUpdate() override;
	bool CleanUp() override;

	void ViewportResized(int width, int height);

	void SetVSync(bool vsync);

public:
	void* context = nullptr;
	unsigned render_texture = 0;
	unsigned depth_renderbuffer = 0;
	unsigned framebuffer = 0;

	unsigned viewport_width = 0;
	unsigned viewport_height = 0;

	float3 clear_color = { 0.1f, 0.1f, 0.1f };

private:
	unsigned current_model;
};
