#include "ModuleFiles.h"

#include "Globals.h"
#include "Utils/Logging.h"

#include "physfs.h"

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
