#pragma once

#include "ModuleRender.h"

class ModuleRenderExercise : public ModuleRender
{
public:
	bool Init() override;
	UpdateStatus PreUpdate() override;
	UpdateStatus Update() override;
	UpdateStatus PostUpdate() override;
	bool CleanUp() override;

private:
	unsigned lenna_vbo = 0;
};
