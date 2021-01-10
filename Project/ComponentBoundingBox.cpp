#include "ComponentBoundingBox.h"

#include "Logging.h"
#include "GameObject.h"
#include "ComponentTransform.h"

#include "debugdraw.h"
#include "Math/float3x3.h"

#include "Leaks.h"

void ComponentBoundingBox::Save(JsonValue& j_component) const
{
	JsonValue& j_local_bounding_box = j_component["LocalBoundingBox"];
	j_local_bounding_box[0] = local_bounding_box_aabb.minPoint.x;
	j_local_bounding_box[1] = local_bounding_box_aabb.minPoint.y;
	j_local_bounding_box[2] = local_bounding_box_aabb.minPoint.z;
	j_local_bounding_box[3] = local_bounding_box_aabb.maxPoint.x;
	j_local_bounding_box[4] = local_bounding_box_aabb.maxPoint.y;
	j_local_bounding_box[5] = local_bounding_box_aabb.maxPoint.z;
}

void ComponentBoundingBox::Load(const JsonValue& j_component)
{
	const JsonValue& j_local_bounding_box = j_component["LocalBoundingBox"];
	local_bounding_box_aabb.minPoint.Set(j_local_bounding_box[0], j_local_bounding_box[1], j_local_bounding_box[2]);
	local_bounding_box_aabb.maxPoint.Set(j_local_bounding_box[3], j_local_bounding_box[4], j_local_bounding_box[5]);

	dirty = true;
}

void ComponentBoundingBox::SetLocalBoundingBox(const AABB& bounding_box)
{
	local_bounding_box_aabb = bounding_box;
	dirty = true;
}

void ComponentBoundingBox::CalculateWorldBoundingBox(bool force)
{
	if (dirty || force)
	{
		const GameObject& owner = GetOwner();
		ComponentTransform* transform = owner.GetComponent<ComponentTransform>();
		float3x3 transformed = transform->GetGlobalMatrix().Float3x3Part();
		world_bounding_box_obb = local_bounding_box_aabb.Transform(transform->GetGlobalMatrix());
		world_bounding_box_aabb = world_bounding_box_obb.MinimalEnclosingAABB();
		dirty = false;
	}
}

void ComponentBoundingBox::DrawBoundingBox()
{
	float3 points[8];

	world_bounding_box_obb.GetCornerPoints(points);
	dd::box(points, dd::colors::White);
}

void ComponentBoundingBox::Invalidate()
{
	dirty = true;
}

const OBB& ComponentBoundingBox::GetOBBWorldBoundingBox() const
{
	return world_bounding_box_obb;
}

const AABB& ComponentBoundingBox::GetAABBWorldBoundingBox() const
{
	return world_bounding_box_aabb;
}
