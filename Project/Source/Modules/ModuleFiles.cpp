#include "ModuleFiles.h"

#include "Globals.h"
#include "Utils/Logging.h"

#include "Math/MathFunc.h"
#include <string.h>
#include <windows.h>
#include <algorithm>

#include "Utils/Leaks.h"

Buffer<char> ModuleFiles::Load(const char* file_path) const {
	Buffer<char> buffer = Buffer<char>();

	FILE* file = fopen(file_path, "rb");
	if (!file) {
		LOG("Error loading file %s (%s).\n", file_path, strerror(errno));
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

bool ModuleFiles::Save(const char* file_path, const Buffer<char>& buffer, bool append) const {
	return Save(file_path, buffer.Data(), buffer.Size(), append);
}

bool ModuleFiles::Save(const char* file_path, const char* buffer, size_t size, bool append) const {
	FILE* file = fopen(file_path, append ? "ab" : "wb");
	if (!file) {
		LOG("Error saving file %s (%s).\n", file_path, strerror(errno));
		return nullptr;
	}
	DEFER {
		fclose(file);
	};

	fwrite(buffer, sizeof(char), size, file);

	return true;
}

void ModuleFiles::CreateFolder(const char* folder_path) const {
	CreateDirectory(folder_path, nullptr);
}

void ModuleFiles::EraseFolder(const char* folder_path) const {
	RemoveDirectory(folder_path);
}

void ModuleFiles::EraseFile(const char* file_path) const {
	remove(file_path);
}

std::vector<std::string> ModuleFiles::GetFilesInFolder(const char* folder_path) const {
	std::string folder_path_ex = std::string(folder_path) + "\\*";
	std::vector<std::string> file_paths;
	WIN32_FIND_DATA data;
	HANDLE handle = FindFirstFile(folder_path_ex.c_str(), &data);
	if (handle == INVALID_HANDLE_VALUE) return file_paths;
	unsigned i = 0;
	do {
		if (i >= 2) // Ignore '.' and '..'
		{
			file_paths.push_back(data.cFileName);
		}
		i++;
	} while (FindNextFile(handle, &data));
	FindClose(handle);
	return file_paths;
}

std::string ModuleFiles::GetFileNameAndExtension(const char* file_path) const {
	const char* last_slash = strrchr(file_path, '/');
	const char* last_backslash = strrchr(file_path, '\\');
	const char* last_separator = Max(last_slash, last_backslash);

	if (last_separator == nullptr) {
		return file_path;
	}

	const char* file_name_and_extension = last_separator + 1;
	return file_name_and_extension;
}

std::string ModuleFiles::GetFileName(const char* file_path) const {
	const char* last_slash = strrchr(file_path, '/');
	const char* last_backslash = strrchr(file_path, '\\');
	const char* last_separator = Max(last_slash, last_backslash);

	const char* file_name = last_separator == nullptr ? file_path : last_separator + 1;
	const char* last_dot = strrchr(file_name, '.');

	if (last_dot == nullptr || last_dot == file_name) {
		return file_name;
	}

	return std::string(file_name).substr(0, last_dot - file_name);
}

std::string ModuleFiles::GetFileExtension(const char* file_path) const {
	const char* last_slash = strrchr(file_path, '/');
	const char* last_backslash = strrchr(file_path, '\\');
	const char* last_separator = Max(last_slash, last_backslash);
	const char* last_dot = strrchr(file_path, '.');

	if (last_dot == nullptr || last_dot == file_path || last_dot < last_separator || last_dot == last_separator + 1) {
		return std::string();
	}

	std::string extension = std::string(last_dot);
	std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
	return extension;
}

std::string ModuleFiles::GetFileFolder(const char* file_path) const {
	const char* last_slash = strrchr(file_path, '/');
	const char* last_backslash = strrchr(file_path, '\\');
	const char* last_separator = Max(last_slash, last_backslash);

	if (last_separator == nullptr) {
		return std::string();
	}

	return std::string(file_path).substr(0, last_separator - file_path);
}
