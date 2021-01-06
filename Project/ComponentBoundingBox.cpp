#include "ComponentBoundingBox.h"

#include "GameObject.h"
#include "ComponentTransform.h"

#include "debugdraw.h"
#include "Math/float3x3.h"

ComponentBoundingBox::ComponentBoundingBox(GameObject& owner)
	: Component(static_type, owner) {}

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
