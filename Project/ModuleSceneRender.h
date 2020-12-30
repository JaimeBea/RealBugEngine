#pragma once
#include "Module.h"

class GameObject;

class ModuleSceneRender : public Module
{
public:
	UpdateStatus Update() override;

private:
	void DrawGameObject(GameObject* game_object);
};
