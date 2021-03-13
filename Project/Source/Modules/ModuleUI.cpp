#include "ModuleUI.h"
#include "Globals.h"
#include "Application.h"
#include "Modules/ModuleRender.h"
#include "Components/ComponentCanvas.h"
#include "GL/glew.h"


bool ModuleUI::Init() {
	return true;
}

bool ModuleUI::Start() {
	return true;
}

void ModuleUI::Render() {
	//GameObject* canvasRenderer = canvas->GetChildren()[0];
	if (canvas != nullptr) {
		//canvas->GetChildren()[0]->GetComponent<ComponentCanvas>()->Render();
		canvas->GetComponent<ComponentCanvas>()->Render();

	}
}

bool ModuleUI::CleanUp() {
	return true;
}

void ModuleUI::StartUI() {
	
}

void ModuleUI::EndUI() {
}

GameObject* ModuleUI::GetCanvas() const {
	return canvas;
}
