#pragma once

#include "Script.h"

class GameObject;
class ComponentTransform;
class TreeFloating : public Script
{
	GENERATE_BODY(TreeFloating);

public:

	void Start() override;
	void Update() override;

public:
	GameObject* gameObject = nullptr;

private:
	float offset = 0.f;
};

