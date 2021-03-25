#pragma once

#include "Module.h"

#include <string>

class ModuleProject : public Module {
public:
	
	bool Init() override;

	void CreateNewProject(const char* name, const char* path);
	void LoadProject(const char* path);
	void SaveProject();

private:
	std::string fileName = "";
};
