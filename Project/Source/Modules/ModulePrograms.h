#pragma once

#include "Module.h"

class ModulePrograms : public Module {
public:
	unsigned CreateProgram(const char* vertexShaderFilePath, const char* fragmentShaderFilePath);
	bool Start() override;
	bool CleanUp() override;

	unsigned CreateProgram(const char* ShaderFilePath);
	void DeleteProgram(unsigned int IdProgram);
};
