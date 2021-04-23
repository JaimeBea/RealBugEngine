#pragma once
#include "Resources/ResourceType.h"
#include "Resources/Resource.h"

#include <list>
#include <unordered_map>
#include "Utils/UID.h"
#include "States.h"
#include "Transition.h"

class ResourceAnimation;
class ResourceStateMachine : public Resource {

public:
	REGISTER_RESOURCE(ResourceStateMachine, ResourceType::STATE_MACHINE);

	void Load() override;
	void Unload() override;
	void SaveToFile(const char* filePath);

	States AddState(const std::string &name,UID clipUid);	//Add state to list of states and add clip to list of clips if dosen't contains him
	void AddClip(UID clipUid);
	void AddTransition(const States& from, const States& to,const float interpolation,const std::string& name );
	Transition* FindTransitionGivenName(const std::string& name);

public:
	std::unordered_map<std::string, ResourceAnimation*> resourceAnimations;
	std::list<States> states;

private:
	std::list<UID> clipsUids;	
	std::unordered_map<std::string, Transition> transitions;
};
