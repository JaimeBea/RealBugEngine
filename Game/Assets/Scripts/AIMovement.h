#pragma once

#include "Scripting/Script.h"

class ComponentAnimation;

enum class AIState {
	SPAWN,
	IDLE,
	RUN,
	HURT,
	ATTACK,
	DEATH
};

class AIMovement : public Script
{
	GENERATE_BODY(AIMovement);

public:

	void Start() override;
	void Update() override;

private:
	bool CharacterInSight(const GameObject* character);
	bool CharacterInMeleeRange(const GameObject* character);
	void Seek(const float3& newPosition);
	bool HitDetected();

public:

	GameObject* fang = nullptr;
	GameObject* onimaru = nullptr;
	GameObject* currentTarget = nullptr;

private:

	int maxSpeed = 8;
	float3 velocity = float3(0, 0, 0);
	const float searchRadius = 50.f;
	const float meleeRange = 5.f;
	AIState state = AIState::SPAWN;
	int lifePoints = 3;
	bool hitTaken = false;
	ComponentAnimation* animation = nullptr;

};

