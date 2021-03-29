#pragma once

#include "Utils/UID.h"

#include <Math/Quat.h>

class ResourceAnimation;

class AnimationController {
public:
	bool GetTransform(const char* name, float3& pos, Quat& quat);

	void Update();

private:
	Quat Interpolate(const Quat& first, const Quat& second, float lambda) const;

public:
	float currentTime = 0.f;
	bool loop = true;
	UID animationID = 0;
};