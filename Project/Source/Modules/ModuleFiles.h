#pragma once

#include "Module.h"
#include "Utils/Buffer.h"
#include "Utils/UID.h"

#include <string>
#include <vector>

class ModuleFiles : public Module {
public:
	bool Init() override;
	bool CleanUp() override;

	Buffer<char> Load(const char* filePath) const;
	bool Save(const char* filePath, const Buffer<char>& buffer, bool append = false) const;
	bool Save(const char* filePath, const char* buffer, size_t size, bool append = false) const;
	bool SaveById(UID id, const char* buffer, size_t size) const;
	bool SaveById(UID id, const Buffer<char>& buffer) const;
	void CreateFolder(const char* folderPath) const;
	void Copy(const char* path, const char* newPath);
	void Erase(const char* path) const;

	bool Exists(const char* filePath) const;
	bool IsDirectory(const char* path) const;
	std::vector<std::string> GetDrives() const;

	long long GetLocalFileModificationTime(const char* path) const;

	std::vector<std::string> GetFilesInFolder(const char* folderPath) const;
	std::vector<std::string> GetFilesInLocalFolder(const char* folderPath) const;
	std::string GetFileNameAndExtension(const char* filePath) const;
	std::string GetFileName(const char* filePath) const;
	std::string GetFileExtension(const char* filePath) const;
	std::string GetFileFolder(const char* filePath) const;
	std::string GetAbsolutePath(const char* filePath) const;

private:
	std::string GenerateMetaPath(UID id) const;
};
