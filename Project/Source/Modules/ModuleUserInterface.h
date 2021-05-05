#pragma once

#include "Module.h"
#include "Utils/UID.h"
#include "Math/float2.h"

class GameObject;
class ComponentEventSystem;
class ResourceFont;

struct Character;

class ModuleUserInterface : public Module {
public:
	bool Init() override;
	bool Start() override;
	UpdateStatus Update() override;
	bool CleanUp() override;
	void ReceiveEvent(TesseractEvent& e) override;

	Character GetCharacter(UID font, char c);																	//Returns the Character that matches the given one in the given font or null otherwise.
	void GetCharactersInString(UID font, const std::string& sentence, std::vector<Character>& charsInSentence); //Fills the given vector with the glyphs of the given font to form the given sentence.
	void Render();
	void SetCurrentEventSystem(UID id_);
	ComponentEventSystem* GetCurrentEventSystem();

	unsigned int GetQuadVBO();
	void ViewportResized(); // To be called when the viewport panel changes size.

private:
	void CreateQuadVBO(); // Creates a vbo made by two triangles centered that form a Quad

private:
	UID currentEvSys = 0;
	unsigned int quadVBO = 0;
};
