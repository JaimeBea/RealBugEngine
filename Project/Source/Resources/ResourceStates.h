#pragma once
#include <string>
#include <unordered_map>

class Clip;
class TransitionResource;
class ResourceStates {

public:
	std::string name;
	Clip* clip;
	std::unordered_map<std::string, TransitionResource>transitions;
};
