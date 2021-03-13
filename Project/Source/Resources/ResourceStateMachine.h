#pragma once
#include <list>
#include <unordered_map>
#include "ResourceStates.h"
class TransitionResource;
//class ResourceStates;
//class AnimationClips;
class ResourceStateMachine {
private:
	//List<AnimationClips> clips;
	std::list<ResourceStates*> states;
	//std::unordered_map<std::string, ResourceStates*> triggers;
	std::unordered_map<std::string, TransitionResource*> transitions;

	ResourceStates* currentState = nullptr;

public:
	ResourceStateMachine();
	ResourceStates* AddState(std::string name); //Missing clip parameter
	void AddTransition(ResourceStates* from, ResourceStates* to,unsigned int interpolation, std::string& name );
	//void AddTrigger(std::string &name, ResourceStates *state);
	void ChangeState(std::string &name);
	TransitionResource* FindTransitionGivenName(std::string& name);
	ResourceStates* GetCurrentState();
};
