#pragma once
#include "Utils/UID.h"
#include <string>

class Clip;
class ResourceStates {

public:
	ResourceStates(std::string& mName, Clip* mClip, float mCurrentTime = 0,UID mid = 0)
		: name(mName)
		, clip(mClip)
		, currentTime(mCurrentTime) {

		id = mid != 0 ? mid :GenerateUID();
	}

public:
	UID id;
	std::string name = "";
	Clip* clip = nullptr;
	float currentTime = 0;

};
