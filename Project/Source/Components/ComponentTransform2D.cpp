#include "ComponentTransform2D.h"
#include "Application.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleDebugDraw.h"
#include "debugdraw.h"
#include "imgui.h"

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

		float3 rot = rotation;
		if (ImGui::DragFloat3("Rotation", rot.ptr(), App->editor->dragSpeed2f, -inf, inf)) {
			SetRotation(rot);
		}

		ImGui::Separator();
	}
}

void ComponentTransform2D::SetPosition(float3 position_) {
	position = position_;
}

void ComponentTransform2D::SetSize(float2 size_) {
	size = size_;
}

void ComponentTransform2D::SetRotation(float3 rotation_) {
	rotation = rotation_;
}

void ComponentTransform2D::SetScale(float3 scale_) {
	scale = scale_;
}

void ComponentTransform2D::SetAnchorX(float2 anchorX_) {
	anchorX = anchorX_;
}

void ComponentTransform2D::SetAnchorY(float2 anchorY_) {
	anchorY = anchorY_;
}
