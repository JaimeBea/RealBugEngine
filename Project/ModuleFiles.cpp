#include "ModuleFiles.h"

#include "Globals.h"
#include "Logging.h"

#include "Math/MathFunc.h"
#include <string.h>

#include "Leaks.h"

Buffer<char> ModuleFiles::Load(const char* file_path) const
{
	Buffer<char> buffer = Buffer<char>();

	FILE* file = fopen(file_path, "rb");
	if (!file)
	{
		LOG("Error loading file %s (%s).\n", file_path, strerror(errno));
		return buffer;
	}
	DEFER
	{
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

bool ModuleFiles::Save(const char* file_path, const Buffer<char>& buffer, bool append) const
{
	FILE* file = fopen(file_path, append ? "ab" : "wb");
	if (!file)
	{
		LOG("Error saving file %s (%s).\n", file_path, strerror(errno));
		return nullptr;
	}
	DEFER
	{
		fclose(file);
	};

	fwrite(buffer.Data(), sizeof(char), buffer.Size(), file);

	return true;
}

std::string ModuleFiles::GetFileNameAndExtension(const char* file_path) const
{
	const char* last_slash = strrchr(file_path, '/');
	const char* last_backslash = strrchr(file_path, '\\');
	const char* last_separator = Max(last_slash, last_backslash);

	if (last_separator == nullptr)
	{
		return file_path;
	}

	const char* file_name_and_extension = last_separator + 1;
	return file_name_and_extension;
}

std::string ModuleFiles::GetFileName(const char* file_path) const
{
	const char* last_slash = strrchr(file_path, '/');
	const char* last_backslash = strrchr(file_path, '\\');
	const char* last_separator = Max(last_slash, last_backslash);

	if (last_separator == nullptr)
	{
		return file_path;
	}

	const char* file_name = last_separator + 1;
	const char* last_dot = strrchr(file_name, '.');

	if (last_dot == nullptr || last_dot == file_name)
	{
		return file_name;
	}

	return std::string(file_name).substr(0, last_dot - file_name);
}

std::string ModuleFiles::GetFileExtension(const char* file_path) const
{
	const char* last_slash = strrchr(file_path, '/');
	const char* last_backslash = strrchr(file_path, '\\');
	const char* last_separator = Max(last_slash, last_backslash);
	const char* last_dot = strrchr(file_path, '.');

	if (last_dot == nullptr || last_dot == file_path || last_dot < last_separator || last_dot == last_separator + 1)
	{
		return std::string();
	}
	
	return last_dot;
}
