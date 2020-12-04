#pragma once

#include "Component.h"

#include "Geometry/Sphere.h"

class ComponentBoundingSphere : public Component
{
public:
	static const ComponentType static_type = ComponentType::BOUNDING_SPHERE;

	ComponentBoundingSphere(GameObject& owner);

public:
	Sphere bounding_sphere = Sphere(vec(0, 0, 0), 0);
};

