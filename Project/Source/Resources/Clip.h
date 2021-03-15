#pragma once

#include <string>

class ResourceAnimation;

class Clip {
public:
	std::string name;
	ResourceAnimation* animation;
	bool loop;
	unsigned int beginIndex;
	unsigned int endIndex;
};
