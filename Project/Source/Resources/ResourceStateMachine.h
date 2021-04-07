#pragma once
#include "Resources/ResourceType.h"
#include "Resources/Resource.h"

#include <list>
#include <unordered_map>
#include "Utils/UID.h"

class ResourceStates;
class ResourceTransition;
class ResourceAnimation;
class Clip;
class ResourceStateMachine : public Resource {
private:

	std::list<ResourceStates*> states;
	//std::unordered_map<std::string, ResourceStates*> triggers;
	std::unordered_map<std::string, ResourceTransition*> transitions;
	ResourceStates* currentState = nullptr;

public:
	REGISTER_RESOURCE(ResourceStateMachine, ResourceType::STATE_MACHINE);

	ResourceStates* AddState(std::string name,Clip *clip);
	void AddTransition(ResourceStates* from, ResourceStates* to, float interpolation, std::string& name );
	ResourceTransition* GetValidTransition(std::string& name);
	ResourceTransition* FindTransitionGivenName(std::string& name);
	ResourceStates* GetCurrentState();
	std::unordered_map<std::string, ResourceAnimation*> resourceAnimations;

	void SetCurrentState(ResourceStates *newState) {
		currentState = newState;
	}

public:
	UID animationID = 0;
};
