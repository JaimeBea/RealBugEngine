#pragma once

#include "Module.h"
#include "Buffer.h"

#include <string>
#include <vector>

class ModuleFiles : public Module
{
public:
	Buffer<char> Load(const char* file_path) const;
	bool Save(const char* file_path, const Buffer<char>& buffer, bool append = false) const;
	bool Save(const char* file_path, const char* buffer, size_t size, bool append = false) const;

	void CreateFolder(const char* folder_path) const;
	void EraseFolder(const char* folder_path) const;
	void EraseFile(const char* file_path) const;

	std::vector<std::string> GetFilesInFolder(const char* folder_path) const;

	std::string GetFileNameAndExtension(const char* file_path) const;
	std::string GetFileName(const char* file_path) const;
	std::string GetFileExtension(const char* file_path) const;
	std::string GetFileFolder(const char* file_path) const;
};
