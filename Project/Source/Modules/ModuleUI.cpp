#include "ModuleUI.h"
#include "Globals.h"
#include "Application.h"
#include "Modules/ModuleRender.h"
#include "GL/glew.h"


bool ModuleUI::Init() {
	return true;
}

bool ModuleUI::Start() {
	return true;
}

UpdateStatus ModuleUI::Update() {
	// Change to ortho perspective
	//App->renderer->EnableOrtographicRender();


	return UpdateStatus::CONTINUE;
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
