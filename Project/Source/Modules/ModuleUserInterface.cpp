#include "ModuleUserInterface.h"
#include "Application.h"
#include "ModuleFiles.h"
#include "Utils/Logging.h"
#include "Utils/Leaks.h"
void ModuleUserInterface::AddFont(std::string fontPath) {
	std::string fontName = App->files->GetFileName(fontPath.c_str());

	std::unordered_map<std::string, std::unordered_map<char, Character>>::const_iterator existsKey = fonts.find(fontName);

	if (existsKey == fonts.end()) {
		std::unordered_map<char, Character> characters;
		FontImporter::LoadFont(fontPath, characters);
		fonts.insert(std::pair<std::string, std::unordered_map<char, Character>>(fontName, characters));
	}
}

Character ModuleUserInterface::GetCharacter(std::string font, char c) {
	std::unordered_map<std::string, std::unordered_map<char, Character>>::const_iterator existsKey = fonts.find(font);

	if (existsKey == fonts.end()) {
		return Character();
	}
	return fonts[font][c];
}

void ModuleUserInterface::GetCharactersInString(std::string font, std::string sentence, std::vector<Character>& charsInSentence) {
	std::unordered_map<std::string, std::unordered_map<char, Character>>::const_iterator existsKey = fonts.find(font);

	if (existsKey == fonts.end()) {
		return;
	}

	for (std::string::const_iterator i = sentence.begin(); i != sentence.end(); ++i) {
		charsInSentence.push_back(fonts[font][*i]);
	}
}

void ModuleUserInterface::OnMouseMovementDetected(float2 mousePos) {
	//LOG("%f,%f", mousePos.x, mousePos.y);
}
