#include "InputStream.h"

#include "Logging.h"

#include "Leaks.h"

InputStream::InputStream(const char* file_path)
{
	file = fopen(file_path, "rb");
	if (!file)
	{
		LOG("Error loading file %s (%s).\n", file_path, strerror(errno));
	}
}

InputStream::~InputStream()
{
	if (!file) return;

	fclose(file);
}
