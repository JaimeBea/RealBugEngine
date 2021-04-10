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

public:
	REGISTER_RESOURCE(ResourceStateMachine, ResourceType::STATE_MACHINE);

	void Load() override;
	void Unload() override;
	void SaveToFile(const char* filePath);

	ResourceStates* AddState(std::string name,Clip* clip);	//Add state to list of states and add clip to list of clips if dosen't contains him
	void AddClip(Clip* clip);
	void AddTransition(ResourceStates* from, ResourceStates* to, float interpolation, std::string& name );
	ResourceTransition* GetValidTransition(std::string& name);
	ResourceTransition* FindTransitionGivenName(std::string& name);
	ResourceStates* GetCurrentState();

	void SetCurrentState(ResourceStates *newState) {
		currentState = newState;
	}

public:
	std::unordered_map<std::string, ResourceAnimation*> resourceAnimations;

private:
	std::list<Clip*> clips;	
	std::list<ResourceStates*> states;
	std::unordered_map<std::string, ResourceTransition*> transitions;
	ResourceStates* currentState = nullptr; //TODO move to component animation

};
