#include "OutputStream.h"

OutputStream::OutputStream(const char* file_path, bool append)
{
	file = fopen(file_path, append ? "ab" : "wb");
}

OutputStream::~OutputStream()
{
	fclose(file);
}
