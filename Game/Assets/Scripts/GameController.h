#pragma once

#include "Script.h"

class GameObject;
class GameController : public Script
{
	GENERATE_BODY(GameController);

public:

	void Start() override;
	void Update() override;

public:
	GameObject* gameCamera = nullptr;
	GameObject* godCamera = nullptr;

private:
	bool godCameraActive = false;
	bool godModeAvailable = false;
};

