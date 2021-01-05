#include "JsonValue.h"

JsonValue::JsonValue(rapidjson::Document& document_, rapidjson::Value& value_)
	: document(document_)
	, value(value_) {}

size_t JsonValue::Size() const
{
	assert(value.IsArray());
	return value.Size();
}

JsonValue JsonValue::operator[](const char* key)
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

JsonValue JsonValue::operator[](unsigned index)
{
	if (!value.IsArray())
	{
		value.SetArray();
	}

	assert(index <= value.Size());

	if (index == value.Size())
	{
		value.PushBack(rapidjson::Value(), document.GetAllocator());
	}

	return JsonValue(document, value[index]);
}

void JsonValue::operator=(bool x)
{
	value = x;
}

void JsonValue::operator=(int x)
{
	value = x;
}

void JsonValue::operator=(unsigned x)
{
	value = x;
}

void JsonValue::operator=(long long x)
{
	value = x;
}

void JsonValue::operator=(unsigned long long x)
{
	value = x;
}

void JsonValue::operator=(float x)
{
	value = x;
}

void JsonValue::operator=(double x)
{
	value = x;
}

void JsonValue::operator=(const char* x)
{
	value.SetString(x, document.GetAllocator());
}

JsonValue::operator bool() const
{
	assert(value.IsBool());
	return value.GetBool();
}

JsonValue::operator int() const
{
	assert(value.IsInt());
	return value.GetInt();
}

JsonValue::operator unsigned() const
{
	assert(value.IsUint());
	return value.GetUint();
}

JsonValue::operator long long() const
{
	assert(value.IsInt64());
	return value.GetInt64();
}

JsonValue::operator unsigned long long() const
{
	assert(value.IsUint64());
	return value.GetUint64();
}

JsonValue::operator float() const
{
	assert(value.IsFloat());
	return value.GetFloat();
}

JsonValue::operator double() const
{
	assert(value.IsDouble());
	return value.GetDouble();
}

JsonValue::operator std::string() const
{
	assert(value.IsString());
	return value.GetString();
}
