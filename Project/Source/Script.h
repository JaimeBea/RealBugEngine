#pragma once

#include "Utils/Creator.h"

#define GENERATE_BODY(classname) \
private:                         \
	static const CreatorImplementation<classname> creator;

#define GENERATE_BODY_IMPL(classname) \
	const CreatorImplementation<classname> classname::creator(#classname);

class Script {
public:
	virtual ~Script() = default;
	virtual void Update() = 0;
	virtual void Start() = 0;
};
