#pragma once

#include "Module.h"
#include "Utils/Quadtree.h"

#include "MathGeoLibFwd.h"
#include "Math/float3.h"

class GameObject;

class ModuleRender : public Module {
public:
	bool Init() override;
	UpdateStatus PreUpdate() override;
	UpdateStatus Update() override;
	UpdateStatus PostUpdate() override;
	bool CleanUp() override;

	void ViewportResized(int width, int height);

	void SetVSync(bool vsync);

public:
	void* context = nullptr;
	unsigned render_texture = 0;
	unsigned depth_renderbuffer = 0;
	unsigned framebuffer = 0;

	unsigned viewport_width = 0;
	unsigned viewport_height = 0;

	float3 clear_color = {0.1f, 0.1f, 0.1f};

	bool draw_quadtree = true;
	bool draw_all_bounding_boxes = false;
	bool skybox_active = true;
	float3 ambient_color = {0.0f, 0.0f, 0.0f};

private:
	void DrawQuadtreeRecursive(const Quadtree<GameObject>::Node& node, const AABB2D& aabb);
	void DrawSceneRecursive(const Quadtree<GameObject>::Node& node, const AABB2D& aabb);
	bool CheckIfInsideFrustum(const AABB& aabb, const OBB& obb);
	void DrawGameObject(GameObject* game_object);
	void DrawSkyBox();
};
