#pragma once

#include "Component.h"
#include "Math/float2.h"
#include "Math/float3.h"
#include "Math/float4x4.h"
#include "Math/Quat.h"

class ComponentTransform2D : public Component {
public:
	REGISTER_COMPONENT(ComponentTransform2D, ComponentType::TRANSFORM2D);
	void Update() override;
	void OnEditorUpdate() override;
	void Save(JsonValue jComponent) const override;
	void Load(JsonValue jComponent) override;
	

	void SetPosition(float3 position);
	void SetSize(float2 size);
	void SetRotation(Quat rotation);
	void SetRotation(float3 rotation);
	void SetScale(float3 scale);
	void SetAnchorX(float2 anchorX);
	void SetAnchorY(float2 anchorY);
	const float4x4 GetGlobalMatrix();
	const float4x4 GetGlobalMatrixWithSize();

private:
	float2 pivot	= float2(0.5, 0.5);		// The position of the pivot
	float2 size		= float2::one;			// The size of the item

	float3 position = float3::zero;			// The offset position
	Quat rotation	= Quat::identity;		// The rotation of the element in Quaternion
	float3 localEulerAngles = float3::zero;	// The rotation of the element in Euler
	float3 scale	= float3::one;			// The scale of the element

	float2 anchorX	= float2::zero;			// The Anchor of X axis. AnchorX.x -> Min position, AnchorX.y -> Max position
	float2 anchorY	= float2::zero;			// The Anchor of Y axis. AnchorY.x -> Min position, AnchorY.y -> Max position

	float4x4 localMatrix = float4x4::identity;
	float4x4 globalMatrix = float4x4::identity;

	void CalculateGlobalMatrix();
};
