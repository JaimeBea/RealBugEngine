#pragma once

#include "Component.h"

#include "Math/float3.h"
#include "Math/Quat.h"
#include "Math/float4x4.h"
#include "imgui.h"
#include "ImGuizmo.h"

class ComponentTransform : public Component
{
public:
	REGISTER_COMPONENT(ComponentTransform, ComponentType::TRANSFORM);

	void Update() override;
	void OnEditorUpdate() override;
	void Save(JsonValue j_component) const override;
	void Load(JsonValue j_component) override;

	void InvalidateHierarchy();
	void Invalidate();

	void SetPosition(float3 position);
	void SetRotation(Quat rotation);
	void SetRotation(float3 rotation);
	void SetScale(float3 scale);
	void CalculateGlobalMatrix(bool force = false);

	float3 GetPosition() const;
	Quat GetRotation() const;
	float3 GetScale() const;
	const float4x4& GetLocalMatrix() const;
	const float4x4& GetGlobalMatrix() const;

	ImGuizmo::OPERATION GetGizmoOperation() const;
	ImGuizmo::MODE GetGizmoMode() const;
	bool GetUseSnap() const;
	float3 GetSnap();

	bool GetDirty() const;

private:
	//ImGuizmo
	ImGuizmo::OPERATION current_guizmo_operation = ImGuizmo::TRANSLATE;
	ImGuizmo::MODE current_guizmo_mode = ImGuizmo::WORLD;

	float3 position = float3::zero;
	Quat rotation = Quat::identity;
	float3 scale = float3::one;

	float3 local_euler_angles = float3::zero;

	bool dirty = true;
	float4x4 local_matrix = float4x4::identity;
	float4x4 global_matrix = float4x4::identity;

	bool use_snap = false;
	float snap[3] = {1.f, 1.f, 1.f};
};
