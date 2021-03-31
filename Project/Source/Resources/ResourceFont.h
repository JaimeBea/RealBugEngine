#pragma once

#include "Resource.h"

#include "Math/float2.h"
#include <unordered_map>

struct Character {
	unsigned int textureID;
	float2 size;
	float2 bearing;
	unsigned int advance;
};

class ResourceFont : public Resource {
public:
	REGISTER_RESOURCE(ResourceFont, ResourceType::FONT);
	
	void Load() override;
	void Unload() override;

public:
	std::unordered_map<char, Character> characters; //Should character be a pointer? It's not a built-in type
	std::string name = "";
};
