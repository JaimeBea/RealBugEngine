#include "ComponentBoundingBox2D.h"

#include "Utils/Logging.h"
#include "GameObject.h"
#include "Components/ComponentTransform2D.h"

#include "Geometry/AABB.h"
#include "Geometry/OBB2D.h"

#include "debugdraw.h"
#include "Math/float3x3.h"
#include "Geometry/Circle.h"

#include "Application.h"
#include <Modules/ModuleInput.h>

//IF EDITOR
#include <Panels/PanelScene.h>
#include <Modules/ModuleEditor.h>
//ENDIF

#include "Utils/Leaks.h"

#define JSON_TAG_LOCAL_BOUNDING_BOX2D "LocalBoundingBox2D"

void ComponentBoundingBox2D::Init() {
	ComponentTransform2D* transform2D = owner->GetComponent<ComponentTransform2D>();
	if (transform2D) {
		float3 minPoint = float3(-0.5f, -0.5f, 0.0f);
		float3 maxPoint = float3(0.5f, 0.5f, 0.0f);

		SetLocalBoundingBox(AABB2D(minPoint.xy(), maxPoint.xy()));
		CalculateWorldBoundingBox();
	}
}

void ComponentBoundingBox2D::OnTransformUpdate() {
	CalculateWorldBoundingBox(true);
}

void ComponentBoundingBox2D::Save(JsonValue jComponent) const {
	JsonValue jLocalBoundingBox = jComponent[JSON_TAG_LOCAL_BOUNDING_BOX2D];
	jLocalBoundingBox[0] = localAABB.minPoint.x;
	jLocalBoundingBox[1] = localAABB.minPoint.y;
	jLocalBoundingBox[2] = localAABB.maxPoint.x;
	jLocalBoundingBox[3] = localAABB.maxPoint.y;
}

void ComponentBoundingBox2D::Load(JsonValue jComponent) {
	JsonValue jLocalBoundingBox = jComponent[JSON_TAG_LOCAL_BOUNDING_BOX2D];
	localAABB.minPoint.Set(jLocalBoundingBox[0], jLocalBoundingBox[1]);
	localAABB.maxPoint.Set(jLocalBoundingBox[2], jLocalBoundingBox[3]);

	dirty = true;
}

void ComponentBoundingBox2D::SetLocalBoundingBox(const AABB2D& boundingBox) {
	localAABB = boundingBox;
	dirty = true;
}

void ComponentBoundingBox2D::CalculateWorldBoundingBox(bool force) {
	if (dirty || force) {
		ComponentTransform2D* t2d = owner->GetComponent<ComponentTransform2D>();

		//worldAABB minPoint is the localAABB's min point mulitplied by the rect transform's scale and size, adding the rect transform position
		//Right now to calculate position we add half the size of the WindowScene because all textures are "centered" for their coordinates

		//IF EDITOR
		worldAABB.minPoint = t2d->GetPosition().xy() + App->editor->panelScene.GetSceneWindowSize() / 2 + localAABB.minPoint.Mul(t2d->GetSize().Mul(t2d->GetScale().xy() * 1.01f));
		worldAABB.maxPoint = t2d->GetPosition().xy() + App->editor->panelScene.GetSceneWindowSize() / 2 + localAABB.maxPoint.Mul(t2d->GetSize().Mul(t2d->GetScale().xy() * 1.01f));
		//ENDIF
	}
}

void ComponentBoundingBox2D::DrawBoundingBox() {
	dd::aabb(float3(worldAABB.minPoint, 0.0f), float3(worldAABB.maxPoint, 0.0f), float3::one);
}

void ComponentBoundingBox2D::Invalidate() {
	dirty = true;
}

const AABB2D& ComponentBoundingBox2D::GetWorldAABB() const {
	return worldAABB;
}

void ComponentBoundingBox2D::Update() {
}

void ComponentBoundingBox2D::DuplicateComponent(GameObject& owner) {
	ComponentBoundingBox2D* component = owner.CreateComponent<ComponentBoundingBox2D>();
}