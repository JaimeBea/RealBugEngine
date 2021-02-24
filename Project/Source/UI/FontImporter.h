#pragma once

#include "Math/float2.h"
#include <unordered_map>

struct Character {
	unsigned int textureID;
	float2 size;
	float2 bearing;
	unsigned int advance;
};

class FontImporter {
public:
	bool LoadFont(std::string fontPath); //Loads font from the given path.
	Character GetCharacter(char c); //Returns the Character that matches the given one or null otherwise.
	void GetCharactersInString(std::string sentence, std::vector<Character>& charsInSentence); //Fills the given vector with the characters that match the given string.
	
private:
	std::unordered_map<char, Character> characters; //Stores all the characters from the loaded font.
};
