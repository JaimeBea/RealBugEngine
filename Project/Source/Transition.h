#pragma once
#include "States.h"
#include "Utils/UID.h"
#include <string>

class Transition {
	
public:
	Transition(const States& mSource,const States& mTarget, float mInterpolation,UID mid = 0)
		: source(mSource)
		, target(mTarget)
		, interpolationDuration(mInterpolation)
	{
		id = mid != 0 ? mid :GenerateUID();
	}

public:
	UID id = 0;
	States source;
	States target;
	float interpolationDuration = 0;
};
