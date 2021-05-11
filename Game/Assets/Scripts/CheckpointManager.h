#pragma once

#include "Scripting/Script.h"

#define N_CHECKPOINTS 4

extern int checkpoint;

class CheckpointManager : public Script {
	GENERATE_BODY(CheckpointManager);

public:

	void Start() override;
	void Update() override;
public:
	UID avatarUID;
	GameObject* avatarObj;
	float3 checkpointPosition1, checkpointPosition2, checkpointPosition3, checkpointPosition4;
	float distanceThreshold, timeBetweenChecks, timeBetweenChecksCounter;

private:
	void CheckDistanceWithCheckpoints();

private:
	float3 runtimeCheckpointPositions[N_CHECKPOINTS];

};

