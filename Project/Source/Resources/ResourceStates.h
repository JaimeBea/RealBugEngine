#pragma once
#include <string>
#include <unordered_map>

class Clip;
class TransitionResource;
class ResourceStates {

public:
	ResourceStates(std::string &name,Clip *clip);
	std::string name = "";
	Clip* clip = nullptr;
};
