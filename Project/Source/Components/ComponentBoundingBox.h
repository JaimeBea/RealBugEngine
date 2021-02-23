#pragma once

#include "Component.h"

#include "Geometry/AABB.h"
#include "Geometry/OBB.h"

class ComponentBoundingBox : public Component {
public:
	REGISTER_COMPONENT(ComponentBoundingBox, ComponentType::BOUNDING_BOX);

	void OnTransformUpdate() override;
	void OnEditorUpdate() override;
	void Save(JsonValue jComponent) const override;
	void Load(JsonValue jComponent) override;

	void SetLocalBoundingBox(const AABB& boundingBox);
	void CalculateWorldBoundingBox(bool force = false);
	void DrawBoundingBox();
	void Invalidate();

	const OBB& GetWorldOBB() const;
	const AABB& GetWorldAABB() const;

private:
	AABB localAABB = {{0, 0, 0}, {0, 0, 0}};

	bool dirty = true;
	AABB worldAABB = {{0, 0, 0}, {0, 0, 0}};
	OBB worldOBB = {worldAABB};

	bool bbActive = false;
};
