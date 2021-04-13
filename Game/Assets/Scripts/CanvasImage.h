#pragma once

#include "Script.h"

class GameObject;

class CanvasImage : public Script
{
	GENERATE_BODY(CanvasImage);

public:

	void Start() override;
	void Update() override;
public:
	GameObject* gameObject = nullptr;
};
