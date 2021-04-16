#pragma once
#include <Math/Quat.h>
#include <list>

class Clip;
class AnimationInterpolation;
class GameObject;

static class AnimationController {
public:
	static bool GetTransform(Clip* clip, float& currentTime, const char* name, float3& pos, Quat& quat);
	static bool InterpolateTransitions(std::list<AnimationInterpolation*>::iterator it, std::list<AnimationInterpolation*> animationInterpolations, GameObject* rootBone, GameObject* gameObject, float3& pos, Quat& quat);
	static Quat Interpolate(const Quat& first, const Quat& second, float lambda);
};