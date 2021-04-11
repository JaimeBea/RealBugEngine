#pragma once

#include "Script.h"

#include "Geometry/Frustum.h"

class GameObject;

class GodModeCamera : public Script
{
	GENERATE_BODY(GodModeCamera);

public:

	void Start() override;
	void Update() override;

	void Rotate(const float3x3& rotationMatrix);

public:
	GameObject* godCamera = nullptr;
	float speed = 0.f;
	float rotationSpeed = 0.f;

private:
	Frustum* frustum;
};

