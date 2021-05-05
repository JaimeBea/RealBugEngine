#pragma once

#include "Scripting/Script.h"

class AIMovement : public Script
{
	GENERATE_BODY(AIMovement);

public:

	void Start() override;
	void Update() override;

private:
	bool CharacterInSight(const GameObject* character);
	void Seek(const float3& newPosition);

public:

	GameObject* fang = nullptr;
	GameObject* onimaru = nullptr;

private:

	int maxSpeed = 8;
	float3 velocity = float3::zero;
	const float searchRadius = 50.f;

};

