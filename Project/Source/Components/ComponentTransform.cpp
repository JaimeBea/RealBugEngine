#include "ComponentTransform.h"

#include "Application.h"
#include "Resources/GameObject.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentBoundingBox.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleInput.h"
#include "Modules/ModuleCamera.h"

#include "Math/float3x3.h"
#include "SDL.h"

#include "Utils/Leaks.h"

#define JSON_TAG_POSITION "Position"
#define JSON_TAG_ROTATION "Rotation"
#define JSON_TAG_SCALE "Scale"
#define JSON_TAG_LOCAL_EULER_ANGLES "LocalEulerAngles"

void ComponentTransform::Init() {
	CalculateGlobalMatrix();
	for (Component* component : GetOwner().components) {
		component->OnTransformUpdate();
	}
}

void ComponentTransform::Update() {
	CalculateGlobalMatrix();
}

void ComponentTransform::OnEditorUpdate() {
	float3 pos = position;
	float3 scl = scale;
	float3 rot = localEulerAngles;

	if (ImGui::CollapsingHeader("Transformation")) {
		ImGui::TextColored(App->editor->titleColor, "Transformation (X,Y,Z)");
		if (ImGui::DragFloat3("Position", pos.ptr(), App->editor->dragSpeed2f, -inf, inf)) {
			SetPosition(pos);
		}
		if (ImGui::DragFloat3("Scale", scl.ptr(), App->editor->dragSpeed2f, 0, inf)) {
			SetScale(scl);
		}

		if (ImGui::DragFloat3("Rotation", rot.ptr(), App->editor->dragSpeed2f, -inf, inf)) {
			SetRotation(rot);
		}
		ImGui::Separator();
	}
}

void ComponentTransform::Save(JsonValue jComponent) const {
	JsonValue jPosition = jComponent[JSON_TAG_POSITION];
	jPosition[0] = position.x;
	jPosition[1] = position.y;
	jPosition[2] = position.z;

	JsonValue jRotation = jComponent[JSON_TAG_ROTATION];
	jRotation[0] = rotation.x;
	jRotation[1] = rotation.y;
	jRotation[2] = rotation.z;
	jRotation[3] = rotation.w;

	JsonValue jScale = jComponent[JSON_TAG_SCALE];
	jScale[0] = scale.x;
	jScale[1] = scale.y;
	jScale[2] = scale.z;

	JsonValue jLocalEulerAngles = jComponent[JSON_TAG_LOCAL_EULER_ANGLES];
	jLocalEulerAngles[0] = localEulerAngles.x;
	jLocalEulerAngles[1] = localEulerAngles.y;
	jLocalEulerAngles[2] = localEulerAngles.z;
}

void ComponentTransform::Load(JsonValue jComponent) {
	JsonValue jPosition = jComponent[JSON_TAG_POSITION];
	position.Set(jPosition[0], jPosition[1], jPosition[2]);

	JsonValue jRotation = jComponent[JSON_TAG_ROTATION];
	rotation.Set(jRotation[0], jRotation[1], jRotation[2], jRotation[3]);

	JsonValue jScale = jComponent[JSON_TAG_SCALE];
	scale.Set(jScale[0], jScale[1], jScale[2]);

	JsonValue jLocalEulerAngles = jComponent[JSON_TAG_LOCAL_EULER_ANGLES];
	localEulerAngles.Set(jLocalEulerAngles[0], jLocalEulerAngles[1], jLocalEulerAngles[2]);

	dirty = true;
}

void ComponentTransform::InvalidateHierarchy() {
	Invalidate();

	for (GameObject* child : GetOwner().GetChildren()) {
		ComponentTransform* childTransform = child->GetComponent<ComponentTransform>();
		if (childTransform != nullptr) {
			childTransform->Invalidate();
		}
	}
}

void ComponentTransform::Invalidate() {
	dirty = true;
	ComponentBoundingBox* boundingBox = GetOwner().GetComponent<ComponentBoundingBox>();
	if (boundingBox) boundingBox->Invalidate();
}

void ComponentTransform::CalculateGlobalMatrix(bool force) {
	if (force || dirty) {
		localMatrix = float4x4::FromTRS(position, rotation, scale);

		GameObject* parent = GetOwner().GetParent();
		if (parent != nullptr) {
			ComponentTransform* parentTransform = parent->GetComponent<ComponentTransform>();

			parentTransform->CalculateGlobalMatrix();
			globalMatrix = parentTransform->globalMatrix * localMatrix;
		} else {
			globalMatrix = localMatrix;
		}

		dirty = false;
	}
}

void ComponentTransform::SetPosition(float3 position_) {
	position = position_;
	InvalidateHierarchy();
	for (Component* component : GetOwner().components) {
		component->OnTransformUpdate();
	}
}

void ComponentTransform::SetRotation(Quat rotation_) {
	rotation = rotation_;
	localEulerAngles = rotation_.ToEulerXYZ().Mul(RADTODEG);
	InvalidateHierarchy();
	for (Component* component : GetOwner().components) {
		component->OnTransformUpdate();
	}
}

void ComponentTransform::SetRotation(float3 rotation_) {
	rotation = Quat::FromEulerXYZ(rotation_.x * DEGTORAD, rotation_.y * DEGTORAD, rotation_.z * DEGTORAD);
	localEulerAngles = rotation_;
	InvalidateHierarchy();
	for (Component* component : GetOwner().components) {
		component->OnTransformUpdate();
	}
}

void ComponentTransform::SetScale(float3 scale_) {
	scale = scale_;
	InvalidateHierarchy();
	for (Component* component : GetOwner().components) {
		component->OnTransformUpdate();
	}
}

float3 ComponentTransform::GetPosition() const {
	return position;
}

Quat ComponentTransform::GetRotation() const {
	return rotation;
}

float3 ComponentTransform::GetScale() const {
	return scale;
}

const float4x4& ComponentTransform::GetLocalMatrix() const {
	return localMatrix;
}

const float4x4& ComponentTransform::GetGlobalMatrix() const {
	return globalMatrix;
}

bool ComponentTransform::GetDirty() const {
	return dirty;
}