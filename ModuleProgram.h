#pragma once

#include "Module.h"
#include <vector>

class ModuleProgram : public Module
{
public:
	ModuleProgram();
	~ModuleProgram();

	unsigned CreateProgram(const char* vertex_shader_file, const char* fragment_shader_file);

	bool CleanUp();

private:
	std::vector<unsigned> programs;
};
