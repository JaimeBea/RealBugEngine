#pragma once

#include "Script.h"

#include "Geometry/Frustum.h"

class GameObject;
class ComponentTransform;
class GameController : public Script
{
	GENERATE_BODY(GameController);

public:

	void Start() override;
	void Update() override;

	void Rotate(float2 mouseMotion);

public:
	GameObject* gameCamera = nullptr;
	GameObject* godCamera = nullptr;
	float speed = 0.f;
	float rotationSpeedX = 0.f;
	float rotationSpeedY = 0.f;
	float focusDistance = 0.f;

private:
	Frustum* frustum = nullptr;
	ComponentTransform* transform = nullptr;
	float yaw = 0.f;
	float pitch = 0.f;
	bool showWireframe = false;
	bool godCameraActive = false;
	bool godModeAvailable = false;
};

