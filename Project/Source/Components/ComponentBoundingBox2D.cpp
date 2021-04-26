#include "ComponentBoundingBox2D.h"

#include "Application.h"
#include "Modules/ModuleInput.h"
#include "Components/UI/ComponentTransform2D.h"
#include "GameObject.h"
#include "Utils/Logging.h"
#include "Panels/PanelScene.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleWindow.h"

#include "debugdraw.h"
#include "Geometry/AABB.h"
#include "Geometry/OBB2D.h"
#include "Geometry/Circle.h"
#include "Math/float3x3.h"

#include "Utils/Leaks.h"

#define JSON_TAG_LOCAL_BOUNDING_BOX2D "LocalBoundingBox2D"

void ComponentBoundingBox2D::Init() {
	ComponentTransform2D* transform2D = GetOwner().GetComponent<ComponentTransform2D>();
	if (transform2D) {
		float2 minPoint = float2(-0.5f, -0.5f);
		float2 maxPoint = float2(0.5f, 0.5f);

		SetLocalBoundingBox(AABB2D(minPoint, maxPoint));
		CalculateWorldBoundingBox();
	}
}

void ComponentBoundingBox2D::Update() {
	CalculateWorldBoundingBox();
}

void ComponentBoundingBox2D::DuplicateComponent(GameObject& owner) {
	ComponentBoundingBox2D* component = owner.CreateComponent<ComponentBoundingBox2D>();
	component->SetLocalBoundingBox(AABB2D(localAABB));
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
		ComponentTransform2D* transform2d = GetOwner().GetComponent<ComponentTransform2D>();

		//worldAABB minPoint is the localAABB's min point mulitplied by the rect transform's scale and size, adding the rect transform position
		//Right now to calculate position we add half the size of the WindowScene because all textures are "centered" for their coordinates

#if !GAME
		worldAABB.minPoint = transform2d->GetPosition().xy().Mul(float2(1.0f, -1.0f)) + App->editor->panelScene.GetSceneWindowSize() / 2.0f + 
			localAABB.minPoint.Mul(transform2d->GetSize().Mul(transform2d->GetScale().xy()));
		worldAABB.maxPoint = transform2d->GetPosition().xy().Mul(float2(1.0f, -1.0f)) + App->editor->panelScene.GetSceneWindowSize() / 2.0f + 
			localAABB.maxPoint.Mul(transform2d->GetSize().Mul(transform2d->GetScale().xy()));
#else
		float2 windowPos = float2(App->window->GetPositionX(), App->window->GetPositionY());
		worldAABB.minPoint = windowPos + transform2d->GetPosition().xy().Mul(float2(1.0f, -1.0f)) + 
			float2(App->window->GetWidth(), App->window->GetHeight()) / 2.0f + localAABB.minPoint.Mul(transform2d->GetSize().Mul(transform2d->GetScale().xy()));
		worldAABB.maxPoint = windowPos + transform2d->GetPosition().xy().Mul(float2(1.0f, -1.0f)) + 
			float2(App->window->GetWidth(), App->window->GetHeight()) / 2.0f + localAABB.maxPoint.Mul(transform2d->GetSize().Mul(transform2d->GetScale().xy()));
#endif
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