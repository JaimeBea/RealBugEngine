#pragma once

#include "Script.h"

class GameObject;
class ComponentText;

class StatsDisplayer : public Script
{
	GENERATE_BODY(StatsDisplayer);

public:

	void Start() override;
	void Update() override;

private:
	GameObject* canvas = nullptr;
	GameObject* player = nullptr;

	ComponentText* fpsText = nullptr;
	ComponentText* trigText = nullptr;
	ComponentText* positionText = nullptr;
	ComponentText* cameraText = nullptr;

	bool panelActive = false;
};

