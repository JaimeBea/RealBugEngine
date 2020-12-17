#pragma once

#include "Module.h"

class ModuleFiles : public Module
{
public:
	char* Load(const char* file_path) const;
	bool Save(const char* file_path, char* buffer, size_t size, bool append = false) const;
};

