#pragma once

#include "Component.h"

#include "Geometry/AABB.h"

class ComponentBoundingBox : public Component
{
public:
	REGISTER_COMPONENT(ComponentBoundingBox, ComponentType::BOUNDING_BOX);

	void SetLocalBoundingBox(const AABB& bounding_box);
	void CalculateWorldBoundingBox(bool force = false);

	const AABB& GetWorldBoundingBox() const;

private:
	bool dirty = true;
	AABB local_bounding_box = AABB(vec(0, 0, 0), vec(0, 0, 0));
	AABB world_bounding_box = AABB(vec(0, 0, 0), vec(0, 0, 0));
};
