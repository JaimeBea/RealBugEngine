#pragma once

#include "Module.h"

class ModulePrograms : public Module
{
public:
	bool Start() override;
	bool CleanUp() override;

public:
	unsigned default_program = 0;
	unsigned phong_program = 0;
	unsigned skybox_program = 0;
};
