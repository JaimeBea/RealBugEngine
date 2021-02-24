#pragma once

#include "Math/float2.h"
#include <unordered_map>
#include <string>

struct Character {
	unsigned int textureID;
	float2 size;
	float2 bearing;
	unsigned int advance;
};

class FontImporter {
public:
	bool LoadFont(std::string fontPath); //Loads font from the path given.
	
private:
	std::unordered_map<char, Character> characters; //Stores all the characters from the loaded font.
};
