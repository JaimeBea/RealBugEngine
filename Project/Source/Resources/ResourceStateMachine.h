#pragma once
#include <list>
#include <unordered_map>

class ResourceStates;
class ResourceTransition;
class ResourceAnimation;
class Clip;
class ResourceStateMachine {
private:

	std::list<ResourceStates*> states;
	//std::unordered_map<std::string, ResourceStates*> triggers;
	std::unordered_map<std::string, ResourceTransition*> transitions;
	ResourceStates* currentState = nullptr;

public:
	ResourceStateMachine();
	ResourceStates* AddState(std::string name,Clip *clip);
	void AddTransition(ResourceStates* from, ResourceStates* to, float interpolation, std::string& name );
	ResourceTransition* GetValidTransition(std::string& name);
	ResourceTransition* FindTransitionGivenName(std::string& name);
	ResourceStates* GetCurrentState();
	std::unordered_map<std::string, ResourceAnimation*> resourceAnimations;

	void SetCurrentState(ResourceStates *newState) {
		currentState = newState;
	}
};
