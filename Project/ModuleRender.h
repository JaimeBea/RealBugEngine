#pragma once

#include "Module.h"

#include "Model.h"

#include "Math/float3.h"

class ModuleRender : public Module
{
public:
	bool Init() override;
	bool PostInit() override;
	UpdateStatus PreUpdate() override;
	UpdateStatus Update() override;
	UpdateStatus PostUpdate() override;
	bool CleanUp() override;

	void WindowResized(int width, int height);

	void SetVSync(bool vsync);

public:
	void* context = nullptr;
	float3 clear_color = { 0.1f, 0.1f, 0.1f };

private:
	unsigned house_model;
};
