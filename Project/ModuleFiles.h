#pragma once

#include "Module.h"
#include "Buffer.h"

class ModuleFiles : public Module
{
public:
	Buffer<char> Load(const char* file_path) const;
	bool Save(const char* file_path, const Buffer<char>& buffer, bool append = false) const;

	const char* GetFileExtension(const char* file_path) const;
};
