#pragma once

#include "Module.h"

class ModuleTextures : public Module
{
public:
	ModuleTextures();
	~ModuleTextures();

	bool PostInit() override;
	bool CleanUp() override;

public:
	unsigned lenna = 0;
};

