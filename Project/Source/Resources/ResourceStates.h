#pragma once
#include <string>
#include <unordered_map>

class Clip;
class ResourceTransition;
class ResourceStates {

public:
	std::string name;
	Clip* clip;
	std::unordered_map<std::string, ResourceTransition>transitions;
};
