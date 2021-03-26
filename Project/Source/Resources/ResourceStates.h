#pragma once
#include <string>

class Clip;
class ResourceTransition;
class ResourceStates {

public:
	ResourceStates(std::string &name,Clip *clip);
	std::string name = "";
	Clip* clip = nullptr;
	float currentTime = 0;
};
