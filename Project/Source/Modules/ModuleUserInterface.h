#pragma once

#include "Module.h"
#include "UI/FontImporter.h"
#include "Utils/UID.h"

#include <unordered_map>

class GameObject;
class ComponentEventSystem;
class ResourceFont;

struct Character;

class ModuleUserInterface : public Module {
public:
	bool Init() override;
	bool Start() override;
	void AddFont(const std::string& fontPath);																				   //Try to load the font and store it.
	Character GetCharacter(UID font, char c);																   //Returns the Character that matches the given one in the given font or null otherwise.
	void GetCharactersInString(UID font, const std::string& sentence, std::vector<Character>& charsInSentence);				   //Fills the given vector with the glyphs of the given font to form the given sentence.

	void Render();
	GameObject* GetCanvas() const;
	void ReceiveEvent(const Event& e) override;
	void SetCurrentEventSystem(ComponentEventSystem* ev);
	ComponentEventSystem* GetCurrentEventSystem();

	bool CleanUp() override;

private:
	//std::<UID> fonts; //Container for different fonts with the name of the font as key.
	//bool importFont = true; // Temporary to check font resource is loading fine
	static ComponentEventSystem* currentEvSys;

public:
	GameObject* canvas = nullptr;
};
