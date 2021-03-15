#pragma once
#include <string>

class ResourceState;
class ResourceTransition {

public:
	ResourceState* source;
	ResourceState* target;
	std::string trigger;
	unsigned int interpolationDuration;
};
