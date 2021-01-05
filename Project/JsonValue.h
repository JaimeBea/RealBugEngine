#pragma once

#include "rapidjson/document.h"
#include "string"

class JsonValue
{
public:
	JsonValue(rapidjson::Document& document, rapidjson::Value& value);

	// Size of the array. Only works if the value is an array.
	size_t Size() const;

	// Non-const object/array access (Creates new members if they don't exist)
	template<typename T> JsonValue operator[](T* key);
	JsonValue operator[](unsigned index);

	// Const object/array access (Can't create new members if they don't exist)
	template<typename T> const JsonValue operator[](T* key) const;
	const JsonValue operator[](unsigned index) const;

	// Assignment operators to modify the value
	void operator=(bool x);
	void operator=(int x);
	void operator=(unsigned x);
	void operator=(long long x);
	void operator=(unsigned long long x);
	void operator=(float x);
	void operator=(double x);
	void operator=(const char* x);

	// Conversion operators for easy access
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

// Template is necessary to disambiguate with 'operator[](int index)'
template<typename T>
inline JsonValue JsonValue::operator[](T* key)
{
	if (!value.IsObject())
	{
		value.SetObject();
	}

	if (!value.HasMember(key))
	{
		value.AddMember(rapidjson::StringRef(key), rapidjson::Value(), document.GetAllocator());
	}

	return JsonValue(document, value[key]);
}

// Template is necessary to disambiguate with 'operator[](int index)'
template<typename T>
inline const JsonValue JsonValue::operator[](T* key) const
{
	assert(value.IsObject());
	assert(value.HasMember(key));

	return JsonValue(document, value[key]);
}
