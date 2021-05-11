#pragma once

#include "Scripting/Script.h"
#include "Math/float3.h"

class GameObject;
class ComponentTransform;
class ComponentCamera;

class PlayerController : public Script
{
	GENERATE_BODY(PlayerController);

public:

	enum class MovementDirection {
		NONE = 0, UP = 1, UP_LEFT = 2, LEFT = 3, DOWN_LEFT = 4, DOWN = 5, DOWN_RIGHT = 6, RIGHT = 7, UP_RIGHT = 8
	};

	void Start() override;
	void Update() override;

public:
	GameObject* gameObject = nullptr;
	GameObject* camera = nullptr;

	UID gameObjectUID = 0;
	UID cameraUID = 0;

private:
	void MoveTo(MovementDirection md);
	void InitDash(MovementDirection md);
	void Dash();
	void LookAtMouse();
	bool const CanDash() const;
	void CheckCoolDowns();
	float3 GetDirection(MovementDirection md) const;

private:
	float dashSpeed = 100.f;
	float dashDistance = 10.f;
	float dashError = 2.f;

	float dashCooldown = 5.f; //seconds
	float dashCooldownRemaing = 0.f;
	bool  dashInCooldown = true;

	float offset = 0.f;
	float movementSpeed = 5.f;
	float rotationSpeed = 2.f;
	bool dashing = false;

	float3 initialPosition = float3(0, 0, 0);
	float3 dashDestination = float3(0, 0, 0);
	float3 dashDirection = float3(0, 0, 0);

	ComponentTransform* transform = nullptr;
	ComponentCamera* compCamera = nullptr;
};

