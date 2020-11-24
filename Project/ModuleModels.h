#pragma once

#include "Module.h"
#include "Model.h"

#include "assimp/cimport.h"

#include "Pool.h"

class ModuleModels : public Module
{
public:
	bool Init() override;
	bool CleanUp() override;

	Model* LoadModel(const char* file_name);
	void ReleaseModel(Model* model);

public:
	Pool<Model> models;

private:
	aiLogStream log_stream;
};

