#pragma once
#include <Math/Quat.h>
#include <list>

class ResourceAnimation;
class Clip;
class AnimationInterpolation;
class GameObject;

class AnimationController {
public:
	AnimationController(ResourceAnimation* resourceAnimation);
	bool GetTransform(Clip* clip, const char* name, float3& pos, Quat& quat);
	void Play();
	void Stop();
	void Update();
	bool InterpolateTransitions(std::list<AnimationInterpolation*>::iterator it, std::list<AnimationInterpolation*> animationInterpolations, GameObject* gameObject, float3& pos, Quat& quat);
	Quat Interpolate(const Quat& first, const Quat& second, float lambda) const;

	void SetAnimationResource(ResourceAnimation* animation) {
		animationResource = animation;
	}

	float currentTime = 0;
private:
	bool running = false;
	ResourceAnimation* animationResource = nullptr;
	//std::vector<Clips> clips;

};