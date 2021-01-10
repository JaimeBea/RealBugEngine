#pragma once
#include "Module.h"

#include "Math/float3.h"

class GameObject;
struct FrustumPlanes;

class ModuleSceneRender : public Module
{
public:
	UpdateStatus Update() override;

private:
	void DrawGameObjectRecursive(GameObject* game_object);
	bool CheckIfInsideFrustum(GameObject* game_object, FrustumPlanes* frustum_planes);
	void DrawGameObject(GameObject* game_object);
	void DrawSkyBox();

public:
	bool draw_all_bounding_boxes = false;
	bool skybox_active = true;
	float3 ambient_color = {0.0f, 0.0f, 0.0f};

private:
	bool frustum_culling_active = false;
};
