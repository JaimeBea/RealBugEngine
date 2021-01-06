#pragma once
#include "Module.h"

#include "Math/float3.h"
#include "Geometry/Plane.h"
#include <list>

class GameObject;

struct FrustumNormals
{
	float3 points[8]; // 0: ftl, 1: ftr, 2: fbl, 3: fbr, 4: ntl, 5: ntr, 6: nbl, 7: nbr. (far/near, top/bottom, left/right).
	Plane planes[6]; // left, right, up, down, front, back
};

class ModuleSceneRender : public Module
{
public:
	UpdateStatus Update() override;

private:
	void DrawScene(GameObject* scene);
	FrustumNormals GetFrustumNormals(Frustum frustum);
	bool CheckIfInsideFrustum(GameObject* game_object, FrustumNormals* frustum_normal);
	void DrawGameObject(GameObject* game_object);
	void DrawSkyBox();

public:
	bool draw_all_bounding_boxes = false;
	bool skybox_active = true;
	float3 ambient_color = {0.0f, 0.0f, 0.0f};

private:
	bool frustum_culling_active = false;
	std::list<GameObject*> drawable_game_objects;
	std::list<FrustumNormals*> frustum_normals;
};
