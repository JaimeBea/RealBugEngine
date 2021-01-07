#include "ComponentBoundingBox.h"

#include "Logging.h"
#include "GameObject.h"
#include "ComponentTransform.h"

#include "Geometry/OBB.h"

#include "Leaks.h"

void ComponentBoundingBox::Save(JsonValue& j_component) const
{
	JsonValue& j_local_bounding_box = j_component["LocalBoundingBox"];
	j_local_bounding_box[0] = local_bounding_box.minPoint.x;
	j_local_bounding_box[1] = local_bounding_box.minPoint.y;
	j_local_bounding_box[2] = local_bounding_box.minPoint.z;
	j_local_bounding_box[3] = local_bounding_box.maxPoint.x;
	j_local_bounding_box[4] = local_bounding_box.maxPoint.y;
	j_local_bounding_box[5] = local_bounding_box.maxPoint.z;
}

void ComponentBoundingBox::Load(const JsonValue& j_component)
{
	const JsonValue& j_local_bounding_box = j_component["LocalBoundingBox"];
	local_bounding_box.minPoint.Set(j_local_bounding_box[0], j_local_bounding_box[1], j_local_bounding_box[2]);
	local_bounding_box.maxPoint.Set(j_local_bounding_box[3], j_local_bounding_box[4], j_local_bounding_box[5]);

	dirty = true;
}

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
