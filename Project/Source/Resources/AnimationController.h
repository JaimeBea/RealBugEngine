#pragma once
#include <Math/Quat.h>

class ResourceAnimation;

class AnimationController {
public:
	AnimationController();

	void SetAnimation(ResourceAnimation* animation);

	bool GetTransform(const char* name, float3& pos, Quat& quat);

	void Play();
	void Stop();
	void Update();

private:
	float currentTime;
	bool loop;
	bool running;
	ResourceAnimation* animationResource = nullptr;

	Quat Interpolate(const Quat& first, const Quat& second, float lambda) const;
};