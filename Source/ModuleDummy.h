#pragma once

#include "Module.h"

class ModuleDummy : public Module
{
	bool Init() override
	{
		LOG("Dummy Init!");
		return true;
	}

	bool CleanUp() override
	{
		LOG("Dummy CleanUp!");
		return true;
	}
};