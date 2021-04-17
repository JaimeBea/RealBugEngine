#pragma once
#include <string>
#include "Utils/UID.h"
class States;
class Transition {
	
public:
	Transition(States* mSource, States* mTarget, float mInterpolation,UID mid = 0)
		: source(mSource)
		, target(mTarget)
		, interpolationDuration(mInterpolation)
	{
		id = mid != 0 ? mid :GenerateUID();
	}

public:
	UID id;
	States* source = nullptr;
	States* target = nullptr;
	float interpolationDuration = 0;
};
