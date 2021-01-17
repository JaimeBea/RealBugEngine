#pragma once

#include "Module.h"
#include "Quadtree.h"

#include "Math/float3.h"

class GameObject;

class ModuleSceneRender : public Module
{
public:
	UpdateStatus Update() override;

private:
	void DrawQuadtreeRecursive(const Quadtree<GameObject>::Node& node, const AABB2D& aabb);
	void DrawSceneRecursive(const Quadtree<GameObject>::Node& node, const AABB2D& aabb);
	bool CheckIfInsideFrustum(const AABB& aabb, const OBB& obb);
	void DrawGameObject(GameObject* game_object);
	void DrawSkyBox();

public:
	bool draw_quadtree = false;
	bool draw_all_bounding_boxes = false;
	bool skybox_active = true;
	float3 ambient_color = {0.0f, 0.0f, 0.0f};
};
