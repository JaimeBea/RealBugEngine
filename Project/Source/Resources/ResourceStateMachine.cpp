#include "ResourceStateMachine.h"
#include "ResourceTransition.h"
#include "ResourceStates.h"

ResourceStates* ResourceStateMachine::AddState(std::string name,Clip *clip) {
	//Checking for unique name
	ResourceTransition* transition = FindTransitionGivenName(name);
	if (transition) {
		return nullptr;
	}

	ResourceStates *state = new ResourceStates(name,clip);
	//Setting default state
	if (states.size() == 0) {
		currentState = state;
	}

	states.push_back(state);

	return state;
}

void ResourceStateMachine::AddTransition(ResourceStates* from, ResourceStates* to, float interpolation, std::string& name) {
	//Checking for unique name
	ResourceTransition* transition = FindTransitionGivenName(name);
	if (transition) {
		return;
	}

	transition = new ResourceTransition(from, to, name, interpolation);
	transitions.insert(std::make_pair(name,transition));
}


ResourceTransition* ResourceStateMachine::GetValidTransition(std::string& transitionName) {

	if (transitions.empty()) {
		return nullptr;
	}	

	return FindTransitionGivenName(transitionName);	
}

ResourceTransition* ResourceStateMachine::FindTransitionGivenName(std::string& name) {
	std::unordered_map<std::string, ResourceTransition*>::const_iterator it = transitions.find(name);

	if (it != transitions.end()) {
		return it->second;
	} 

	return nullptr;
}

ResourceStates* ResourceStateMachine::GetCurrentState() {
	return currentState;
}
