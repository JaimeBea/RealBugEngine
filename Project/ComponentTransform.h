#pragma once

#include "Component.h"

#include "Math/float3.h"
#include "Math/Quat.h"
#include "Math/float4x4.h"

class ComponentTransform : public Component
{
public:
	REGISTER_COMPONENT(ComponentType::TRANSFORM);

	ComponentTransform(GameObject& owner);

	void OnEditorUpdate() override;

	void InvalidateHierarchy();
	void Invalidate();

	void SetPosition(float3 position);
	void SetRotation(Quat rotation);
	void SetScale(float3 scale);
	void CalculateGlobalMatrix(bool force = false);
	void UpdateTransform();

	float3 GetPosition() const;
	Quat GetRotation() const;
	float3 GetScale() const;
	const float4x4& GetGlobalMatrix() const;

private:
	float3 position = float3(0, 0, 0);
	Quat rotation = Quat::identity;
	float3 scale = float3(1, 1, 1);

	float4x4 local_matrix = float4x4::identity;
	float4x4 global_matrix = float4x4::identity;
	bool dirty = true;
};
