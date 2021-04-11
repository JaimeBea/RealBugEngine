#pragma once

#include "Script.h"

class GameObject;

class aaaa : public Script
{
	GENERATE_BODY(aaaa);

public:

	void Start() override;
	void Update() override;

public:

	GameObject* gameObject = nullptr;
};
