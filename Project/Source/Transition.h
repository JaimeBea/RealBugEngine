#pragma once
#include <string>
#include "Utils/UID.h"
#include "States.h"
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
	UID id;
	States source;
	States target;
	float interpolationDuration = 0;
};
