#pragma once

#include <stdio.h>

class InputStream
{
public:
	InputStream(const char* file_path);
	~InputStream();

	template <typename T>
	T Read() const;

private:
	FILE* file = nullptr;
};

template<typename T>
inline T InputStream::Read() const
{
	T element;
	fread(&element, sizeof(T), 1, file);
	return element;
}
