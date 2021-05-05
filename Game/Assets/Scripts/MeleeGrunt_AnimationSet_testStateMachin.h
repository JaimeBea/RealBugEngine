#pragma once

#include "Scripting/Script.h"

class ComponentAnimation;

class MeleeGrunt_AnimationSet_testStateMachin : public Script
{
	GENERATE_BODY(MeleeGrunt_AnimationSet_testStateMachin);

public:

	void Start() override;
	void Update() override;

private:
	ComponentAnimation* animation = nullptr;
};

