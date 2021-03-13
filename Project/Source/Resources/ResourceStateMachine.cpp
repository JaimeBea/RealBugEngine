#include "ResourceStateMachine.h"
#include "TransitionResource.h"
#include "ResourceStates.h"
ResourceStateMachine::ResourceStateMachine() {
}



ResourceStates* ResourceStateMachine::AddState(std::string name) {
	//Checking for unique name
	TransitionResource* transition = FindTransitionGivenName(name);
	if (transition) {
		return nullptr;
	}

	ResourceStates *state = new ResourceStates(name,nullptr);
	states.push_back(state);

	return state;
}

void ResourceStateMachine::AddTransition(ResourceStates* from, ResourceStates* to, unsigned int interpolation, std::string& name) {
	//Checking for unique name
	TransitionResource* transition = FindTransitionGivenName(name);
	if (transition) {
		return;
	}

	transition = new TransitionResource(from, to, name, interpolation);
	transitions.insert(std::make_pair(name,transition));
}


void ResourceStateMachine::ChangeState(std::string& transitionName) {

	if (transitions.empty()) {
		return;
	}
	

	TransitionResource* transition = FindTransitionGivenName(transitionName);
	if (transition) {

		//check able movement
		if (GetCurrentState() == nullptr || GetCurrentState() == transition->source) {
			currentState = transition->target;
		} else {
			//movement not valid
		}

	}
	
}

TransitionResource* ResourceStateMachine::FindTransitionGivenName(std::string& name) {
	std::unordered_map<std::string, TransitionResource*>::const_iterator it = transitions.find(name);

	if (it != transitions.end()) {
		return it->second;
	} 

	return nullptr;
}

ResourceStates* ResourceStateMachine::GetCurrentState() {
	return currentState;
}
