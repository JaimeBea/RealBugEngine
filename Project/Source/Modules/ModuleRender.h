#pragma once

#include "Module.h"
#include "Utils/Quadtree.h"

#include "MathGeoLibFwd.h"
#include "Math/float3.h"

class GameObject;

class ModuleRender : public Module {
public:
	// ------- Core Functions ------ //
	bool Init() override;
	UpdateStatus PreUpdate() override;
	UpdateStatus Update() override;
	UpdateStatus PostUpdate() override;
	bool CleanUp() override;

	void ViewportResized(int width, int height);

	void SetVSync(bool vsync);

	void UpdateShadingMode(const char* shadingMode);

public:
	void* context = nullptr; // SDL context.

	// - Render Buffer GL pointers - //
	unsigned renderTexture = 0;
	unsigned depthRenderbuffer = 0;
	unsigned framebuffer = 0;

	// ------- Viewport Size ------- //
	unsigned viewportWidth = 0;
	unsigned viewportHeight = 0;

	// -- Debugging Tools Toggles -- //
	bool drawQuadtree = true;
	bool drawAllBoundingBoxes = false;

	float3 ambientColor = {0.0f, 0.0f, 0.0f}; // Color of ambient Light
	float3 clearColor = {0.1f, 0.1f, 0.1f};	  // Color of the viewport between frames

private:
	void DrawQuadtreeRecursive(const Quadtree<GameObject>::Node& node, const AABB2D& aabb); // Draws the quadrtee nodes if 'drawQuadtree' is set to true.
	void DrawSceneRecursive(const Quadtree<GameObject>::Node& node, const AABB2D& aabb);	// ??
	bool CheckIfInsideFrustum(const AABB& aabb, const OBB& obb);							// ??
	void DrawGameObject(GameObject* gameObject);											// ??
	void DrawSkyBox();																		// Draws a default skybox if 'skyboxActive' is set to true.
	#if !GAME
	void DrawAnimation(const GameObject* gameObject, bool hasAnimation = false);
	#endif // !GAME
	void RenderUI();
	void SetOrtographicRender();
	void SetPerspectiveRender();
};
