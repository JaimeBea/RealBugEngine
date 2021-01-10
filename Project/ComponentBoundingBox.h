#pragma once

#include "Component.h"

#include "Geometry/AABB.h"
#include "Geometry/OBB.h"

class ComponentBoundingBox : public Component
{
public:
	REGISTER_COMPONENT(ComponentBoundingBox, ComponentType::BOUNDING_BOX);

	void Save(JsonValue& j_component) const override;
	void Load(const JsonValue& j_component) override;

	void SetLocalBoundingBox(const AABB& bounding_box);
	void CalculateWorldBoundingBox(bool force = false);
	void DrawBoundingBox();
	void Invalidate();

	const OBB& GetOBBWorldBoundingBox() const;
	const AABB& GetAABBWorldBoundingBox() const;

private:
	bool dirty = true;

	AABB local_bounding_box_aabb = AABB(vec(0, 0, 0), vec(0, 0, 0));
	AABB world_bounding_box_aabb = AABB(vec(0, 0, 0), vec(0, 0, 0));
	OBB world_bounding_box_obb = OBB(world_bounding_box_aabb);
};
