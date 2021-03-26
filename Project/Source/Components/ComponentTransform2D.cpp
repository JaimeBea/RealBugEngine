#include "ComponentTransform2D.h"
#include "Application.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleDebugDraw.h"

#include "Resources/GameObject.h"
#include "ComponentBoundingBox2D.h"

#include "debugdraw.h"
#include "imgui.h"
#include "Math/TransformOps.h"

void ComponentTransform2D::Update() {
	//dd::plane(vec(0,0,0))
}

void ComponentTransform2D::OnEditorUpdate() {
	if (ImGui::CollapsingHeader("Rect Transform")) {
		float3 editorPos = position;
		ImGui::TextColored(App->editor->titleColor, "Transformation (X,Y,Z)");
		if (ImGui::DragFloat3("Position", editorPos.ptr(), App->editor->dragSpeed2f, -inf, inf)) {
			SetPosition(editorPos);
		}

		float2 editorSize = size;
		ImGui::TextColored(App->editor->titleColor, "Size (Width,Height)");
		if (ImGui::DragFloat2("Size", editorSize.ptr(), App->editor->dragSpeed2f, -inf, inf)) {
			SetSize(editorSize);
		}

		float2 anchX = anchorX;
		float2 anchY = anchorY;
		ImGui::TextColored(App->editor->titleColor, "Anchors");
		if (ImGui::DragFloat2("Anchor X (Min, Max)", anchX.ptr(), App->editor->dragSpeed2f, 0, 1)) {
			SetAnchorX(anchX);
		}
		if (ImGui::DragFloat2("Anchor Y (Min, Max)", anchY.ptr(), App->editor->dragSpeed2f, 0, 1)) {
			SetAnchorY(anchY);
		}

		float3 scl = scale;
		if (ImGui::DragFloat3("Scale", scl.ptr(), App->editor->dragSpeed2f, 0, inf)) {
			SetScale(scl);
		}

		float3 rot = rotation.ToEulerXYZ() * RADTODEG;
		if (ImGui::DragFloat3("Rotation", rot.ptr(), App->editor->dragSpeed2f, -inf, inf)) {
			SetRotation(Quat::FromEulerXYZ(rot.x * DEGTORAD, rot.y * DEGTORAD, rot.z * DEGTORAD));
		}

		ImGui::Separator();
	}
}

void ComponentTransform2D::SetPosition(float3 position_) {
	position = position_;

	InvalidateHierarchy();
	for (Component* component : GetOwner().components) {
		component->OnTransformUpdate();
	}
}

void ComponentTransform2D::SetSize(float2 size_) {
	size = size_;

	InvalidateHierarchy();
	for (Component* component : GetOwner().components) {
		component->OnTransformUpdate();
	}
}

void ComponentTransform2D::SetRotation(Quat rotation_) {
	InvalidateHierarchy();
	for (Component* component : GetOwner().components) {
		component->OnTransformUpdate();
	}

	rotation = rotation_;

	InvalidateHierarchy();
	for (Component* component : GetOwner().components) {
		component->OnTransformUpdate();
	}
}

void ComponentTransform2D::SetScale(float3 scale_) {
	scale = scale_;
	InvalidateHierarchy();
	for (Component* component : GetOwner().components) {
		component->OnTransformUpdate();
	}
}

void ComponentTransform2D::SetAnchorX(float2 anchorX_) {
	anchorX = anchorX_;
	InvalidateHierarchy();
	for (Component* component : GetOwner().components) {
		component->OnTransformUpdate();
	}
}

void ComponentTransform2D::SetAnchorY(float2 anchorY_) {
	anchorY = anchorY_;
	InvalidateHierarchy();
	for (Component* component : GetOwner().components) {
		component->OnTransformUpdate();
	}
}

const float4x4 ComponentTransform2D::GetGlobalMatrix() {
	return float4x4::FromTRS(position, rotation, vec(scale.x * size.x, scale.y * size.y, 0));
}

float3 ComponentTransform2D::GetPosition() const {
	return position;
}

float2 ComponentTransform2D::GetSize() const {
	return size;
}

float3 ComponentTransform2D::GetScale() const {
	return scale;
}

void ComponentTransform2D::InvalidateHierarchy() {
	Invalidate();

	for (GameObject* child : GetOwner().GetChildren()) {
		ComponentTransform2D* childTransform = child->GetComponent<ComponentTransform2D>();
		if (childTransform != nullptr) {
			childTransform->Invalidate();
		}
	}
}

void ComponentTransform2D::Invalidate() {
	dirty = true;
	ComponentBoundingBox2D* boundingBox = GetOwner().GetComponent<ComponentBoundingBox2D>();
	if (boundingBox) boundingBox->Invalidate();
}
