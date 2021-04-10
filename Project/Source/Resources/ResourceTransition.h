#pragma once
#include <string>
#include "Utils/UID.h"
class ResourceStates;
class ResourceTransition {
	
public:
	ResourceTransition(ResourceStates* mSource, ResourceStates* mTarget, float mInterpolation,UID mid = 0)
		: source(mSource)
		, target(mTarget)
		, interpolationDuration(mInterpolation)
	{
		id = mid != 0 ? mid :GenerateUID();
	}

public:
	UID id;
	ResourceStates* source = nullptr;
	ResourceStates* target = nullptr;
	float interpolationDuration = 0;
};
