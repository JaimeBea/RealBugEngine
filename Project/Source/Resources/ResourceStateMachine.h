#pragma once
#include <list>

class ResourceStates;
class ResourceTransition;
//class AnimationClips;
class ResourceStateMachine {

public:
	//List<AnimationClips> clips;
	std::list<ResourceStates> states;
	std::list<ResourceStates> triggers;
	std::list<ResourceTransition> transitions;

	ResourceStates *currentState;


};
