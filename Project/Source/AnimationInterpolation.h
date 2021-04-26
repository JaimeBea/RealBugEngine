#pragma once

class States;

class AnimationInterpolation {
public:
	AnimationInterpolation(States* resourceState, float current, float fade, float transition)
		: state(resourceState)
		, currentTime(current)
		, fadeTime(fade)
		, TransistionTime(transition) {}

public:
	States* state;
	float currentTime = 0;
	float fadeTime = 0;
	float TransistionTime = 0;
};