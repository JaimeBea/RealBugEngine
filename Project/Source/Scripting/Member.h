#pragma once

#include <variant>
#include <string>

#define MEMBER_VARIANT std::variant<bool, char, unsigned char, short, unsigned short, int, unsigned int, long long, unsigned long long, float, double, std::string>

/* Creating a new member type:
*    1. Add a new MemberType for the new member
*    2. Add the new member to the GetMemberTypeName and GetMemberTypeFromName functions in Member.cpp
*    3. Add the member editor functionality in ComponentScript.cpp
*/

enum class MemberType {
	UNKNOWN,

	// Numeric types
	BOOL,
	INT,
	UINT,
	LONGLONG,
	ULONGLONG,
	FLOAT,
	DOUBLE,

	// String
	STRING,

	// GameObject UID
	GAME_OBJECT_UID,

	// Resources
	PREFAB_RESOURCE_UID
};

struct Member {
	Member(MemberType type_, std::string name_, size_t offset_)
		: type(type_)
		, name(name_)
		, offset(offset_) {}

	MemberType type;
	std::string name;
	size_t offset;
};

const char* GetMemberTypeName(MemberType type);
MemberType GetMemberTypeFromName(const char* name);