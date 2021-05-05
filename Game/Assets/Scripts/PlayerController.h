#pragma once

#include "Script.h"
#include "Math/float3.h"

class GameObject;
class ComponentTransform;

class PlayerController : public Script
{
	GENERATE_BODY(PlayerController);

public:

	void Start() override;
	void Update() override;

public:
	GameObject* gameObject = nullptr;
	GameObject* camera = nullptr;
	enum MovementDirection {
		NONE = 0,	UP = 1, LEFT = 2, DOWN = 3, RIGHT = 4
	};

private:	
	float dashSpeed = 100.f;
	float dashDistance = 20.f;
	float dashError = 2.f;

	float dashCooldown = 5.f; //seconds
	float dashCooldownRemaing = 0.f;
	bool  dashInCooldown = true;

	float offset = 0.f;
	float movementSpeed = 5.f;
	float rotationSpeed = 2.f;
	bool dashing = false;
	//float cameraXPosition = -163.f;
	float3 initialPosition = float3(0,0,0);
	float3 dashDestination = float3(0,0,0);
	float3 dashDirection = float3(0,0,0);
	ComponentTransform* transform = nullptr;
	MovementDirection dashMovementDirection = MovementDirection::NONE;

private:
	void MoveTo(MovementDirection md);
	void InitDash();
	void Dash();
	bool const CanDash() const;
	void CheckCoolDowns();
};

