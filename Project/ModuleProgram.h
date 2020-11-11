#pragma once

#include "Module.h"

class ModuleProgram : public Module
{
public:
	ModuleProgram();
	~ModuleProgram();

	bool Init() override;
	bool CleanUp() override;

public:
	unsigned program = 0;
};
