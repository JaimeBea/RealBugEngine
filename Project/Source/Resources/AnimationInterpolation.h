#pragma once
class ResourceStates;

class AnimationInterpolation {
public:
	AnimationInterpolation(ResourceStates* resourceState, float current, float fade, float transition)
		: state(resourceState)
		, currentTime(current)
		, fadeTime(fade)
		, TransistionTime(transition) {}
	ResourceStates* state = nullptr;
	float currentTime = 0;
	float fadeTime = 0;
	float TransistionTime = 0;
};