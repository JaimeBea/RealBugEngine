#pragma once

#include "Module.h"
#include "Model.h"

#include <vector>

class ModuleModels : public Module
{
public:
	unsigned LoadModel(const char* file_name);
	void ReleaseModel(unsigned model);

public:
	std::vector<Model> models;
};

