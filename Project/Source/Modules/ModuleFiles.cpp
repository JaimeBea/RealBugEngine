#include "ModuleFiles.h"

#include "Globals.h"
#include "Utils/Logging.h"

#include "physfs.h"
#include "Math/MathFunc.h"
#include <string.h>
#include <windows.h>
#include <algorithm>

#include "Utils/Leaks.h"

bool ModuleFiles::Init() {
	PHYSFS_init(nullptr);
	PHYSFS_mount(".", nullptr, 0);
	PHYSFS_setWriteDir(".");

	return true;
}

bool ModuleFiles::CleanUp() {
	PHYSFS_deinit();

	return true;
}

Buffer<char> ModuleFiles::Load(const char* filePath) const {
	Buffer<char> buffer = Buffer<char>();

	PHYSFS_File* file = PHYSFS_openRead(filePath);
	if (!file) {
		LOG("Error opening file %s (%s).\n", filePath, PHYSFS_getLastError());
		return buffer;
	}
	DEFER {
		PHYSFS_close(file);
	};

	PHYSFS_sint64 size = PHYSFS_fileLength(file);
	if (size < 0) {
		LOG("File size couldn't be determined for %s (%s).\n", filePath, PHYSFS_getLastError());
		return buffer;
	}

	buffer.Allocate(size + 1);
	char* data = buffer.Data();
	PHYSFS_sint64 numBytes = PHYSFS_readBytes(file, data, size);
	if (numBytes < size) {
		LOG("Error reading file %s (%s).\n", filePath, PHYSFS_getLastError());
		return buffer;
	}
	data[size] = '\0';

	return buffer;
}

bool ModuleFiles::Save(const char* filePath, const Buffer<char>& buffer, bool append) const {
	return Save(filePath, buffer.Data(), buffer.Size(), append);
}

bool ModuleFiles::Save(const char* filePath, const char* buffer, size_t size, bool append) const {
	PHYSFS_File* file = append ? PHYSFS_openAppend(filePath) : PHYSFS_openWrite(filePath);
	if (!file) {
		LOG("Error saving file %s (%s).\n", filePath, strerror(errno));
		return false;
	}
	DEFER {
		PHYSFS_close(file);
	};

	PHYSFS_sint64 numBytes = PHYSFS_writeBytes(file, buffer, size);
	if (numBytes < size) {
		LOG("Error writing to file %s (%s).\n", filePath, PHYSFS_getLastError());
		return false;
	}

	return true;
}

void ModuleFiles::CreateFolder(const char* folderPath) const {
	if (!PHYSFS_mkdir(folderPath)) LOG(PHYSFS_getLastError());
}

void ModuleFiles::Erase(const char* path) const {
	if (!PHYSFS_delete(path)) {
		LOG(PHYSFS_getLastError());
	}
}

bool ModuleFiles::Exists(const char* path) const {
	return PHYSFS_exists(path);
}

bool ModuleFiles::IsDirectory(const char* path) const {

	DWORD ftyp = GetFileAttributesA(path);
	return ftyp & FILE_ATTRIBUTE_DIRECTORY;
}

std::vector<std::string> ModuleFiles::GetDrives() const {
	DWORD mask = GetLogicalDrives();
	std::vector<std::string> drives;
	for (int i = 0; i < 26; ++i) {
		if (!(mask & (1 << i))) continue;

		char letter = 'A' + i;
		std::string drive = std::string() + letter + ":";
		drives.push_back(drive);
	}

	return drives;
}

std::vector<std::string> ModuleFiles::GetFilesInFolder(const char* folderPath) const {
	std::string folderPathEx = std::string(folderPath) + "\\*";
	std::vector<std::string> filePaths;
	WIN32_FIND_DATA data;
	HANDLE handle = FindFirstFile(folderPathEx.c_str(), &data);
	if (handle == INVALID_HANDLE_VALUE) return filePaths;
	unsigned i = 0;
	do {
		if (i >= 1) // Ignore '.'
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

std::string ModuleFiles::GetAbsolutePath(const char* filePath) const {
	char buff[FILENAME_MAX];
	GetModuleFileName(NULL, buff, FILENAME_MAX);
	return std::string(buff) + filePath;
}
