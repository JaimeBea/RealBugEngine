#pragma once

#include "Module.h"
#include "Buffer.h"
#include "UID.h"

#include <unordered_map>
#include <string>

class ModuleFiles : public Module
{
public:
	Buffer<char> Load(const char* file_path) const;
	bool Save(const char* file_path, const Buffer<char>& buffer, bool append = false) const;

	UID CreateUIDForFileName(const char* file_name);
	std::string GetFileNameFromUID(UID id) const;

	std::string GetFileNameAndExtension(const char* file_path) const;
	std::string GetFileName(const char* file_path) const;
	std::string GetFileExtension(const char* file_path) const;

private:
	std::unordered_map<UID, std::string> id_to_file_name_map = std::unordered_map<UID, std::string>();
};
