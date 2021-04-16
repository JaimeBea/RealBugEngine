#pragma once
#include "Utils/UID.h"
#include <string>

class ResourceClip;
class ResourceStates {

public:
	ResourceStates(std::string& mName, ResourceClip* mClip, float mCurrentTime = 0, UID mid = 0)
		: name(mName)
		, clip(mClip)
		, currentTime(mCurrentTime) {

		id = mid != 0 ? mid :GenerateUID();
	}

public:
	UID id;
	std::string name = "";
	ResourceClip* clip = nullptr;
	float currentTime = 0;

};
