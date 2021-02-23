#pragma once

#include "Module.h"
#include "Utils/Buffer.h"

#include <string>
#include <vector>
constexpr char* extensions[] = {".jpg", ".png", ".dds"};
enum class AllowedExtensionsFlag {
	ALL = 0,
	JPG = 1,
	PNG = 1 << 1,
	DDS = 1 << 2
};
class ModuleFiles : public Module {

public:
	bool Init() override;
	bool CleanUp() override;

	Buffer<char> Load(const char* filePath) const;
	bool Save(const char* filePath, const Buffer<char>& buffer, bool append = false) const;
	bool Save(const char* filePath, const char* buffer, size_t size, bool append = false) const;

	void CreateFolder(const char* folderPath) const;
	void Erase(const char* path) const;

	bool Exists(const char* filePath) const;
	bool IsDirectory(const char* path) const;
	std::vector<std::string> GetDrives() const;

	std::vector<std::string> GetFilesInFolder(const char* folderPath, AllowedExtensionsFlag extFilter = AllowedExtensionsFlag::ALL) const;
	std::string GetFileNameAndExtension(const char* filePath) const;
	std::string GetFileName(const char* filePath) const;
	std::string GetFileExtension(const char* filePath) const;
	std::string GetFileFolder(const char* filePath) const;
	std::string GetAbsolutePath(const char* filePath) const;
	std::string UpOneLevel(const char* filePath) const;

private:
	std::vector<std::string> GetFileExtensions(AllowedExtensionsFlag ext) const;
};
