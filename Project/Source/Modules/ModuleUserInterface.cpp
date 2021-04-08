#include "ModuleUserInterface.h"

#include "Application.h"
#include "ModuleFiles.h"
#include "ModuleEventSystem.h"
#include "Modules/ModuleResources.h"
#include "ModuleScene.h"
#include "Utils/FileDialog.h"
#include "FileSystem/TextureImporter.h"

#include "GameObject.h"
#include "Resources/ResourceFont.h"
#include "Scene.h"
#include "Components/ComponentCanvas.h"
#include "Components/UI/ComponentSelectable.h"

#include "Components/ComponentEventSystem.h"
#include "Components/ComponentBoundingBox2D.h"
#include "Components/ComponentEventSystem.h"
#include "Event.h"

#include "UI/Interfaces/IPointerEnterHandler.h"
#include "UI/Interfaces/IPointerExitHandler.h"
#include "UI/Interfaces/IMouseClickHandler.h"

#include "Utils/Logging.h"
#include "Utils/Leaks.h"

ComponentEventSystem* ModuleUserInterface::currentEvSys = nullptr;

bool ModuleUserInterface::Init() {
	App->eventSystem->AddObserverToEvent(Event::EventType::MOUSE_UPDATE, this);
	App->eventSystem->AddObserverToEvent(Event::EventType::MOUSE_CLICKED, this);
	return true;
}

bool ModuleUserInterface::Start() {
	CreateQuadVBO();

	/* Testing font importer
	AddFont("./Fonts/fa-solid-900.ttf");
	GetCharacter("fa-solid-900", 'b');
	std::vector<Character> phrase;
	GetCharactersInString("fa-solid-900", "-This Is a test-", phrase); */
	return true;
}

void ModuleUserInterface::AddFont(const std::string& fontPath) {
	//Right now we have FileDialog, it may change in the future
	//FileDialog::GetFileName(fontPath.c_str());

	//OLD VERSION, TO DO USE NEW SYSTEM, AS STATED IN THE PREVIOUS COMMENT
	/*
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
	*/
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
	// if (importFont) {
	// 	// Temporary code to check font resource is loading fine
	// 	std::vector<UID>& fontResourcesID = App->resources->ImportAsset("Assets/fa-solid-900.ttf");
	// 	UID fontId = fontResourcesID[0];
	// 	App->resources->IncreaseReferenceCount(fontId);
	// 	ResourceFont* font = (ResourceFont*)App->resources->GetResource(fontId);
	// 	font->name = "fa-solid-900";
	// 	fonts.insert(std::pair<std::string, ResourceFont*>(font->name, font));
	// 	GetCharacter("fa-solid-900", 'b');
	// 	std::vector<Character> phrase;
	// 	GetCharactersInString("fa-solid-900", "-This Is a test-", phrase);
	// 	importFont = false;
	//}
	Scene* scene = App->scene->scene;
	if (scene != nullptr) {
		for (ComponentCanvasRenderer canvasRenderer : scene->canvasRendererComponents) {
			canvasRenderer.Render(&canvasRenderer.GetOwner());
		}
	}
}

GameObject* ModuleUserInterface::GetCanvas() const {
	return canvas;
}

void ModuleUserInterface::ReceiveEvent(const Event& e) {
	float2 mousePos = float2(e.point2d.x, e.point2d.y);
	switch (e.type) {
	case Event::EventType::MOUSE_UPDATE:
		if (currentEvSys) {
			for (ComponentSelectable& selectable : App->scene->scene->selectableComponents) {
				ComponentBoundingBox2D* bb = selectable.GetOwner().GetComponent<ComponentBoundingBox2D>();

				if (!selectable.IsHovered()) {
					if (bb->GetWorldAABB().Contains(mousePos)) {
						selectable.OnPointerEnter();
					}
				} else {
					if (!bb->GetWorldAABB().Contains(mousePos)) {
						selectable.OnPointerExit();
					}
				}
			}
		}
		break;

	case Event::EventType::MOUSE_CLICKED:
		if (currentEvSys != nullptr) {
			ComponentSelectable* lastHoveredSelectable = currentEvSys->GetCurrentlyHovered();
			if (lastHoveredSelectable != nullptr) {
				if (lastHoveredSelectable->IsInteractable()) {
					IMouseClickHandler* i = dynamic_cast<IMouseClickHandler*>(lastHoveredSelectable->GetSelectableComponent());
					if (i != nullptr) {
						i->OnClicked();
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
	glDeleteBuffers(1, &quadVBO);
	return true;
}

unsigned int ModuleUserInterface::GetQuadVBO() {
	return quadVBO;
}

void ModuleUserInterface::CreateQuadVBO() {
	float buffer_data[] = {
		-0.5f,
		-0.5f,
		0.0f, //  v0 pos

		0.5f,
		-0.5f,
		0.0f, // v1 pos

		-0.5f,
		0.5f,
		0.0f, //  v2 pos

		0.5f,
		-0.5f,
		0.0f, //  v3 pos

		0.5f,
		0.5f,
		0.0f, // v4 pos

		-0.5f,
		0.5f,
		0.0f, //  v5 pos

		0.0f,
		0.0f, //  v0 texcoord

		1.0f,
		0.0f, //  v1 texcoord

		0.0f,
		1.0f, //  v2 texcoord

		1.0f,
		0.0f, //  v3 texcoord

		1.0f,
		1.0f, //  v4 texcoord

		0.0f,
		1.0f //  v5 texcoord
	};

	glGenBuffers(1, &quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO); // set vbo active
	glBufferData(GL_ARRAY_BUFFER, sizeof(buffer_data), buffer_data, GL_STATIC_DRAW);
}

void ModuleUserInterface::SetCurrentEventSystem(ComponentEventSystem* ev) {
	currentEvSys = ev;
}

ComponentEventSystem* ModuleUserInterface::GetCurrentEventSystem() {
	return currentEvSys;
}
