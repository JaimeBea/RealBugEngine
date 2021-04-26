#pragma once

class State;

class AnimationInterpolation {
public:
	AnimationInterpolation(State* resourceState, float current, float fade, float transition)
		: state(resourceState)
		, currentTime(current)
		, fadeTime(fade)
		, TransistionTime(transition) {}

public:
	State* state;
	float currentTime = 0;
	float fadeTime = 0;
	float TransistionTime = 0;
};