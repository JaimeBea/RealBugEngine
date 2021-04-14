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

	void Rotate(float2 mouseMotion, Frustum* frustum, ComponentTransform* transform);

public:
	GameObject* gameCamera = nullptr;
	GameObject* godCamera = nullptr;
	GameObject* staticCamera1 = nullptr;
	GameObject* staticCamera2 = nullptr;
	GameObject* staticCamera3 = nullptr;
	GameObject* staticCamera4 = nullptr;
	
	GameObject* player = nullptr;
	 
	float speed = 0.f;
	float rotationSpeedX = 0.f;
	float rotationSpeedY = 0.f;
	float focusDistance = 0.f;
	float transitionSpeed = 1000.f;

private:
	void DoTransition();

private:
	float yaw = 0.f;
	float pitch = 0.f;
	bool showWireframe = false;
	bool godCameraActive = false;
	bool godModeAvailable = false;
	bool transitionFinished = false;

};

