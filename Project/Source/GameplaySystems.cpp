#include "GameplaySystems.h"

#include "Application.h"
#include "Modules/ModuleTime.h"
#include "Modules/ModuleScene.h"

float GameplaySystems::GetDeltaTime() {
	return App->time->GetDeltaTime();
}

GameObject* GameplaySystems::GetGameObject(const char* name) {
	return App->scene->scene->root->FindDescendant(name);
}
