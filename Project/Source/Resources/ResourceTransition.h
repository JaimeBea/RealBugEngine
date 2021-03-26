#pragma once
#include <string>

class ResourceStates;
class ResourceTransition {
	
public:
	ResourceTransition(ResourceStates* mSource, ResourceStates* mTarget, std::string& mTrigger, float mInterpolation)
		: source(mSource)
		, target(mTarget)
		, trigger(mTrigger)
		, interpolationDuration(mInterpolation)
	{

	}
	ResourceStates* source = nullptr;
	ResourceStates* target = nullptr;
	std::string trigger = "";
	float interpolationDuration = 0;
	float currentDuration = 0;
};
