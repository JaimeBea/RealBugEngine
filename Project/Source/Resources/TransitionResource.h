#pragma once
#include <string>

class ResourceState;
class TransitionResource {

public:
	ResourceState* source;
	ResourceState* target;
	std::string trigger;
	unsigned int interpolationDuration;
};
