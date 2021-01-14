#pragma once

#include "Component.h"

#include "Geometry/AABB.h"
#include "Geometry/OBB.h"

class ComponentBoundingBox : public Component
{
public:
	REGISTER_COMPONENT(ComponentBoundingBox, ComponentType::BOUNDING_BOX);

	void Update() override;
	void Save(JsonValue& j_component) const override;
	void Load(const JsonValue& j_component) override;

	void SetLocalBoundingBox(const AABB& bounding_box);
	void CalculateWorldBoundingBox(bool force = false);
	void DrawBoundingBox();
	void Invalidate();

	const OBB& GetWorldOBB() const;
	const AABB& GetWorldAABB() const;

private:
	AABB local_aabb = {{0, 0, 0}, {0, 0, 0}};

	bool dirty = true;
	AABB world_aabb = {{0, 0, 0}, {0, 0, 0}};
	OBB world_obb = {world_aabb};
};
