#pragma once
#include "Utils/UID.h"
#include <string>

class States {

public:
	States() {}
	States(const States& mState)
		: id(mState.id)
		, name(mState.name)
		, clipUid(mState.clipUid)
		, currentTime(mState.currentTime){
	}

	States(const std::string& mName, UID mClipUid, float mCurrentTime = 0, UID mid = 0)
		: name(mName)
		, clipUid(mClipUid)
		, currentTime(mCurrentTime) {

		id = mid != 0 ? mid :GenerateUID();
	}

public:
	UID id;
	std::string name = "";
	UID clipUid = 0;
	float currentTime = 0;

};
