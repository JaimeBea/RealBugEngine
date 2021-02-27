#pragma once

#include "Math/float2.h"
#include <unordered_map>

struct Character {
	unsigned int textureID;
	float2 size;
	float2 bearing;
	unsigned int advance;
};

namespace FontImporter {
	void LoadFont(std::string fontPath, std::unordered_map<char, Character>& characters); //Loads font from the given path in the given map.	
}; //namespace FontImporter
