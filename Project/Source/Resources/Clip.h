#pragma once
#include "ResourceAnimation.h"
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
	float duration = 0;
	unsigned int keyFramesSize = 0;

	void setBeginIndex(unsigned int index) {
		if (endIndex >= index && animation) {
			beginIndex = index;
			keyFramesSize = endIndex - beginIndex;
			duration = keyFramesSize * animation->duration / animation->keyFrames.size();
		}
	}

	void setEndIndex(unsigned int index) {
		if (index >= beginIndex && animation) {
			endIndex = index;
			keyFramesSize = endIndex - beginIndex;
			duration = keyFramesSize * animation->duration / animation->keyFrames.size();
		}
	}

};
