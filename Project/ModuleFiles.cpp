#include "ModuleFiles.h"

#include "Globals.h"
#include "Logging.h"
#include <string.h>

#include "Leaks.h"

char* ModuleFiles::Load(const char* file_path) const
{
	FILE* file = fopen(file_path, "rb");
	if (!file)
	{
		LOG("Error loading file %s (%s).\n", file_path, strerror(errno));
		return nullptr;
	}
	DEFER{ fclose(file); };

	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	rewind(file);

	char* data = new char[size + 1];
	fread(data, 1, size, file);
	data[size] = '\0';

	return data;
}

bool ModuleFiles::Save(const char* file_path, char* buffer, size_t size, bool append) const
{
	FILE* file = fopen(file_path, append ? "ab" : "wb");
	if (!file)
	{
		LOG("Error saving file %s (%s).\n", file_path, strerror(errno));
		return nullptr;
	}
	DEFER{ fclose(file); };

	fwrite(buffer, 1, size, file);

	return true;
}
