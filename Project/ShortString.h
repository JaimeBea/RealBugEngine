#pragma once

template<size_t S>
class ShortString
{
public:
	ShortString() : chars{ '\0' }, length(0) {}

	ShortString(const char* source_chars)
	{
		char source_char = source_chars[0];
		int size = 0;
		while (source_char != '\0' && size < S - 1)
		{
			chars[size] = source_char;
			size += 1;
			source_char = source_chars[size];
		}
		chars[size] = '\0';
		length = size;
	}

	ShortString(const ShortString& other) : ShortString(other.chars) {}

	bool operator==(const ShortString& other) const
	{
		if (length != other.length) return false;

		for (int i = 0; i < length; ++i)
		{
			if (chars[i] != other.chars[i]) return false;
		}

		return true;
	}

	ShortString& operator=(const char* other)
	{
		char source_char = source_chars[0];
		int size = 0;
		while (source_char != '\0' && size < S - 1)
		{
			chars[size] = source_char;
			size += 1;
			source_char = source_chars[size];
		}
		chars[size] = '\0';
		length = size;
		return *this;
	}

	void Clear()
	{
		chars = '\0';
		length = 0;
	}

	size_t Length() const
	{
		return length;
	}

	char* GetChars() const
	{
		return (char*) chars;
	}

	size_t GetMaxSize() const
	{
		return S;
	}

private:
	size_t length;
	char chars[S];
};
