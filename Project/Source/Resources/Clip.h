#pragma once

#include <string>

class ResourceAnimation;

class Clip {
public:
	Clip(std::string& mName, ResourceAnimation* mAnimation = nullptr)
		: name(mName)
		, animation(mAnimation)
	{

	}
	std::string name;
	ResourceAnimation* animation;
	bool loop = false;
	unsigned int beginIndex = 0;
	unsigned int endIndex = 0;

	void setBeginIndex(unsigned int index) {
		beginIndex = index;
	}

	void setEndIndex(unsigned int index) {
		endIndex = index;
	}

};
