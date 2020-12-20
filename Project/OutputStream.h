#pragma once

#include <stdio.h>

class OutputStream
{
public:
	OutputStream(const char* file_path, bool append = false);
	~OutputStream();

	template <typename T>
	void Write(T element) const;

private:
	FILE* file = nullptr;
};

template<typename T>
inline void OutputStream::Write(T element) const
{
	fwrite(&element, sizeof(T), 1, file);
}
