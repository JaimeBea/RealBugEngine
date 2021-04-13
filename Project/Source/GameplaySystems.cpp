#include "GameplaySystems.h"

#include "Application.h"
#include "Components/UI/ComponentTransform2D.h"
#include "Modules/ModuleTime.h"
#include "Modules/ModuleScene.h"
#include "Modules/ModuleInput.h"
#include "Modules/ModuleWindow.h"
#include "FileSystem/SceneImporter.h"

float GameplaySystems::GetDeltaTime() {
	return App->time->GetDeltaTime();
}

GameObject* GameplaySystems::GetGameObject(const char* name) {
	return App->scene->scene->root->FindDescendant(name);
}

bool Input::GetMouseButtonDown(int button) {
	return App->input->GetMouseButtons()[button] == KS_DOWN;
}

bool Input::GetMouseButtonUp(int button) {
	return App->input->GetMouseButtons()[button] == KS_UP;
}

bool Input::GetMouseButton(int button) {
	return App->input->GetMouseButtons()[button] == KS_REPEAT;
}

bool Input::GetKeyCodeDown(KEYCODE keycode) {
	return App->input->GetKeyboard()[keycode] == KS_DOWN;
}

bool Input::GetKeyCodeUp(KEYCODE keycode) {
	return App->input->GetKeyboard()[keycode] == KS_UP;
}

bool Input::GetKeyCode(KEYCODE keycode) {
	return App->input->GetKeyboard()[keycode] == KS_REPEAT;
}

void SceneManager::SceneLoad(const char* filePath) {
	SceneImporter::LoadScene(filePath);
}

TESSERACT_ENGINE_API UpdateStatus SceneManager::ExitGame() {
	return UpdateStatus::STOP;
}

float Screen::GetScreenWitdh() {
	return App->window->GetWidth();
}

float Screen::GetScreenHeight() {
	return App->window->GetHeight();
}