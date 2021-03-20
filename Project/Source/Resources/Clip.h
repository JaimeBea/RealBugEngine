#pragma once
#include "ResourceAnimation.h"
#include <string>

class ResourceAnimation;

class Clip {
public:
	Clip(std::string& mName, ResourceAnimation* mAnimation = nullptr , bool mLoop = false)
		: name(mName)
		, animation(mAnimation)
		, loop(mLoop)
	{
		if (mAnimation != nullptr) {
			beginIndex = 0;
			keyFramesSize = mAnimation->keyFrames.size();
			endIndex =  keyFramesSize;
			duration = mAnimation->duration;
		}
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
