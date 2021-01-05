#pragma once

#include "rapidjson/document.h"
#include "string"

class JsonValue
{
public:
	JsonValue(rapidjson::Document& document, rapidjson::Value& value);

	size_t Size() const;

	JsonValue operator[](const char* key);
	JsonValue operator[](unsigned index);

	void operator=(bool x);
	void operator=(int x);
	void operator=(unsigned x);
	void operator=(long long x);
	void operator=(unsigned long long x);
	void operator=(float x);
	void operator=(double x);
	void operator=(const char* x);

	operator bool() const;
	operator int() const;
	operator unsigned() const;
	operator long long() const;
	operator unsigned long long() const;
	operator float() const;
	operator double() const;
	operator std::string() const;

private:
	rapidjson::Document& document;
	rapidjson::Value& value;
};