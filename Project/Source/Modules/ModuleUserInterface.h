#pragma once

#include "Module.h"
#include "UI/FontImporter.h"
#include <Math/float2.h>

class ModuleUserInterface : public Module {
private:
	std::unordered_map<std::string, std::unordered_map<char, Character>> fonts; //Container for different fonts with the name of the font as key.

public:
	void AddFont(std::string fontPath); //Try to load the font and store it.
	Character GetCharacter(std::string font, char c); //Returns the Character that matches the given one in the given font or null otherwise.
	void GetCharactersInString(std::string font, std::string sentence, std::vector<Character>& charsInSentence); //Fills the given vector with the glyphs of the given font to form the given sentence.
	void OnMouseMovementDetected(float2 mousePos);
};
