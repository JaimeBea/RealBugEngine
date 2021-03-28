#pragma once

#include "Component.h"

#include "Geometry/AABB2D.h"
#include "Geometry/OBB.h"

class ComponentBoundingBox2D : public Component {
public:
	REGISTER_COMPONENT(ComponentBoundingBox2D, ComponentType::BOUNDING_BOX_2D, false);

	void OnTransformUpdate() override;
	void Save(JsonValue jComponent) const override;
	void Load(JsonValue jComponent) override;
	void Init() override;

	void SetLocalBoundingBox(const AABB2D& boundingBox);
	void CalculateWorldBoundingBox(bool force = false);
	void DrawBoundingBox();
	void Invalidate();
	void Update() override;
	void DuplicateComponent(GameObject& owner) override;

	const AABB2D& GetWorldAABB() const;

private:
	bool dirty = true;
	AABB2D localAABB = {{0, 0}, {0, 0}};
	AABB2D worldAABB = {{0, 0}, {0, 0}};
};