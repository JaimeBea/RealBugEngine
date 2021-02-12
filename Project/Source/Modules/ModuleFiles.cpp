#include "ModuleFiles.h"

#include "Globals.h"
#include "Utils/Logging.h"

#include "Math/MathFunc.h"
#include <string.h>
#include <windows.h>
#include <algorithm>

#include "Utils/Leaks.h"

Buffer<char> ModuleFiles::Load(const char* filePath) const {
	Buffer<char> buffer = Buffer<char>();

	FILE* file = fopen(filePath, "rb");
	if (!file) {
		LOG("Error loading file %s (%s).\n", filePath, strerror(errno));
		return buffer;
	}
	DEFER {
		fclose(file);
	};

	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	rewind(file);

	buffer.Allocate(size + 1);
	char* data = buffer.Data();
	fread(data, sizeof(char), size, file);
	data[size] = '\0';

	return buffer;
}

bool ModuleFiles::Save(const char* filePath, const Buffer<char>& buffer, bool append) const {
	return Save(filePath, buffer.Data(), buffer.Size(), append);
}

bool ModuleFiles::Save(const char* filePath, const char* buffer, size_t size, bool append) const {
	FILE* file = fopen(filePath, append ? "ab" : "wb");
	if (!file) {
		LOG("Error saving file %s (%s).\n", filePath, strerror(errno));
		return nullptr;
	}
	DEFER {
		fclose(file);
	};

	fwrite(buffer, sizeof(char), size, file);

	return true;
}

void ModuleFiles::CreateFolder(const char* folderPath) const {
	CreateDirectory(folderPath, nullptr);
}

void ModuleFiles::EraseFolder(const char* folderPath) const {
	RemoveDirectory(folderPath);
}

void ModuleFiles::EraseFile(const char* filePath) const {
	remove(filePath);
}

std::vector<std::string> ModuleFiles::GetFilesInFolder(const char* folderPath) const {
	std::string folderPathEx = std::string(folderPath) + "\\*";
	std::vector<std::string> filePaths;
	WIN32_FIND_DATA data;
	HANDLE handle = FindFirstFile(folderPathEx.c_str(), &data);
	if (handle == INVALID_HANDLE_VALUE) return filePaths;
	unsigned i = 0;
	do {
		if (i >= 2) // Ignore '.' and '..'
		{
			filePaths.push_back(data.cFileName);
		}
		i++;
	} while (FindNextFile(handle, &data));
	FindClose(handle);
	return filePaths;
}

std::string ModuleFiles::GetFileNameAndExtension(const char* filePath) const {
	const char* lastSlash = strrchr(filePath, '/');
	const char* lastBackslash = strrchr(filePath, '\\');
	const char* lastSeparator = Max(lastSlash, lastBackslash);

	if (lastSeparator == nullptr) {
		return filePath;
	}

	const char* fileNameAndExtension = lastSeparator + 1;
	return fileNameAndExtension;
}

std::string ModuleFiles::GetFileName(const char* filePath) const {
	const char* lastSlash = strrchr(filePath, '/');
	const char* lastBackslash = strrchr(filePath, '\\');
	const char* lastSeparator = Max(lastSlash, lastBackslash);

	const char* fileName = lastSeparator == nullptr ? filePath : lastSeparator + 1;
	const char* lastDot = strrchr(fileName, '.');

	if (lastDot == nullptr || lastDot == fileName) {
		return fileName;
	}

	return std::string(fileName).substr(0, lastDot - fileName);
}

std::string ModuleFiles::GetFileExtension(const char* filePath) const {
	const char* lastSlash = strrchr(filePath, '/');
	const char* lastBackslash = strrchr(filePath, '\\');
	const char* lastSeparator = Max(lastSlash, lastBackslash);
	const char* lastDot = strrchr(filePath, '.');

	if (lastDot == nullptr || lastDot == filePath || lastDot < lastSeparator || lastDot == lastSeparator + 1) {
		return std::string();
	}

	std::string extension = std::string(lastDot);
	std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
	return extension;
}

std::string ModuleFiles::GetFileFolder(const char* filePath) const {
	const char* lastSlash = strrchr(filePath, '/');
	const char* lastBackslash = strrchr(filePath, '\\');
	const char* lastSeparator = Max(lastSlash, lastBackslash);

	if (lastSeparator == nullptr) {
		return std::string();
	}

	return std::string(filePath).substr(0, lastSeparator - filePath);
}
