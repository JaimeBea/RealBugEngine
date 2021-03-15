#pragma once
#include <list>
#include <unordered_map>

class ResourceStates;
class ResourceTransition;
//class AnimationClips;

class ResourceStateMachine {
private:
	//List<AnimationClips> clips;
	std::list<ResourceStates*> states;
	//std::unordered_map<std::string, ResourceStates*> triggers;
	std::unordered_map<std::string, ResourceTransition*> transitions;

	ResourceStates* currentState = nullptr;

public:
	ResourceStateMachine();
	ResourceStates* AddState(std::string name); //Missing clip parameter
	void AddTransition(ResourceStates* from, ResourceStates* to,unsigned int interpolation, std::string& name );
	//void AddTrigger(std::string &name, ResourceStates *state);
	void ChangeState(std::string &name);
	ResourceTransition* FindTransitionGivenName(std::string& name);
	ResourceStates* GetCurrentState();
};
