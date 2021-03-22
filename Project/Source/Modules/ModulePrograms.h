#pragma once

#include "Module.h"

class ModulePrograms : public Module {
public:
	bool Start() override;
	bool CleanUp() override;

	unsigned CreateProgram(const char* ShaderFilePath);
	void DeleteProgram(unsigned int IdProgram);
};
