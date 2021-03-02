#pragma once

#include "Module.h"

class ModulePrograms : public Module {
public:
	bool Start() override;
	bool CleanUp() override;

public:
	unsigned defaultProgram = 0;
	unsigned standardProgram = 0;
	unsigned standardSpecularProgram = 0;
	unsigned skyboxProgram = 0;
};
