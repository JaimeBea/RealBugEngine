#pragma once

#include "Module.h"
#include "Math/float3.h"

class ModuleRender : public Module
{
public:
	ModuleRender();
	~ModuleRender();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();
	void WindowResized(int width, int height);

	void SetVSync(bool vsync);

public:
	void* context;
	float3 clear_color = { 0.1f, 0.1f, 0.1f };
};
