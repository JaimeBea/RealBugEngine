#pragma once
#include <Math/Quat.h>
#include <list>

class ResourceClip;
class AnimationInterpolation;
class GameObject;

class AnimationController {
public:
	static bool GetTransform(const ResourceClip &clip, float& currentTime, const char* name, float3& pos, Quat& quat);
	static bool InterpolateTransitions(const std::list<AnimationInterpolation>::iterator &it, std::list<AnimationInterpolation> &animationInterpolations, GameObject& rootBone, GameObject& gameObject, float3& pos, Quat& quat);
	static Quat Interpolate(const Quat& first, const Quat& second, float lambda);
};