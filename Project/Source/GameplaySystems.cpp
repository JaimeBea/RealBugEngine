#include "GameplaySystems.h"

#include "Application.h"
#include "Components/UI/ComponentTransform2D.h"
#include "Modules/ModuleTime.h"
#include "Modules/ModuleScene.h"
#include "Modules/ModuleInput.h"
#include "Modules/ModuleCamera.h"
#include "Modules/ModuleRender.h"
#include "Modules/ModuleWindow.h"
#include "FileSystem/SceneImporter.h"
#include <SDL_events.h>

// ----------- GAMEPLAY ------------ //
GameObject* GameplaySystems::GetGameObject(const char* name) {
	return App->scene->scene->root->FindDescendant(name);
}

TESSERACT_ENGINE_API void GameplaySystems::SetRenderCamera(GameObject* camera) {
	App->camera->ChangeActiveFrustum(camera->GetComponent<ComponentCamera>()->frustum, true);
	App->camera->ChangeCullingFrustum(camera->GetComponent<ComponentCamera>()->frustum, true);
}

// ------------- DEBUG ------------- //

void Debug::ToggleDebugMode() {
	App->renderer->ToggleDebugMode();
}

void Debug::ToggleDebugDraw() {
	App->renderer->ToggleDebugDraw();
}

void Debug::ToggleDrawQuadtree() {
	App->renderer->ToggleDrawQuadtree();
}

void Debug::ToggleDrawBBoxes() {
	App->renderer->ToggleDrawBBoxes();
}

void Debug::ToggleDrawSkybox() {
	App->renderer->ToggleDrawSkybox();
}

void Debug::ToggleDrawAnimationBones() {
	App->renderer->ToggleDrawAnimationBones();
}

void Debug::ToggleDrawCameraFrustums() {
	App->renderer->ToggleDrawCameraFrustums();
}

void Debug::ToggleDrawLightGizmos() {
	App->renderer->ToggleDrawLightGizmos();
}

void Debug::UpdateShadingMode(const char* shadingMode) {
	App->renderer->UpdateShadingMode(shadingMode);
}

// ------------- TIME -------------- //
float Time::GetDeltaTime() {
	return App->time->GetDeltaTime();
}

// ------------- INPUT ------------- //
bool Input::GetMouseButtonDown(int button) {
	return App->input->GetMouseButtons()[button] == KS_DOWN;
}

bool Input::GetMouseButtonUp(int button) {
	return App->input->GetMouseButtons()[button] == KS_UP;
}

bool Input::GetMouseButtonRepeat(int button) {
	return App->input->GetMouseButtons()[button] == KS_REPEAT;
}

bool Input::GetMouseButton(int button) {
	return App->input->GetMouseButtons()[button];
}

const float2& Input::GetMouseMotion() {
	return App->input->GetMouseMotion();
}

bool Input::GetKeyCodeDown(KEYCODE keycode) {
	return App->input->GetKeyboard()[keycode] == KS_DOWN;
}

bool Input::GetKeyCodeUp(KEYCODE keycode) {
	return App->input->GetKeyboard()[keycode] == KS_UP;
}

bool Input::GetKeyCodeRepeat(KEYCODE keycode) {
	return App->input->GetKeyboard()[keycode] == KS_REPEAT;
}

bool Input::GetKeyCode(KEYCODE keycode) {
	return App->input->GetKeyboard()[keycode];
}

// --------- SCENE MANAGER --------- //
void SceneManager::SceneLoad(const char* filePath) {
	SceneImporter::LoadScene(filePath);
}

void SceneManager::ExitGame() {
	SDL_Event event;
	event.type = SDL_QUIT;
	SDL_PushEvent(&event);
}

float Screen::GetScreenWitdh() {
	return App->window->GetWidth();
}

float Screen::GetScreenHeight() {
	return App->window->GetHeight();
}