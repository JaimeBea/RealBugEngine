#include "ComponentBoundingBox.h"

#include "GameObject.h"
#include "ComponentTransform.h"

#include "Geometry/OBB.h"

ComponentBoundingBox::ComponentBoundingBox(GameObject& owner) : Component(static_type, owner) {}

void ComponentBoundingBox::SetLocalBoundingBox(const AABB& bounding_box)
{
	local_bounding_box = bounding_box;
	dirty = true;
}

void ComponentBoundingBox::CalculateWorldBoundingBox(bool force)
{
	if (dirty || force)
	{
		const GameObject& owner = GetOwner();
		ComponentTransform* transform = owner.GetComponent<ComponentTransform>();

		world_bounding_box = local_bounding_box.Transform(transform->GetGlobalMatrix()).MinimalEnclosingAABB();
		dirty = false;
	}
}

const AABB& ComponentBoundingBox::GetWorldBoundingBox() const
{
	return world_bounding_box;
}
