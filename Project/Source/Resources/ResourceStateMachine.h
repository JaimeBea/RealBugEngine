#pragma once
#include <list>

class ResourceStates;
//class AnimationClips;
class ResourceStateMachine {

private:
	//List<AnimationClips> clips;
	std::list<ResourceStates> states;
	std::list<ResourceStates> triggers;
	std::list<ResourceStates> transitions;

	ResourceStates *currentState;


};
