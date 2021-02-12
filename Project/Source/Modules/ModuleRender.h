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
	unsigned renderTexture = 0;
	unsigned depthRenderbuffer = 0;
	unsigned framebuffer = 0;

	unsigned viewportWidth = 0;
	unsigned viewportHeight = 0;

	float3 clearColor = {0.1f, 0.1f, 0.1f};

	bool drawQuadtree = true;
	bool drawAllBoundingBoxes = false;
	bool skyboxActive = true;
	float3 ambientColor = {0.0f, 0.0f, 0.0f};

private:
	void DrawQuadtreeRecursive(const Quadtree<GameObject>::Node& node, const AABB2D& aabb);
	void DrawSceneRecursive(const Quadtree<GameObject>::Node& node, const AABB2D& aabb);
	bool CheckIfInsideFrustum(const AABB& aabb, const OBB& obb);
	void DrawGameObject(GameObject* gameObject);
	void DrawSkyBox();
};
