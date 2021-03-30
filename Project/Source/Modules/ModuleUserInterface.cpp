#include "ModuleUserInterface.h"

#include "Application.h"
#include "ModuleFiles.h"
#include "ModuleEventSystem.h"
#include "Utils/FileDialog.h"

#include "GameObject.h"
#include "Resources/ResourceFont.h"
#include "Components/ComponentCanvas.h"
#include "FileSystem/TextureImporter.h"
#include <Components/UI/ComponentSelectable.h>
#include "Components/ComponentEventSystem.h"
#include "Components/ComponentBoundingBox2D.h"
#include "Event.h"

#include "UI/Interfaces/IPointerEnterHandler.h"

#include "Utils/Logging.h"
#include "Utils/Leaks.h"

bool ModuleUserInterface::Init() {
	App->eventSystem->AddObserverToEvent(Event::EventType::MOUSE_UPDATE, this);
	return true;
}

bool ModuleUserInterface::Start() {
	// Testing font importer
	AddFont("./Fonts/fa-solid-900.ttf");
	GetCharacter("fa-solid-900", 'b');
	std::vector<Character> phrase;
	GetCharactersInString("fa-solid-900", "-This Is a test-", phrase);

	return true;
}

void ModuleUserInterface::AddFont(const std::string& fontPath) {
	//Right now we have FileDialog, it may change in the future
	//FileDialog::GetFileName(fontPath.c_str());

	//OLD VERSION, TO DO USE NEW SYSTEM, AS STATED IN THE PREVIOUS COMMENT

	std::string fontName = FileDialog::GetFileName(fontPath.c_str());

	std::unordered_map<std::string, ResourceFont*>::const_iterator existsKey = fonts.find(fontName);

	if (existsKey == fonts.end()) {
		ResourceFont* font = FontImporter::ImportFont(fontPath);
		//FontImporter::LoadFont(fontPath, characters);
		if (font) { // TODO: This is a bad check right now. It will always be initialized. Change it when we have the resource manager
			font->name = fontName;
			fonts.insert(std::pair<std::string, ResourceFont*>(fontName, font));
		} else {
			LOG("Couldn't load font %s", fontPath.c_str());
		}
	}
}

Character ModuleUserInterface::GetCharacter(const std::string& font, char c) { // Should this return a Character*?
	std::unordered_map<std::string, ResourceFont*>::const_iterator existsKey = fonts.find(font);

	if (existsKey == fonts.end()) {
		return Character();
	}
	return fonts[font]->characters[c];
}

void ModuleUserInterface::GetCharactersInString(const std::string& font, const std::string& sentence, std::vector<Character>& charsInSentence) {
	std::unordered_map<std::string, ResourceFont*>::const_iterator existsKey = fonts.find(font);

	if (existsKey == fonts.end()) {
		return;
	}

	for (std::string::const_iterator i = sentence.begin(); i != sentence.end(); ++i) {
		charsInSentence.push_back(fonts[font]->characters[*i]);
	}
}

void ModuleUserInterface::Render() {
	//GameObject* canvasRenderer = canvas->GetChildren()[0];
	if (canvas != nullptr) {
		//canvas->GetChildren()[0]->GetComponent<ComponentCanvas>()->Render();
		ComponentCanvas* canvasComponent = canvas->GetComponent<ComponentCanvas>();
		if (canvasComponent) {
			canvasComponent->Render();
		}
	}
}

void ModuleUserInterface::StartUI() {
}

void ModuleUserInterface::EndUI() {
}

GameObject* ModuleUserInterface::GetCanvas() const {
	return canvas;
}

void ModuleUserInterface::ReceiveEvent(const Event& e) {
	float2 mousePos = float2(e.point2d.x, e.point2d.y);
	switch (e.type) {
	case Event::EventType::MOUSE_UPDATE:
		if (ComponentEventSystem::currentEvSys) {
			for (ComponentSelectable* selectable : ComponentEventSystem::currentEvSys->m_Selectables) {
				ComponentBoundingBox2D* bb = selectable->GetOwner().GetComponent<ComponentBoundingBox2D>();

				if (!selectable->IsHovered()) {
					if (bb->GetWorldAABB().Contains(mousePos)) {
						selectable->OnPointerEnter();
					}
				} else {
					if (!bb->GetWorldAABB().Contains(mousePos)) {
						selectable->OnPointerExit();
					}
				}
			}
		}
		break;
	default:
		break;
	}
}

bool ModuleUserInterface::CleanUp() {
	// Don't like this, review
	// TODO: call glDeleteTextures for each character in a font
	for (std::unordered_map<std::string, ResourceFont*>::iterator it = fonts.begin(); it != fonts.end(); ++it) {
		RELEASE((*it).second);
	}
	fonts.clear();

	return true;
}
