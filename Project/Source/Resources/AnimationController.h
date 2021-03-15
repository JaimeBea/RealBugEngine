#pragma once
#include <Math/Quat.h>

class ResourceAnimation;
class Clip;

class AnimationController {
public:
	AnimationController(ResourceAnimation* resourceAnimation);
	bool GetTransform(Clip* clip, const char* name, float3& pos, Quat& quat);
	void Play();
	void Stop();
	void Update();
	void InterpolateTransitions(Clip* source, Clip* target, unsigned int interpolationTime, const char* name, float3& pos, Quat& quat);

private:
	float currentTime;
	bool loop;
	bool running;
	//ResourceAnimation* animationResource = nullptr;
	//std::vector<Clips> clips;

	Quat Interpolate(const Quat& first, const Quat& second, float lambda) const;
};