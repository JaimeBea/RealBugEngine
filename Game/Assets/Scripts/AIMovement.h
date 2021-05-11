#pragma once

#include "Scripting/Script.h"

class ComponentAnimation;
struct TesseractEvent;

enum class AIState {
	START,
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
	void ReceiveEvent(TesseractEvent& e) override;

private:
	bool CharacterInSight(const GameObject* character);
	bool CharacterInMeleeRange(const GameObject* character);
	void Seek(const float3& newPosition);
	bool HitDetected();
	

public:

	UID fangUID;
	UID onimaruUID;
	GameObject* fang = nullptr;
	GameObject* onimaru = nullptr;
	GameObject* currentTarget = nullptr;
	int maxSpeed = 8;
	float searchRadius = 50.f;
	float meleeRange = 5.f;
	int lifePoints = 3;


private:

	float3 velocity = float3(0, 0, 0);	
	AIState state = AIState::SPAWN;
	bool hitTaken = false;
	ComponentAnimation* animation = nullptr;

};

