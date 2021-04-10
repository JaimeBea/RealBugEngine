#pragma once
#include "ResourceAnimation.h"

#include "Application.h"
#include "Modules/ModuleResources.h"

#include "Utils/UID.h"
#include <string>

class Clip {
public:
	Clip(std::string& mName, UID mAnimationUID = 0, unsigned int mBeginIndex = 0, unsigned int mEndIndex = 0, bool mLoop = false, UID mid = 0)
		: name(mName)
		, animationUID(mAnimationUID)
		, loop(mLoop)
	{
		if (animationUID != 0) {
			id = mid != 0 ? mid :GenerateUID();
			ResourceAnimation* animationResource = GetResourceAnimation();
			App->resources->IncreaseReferenceCount(mAnimationUID);
			setEndIndex(mEndIndex);
			setBeginIndex(mBeginIndex);
		}
	}	

	void setBeginIndex(unsigned int index) {
		ResourceAnimation* animationResource = GetResourceAnimation();
		if (endIndex >= index && animationResource) {
			beginIndex = index;
			keyFramesSize = endIndex - beginIndex;
			duration = keyFramesSize * animationResource->duration / animationResource->keyFrames.size();
		}
	}

	void setEndIndex(unsigned int index) {
		ResourceAnimation* animationResource = GetResourceAnimation();
		if (index >= beginIndex && animationResource) {
			endIndex = index;
			keyFramesSize = endIndex - beginIndex;
			duration = keyFramesSize * animationResource->duration / animationResource->keyFrames.size();
		}
	}

	ResourceAnimation* GetResourceAnimation() {
		ResourceAnimation* a = (ResourceAnimation*) App->resources->GetResource(animationUID);
		return a;
	}

public:
	UID id;
	std::string name;
	UID animationUID;
	bool loop = false;
	unsigned int beginIndex = 0;
	unsigned int endIndex = 0;
	float duration = 0;
	unsigned int keyFramesSize = 0;
};
