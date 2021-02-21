#pragma once
#include <Math/Quat.h>


class AnimationController {
public:
	unsigned int currentTime;
	bool loop;
	bool running;
	//AnimationResource *animationResource = nullptr;
	bool GetTransform(const char *name, float3& pos, Quat &quat );
	void Play();
	void Stop();
	void Update();

private:
	
	Quat Interpolate(const Quat& first, const Quat& second, float lambda) const;
};