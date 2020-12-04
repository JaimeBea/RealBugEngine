#pragma once

#include "Component.h"

class ComponentMaterial : public Component
{
public:
	static const ComponentType static_type = ComponentType::MATERIAL;

	ComponentMaterial(GameObject& owner);

public:
	unsigned texture = 0;
};

