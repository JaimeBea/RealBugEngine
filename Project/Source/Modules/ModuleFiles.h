#pragma once

#include "Module.h"
#include "Utils/Buffer.h"

#include <string>
#include <vector>

class ModuleFiles : public Module {
public:
	bool Init() override;
	bool CleanUp() override;

	Buffer<char> Load(const char* filePath) const;
	bool Save(const char* filePath, const Buffer<char>& buffer, bool append = false) const;
	bool Save(const char* filePath, const char* buffer, size_t size, bool append = false) const;

	void CreateFolder(const char* folderPath) const;
	void EraseFolder(const char* folderPath) const;
	void EraseFile(const char* filePath) const;

	bool Exist(const char* filePath) const;

	std::vector<std::string> GetFilesInFolder(const char* folderPath) const;
	std::string GetFileNameAndExtension(const char* filePath) const;
	std::string GetFileName(const char* filePath) const;
	std::string GetFileExtension(const char* filePath) const;
	std::string GetFileFolder(const char* filePath) const;
};
