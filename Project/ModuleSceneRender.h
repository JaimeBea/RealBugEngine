#pragma once
#include "Module.h"

#include "Math/float3.h"

class GameObject;

class ModuleSceneRender : public Module
{
public:
	UpdateStatus Update() override;

private:
	void DrawGameObject(GameObject* game_object);
	void DrawSkyBox();

public:
	bool skybox_active = true;
	float3 ambient_color = {0.0f, 0.0f, 0.0f};
};
