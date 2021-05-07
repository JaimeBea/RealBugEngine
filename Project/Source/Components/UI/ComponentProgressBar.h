#pragma once

#include "Components/Component.h"

#include "Math/float4.h"
#include "Math/float2.h"

class ComponentTransform2D;

// Component that renders an Image on a Quad
class ComponentProgressBar : public Component {
public:
	REGISTER_COMPONENT(ComponentProgressBar, ComponentType::PROGRESS_BAR, false);

	~ComponentProgressBar(); // Destructor

	void Init() override;							// Inits the component
	void Update() override;							// Update
	void OnEditorUpdate() override;					// Input for parameters
	void Save(JsonValue jComponent) const override; // Serializes object
	void Load(JsonValue jComponent) override;		// Deserializes object
	void DuplicateComponent(GameObject& owner) override;

	void SetValue(float v);
	void SetFillPos(float fillPos);
	void SetMin(float m);
	void SetMax(float n);
	void SetFillDir(bool fillDir);

public:
	float value = .5f;

private:
	GameObject* background = nullptr;
	GameObject* fill = nullptr;

	ComponentTransform2D* rectBack = nullptr;
	ComponentTransform2D* rectFill = nullptr;

	float3 backPos = float3::zero;
	float2 backSize = float2::zero;

	float fillXPos = 0.0f;
	float min = 0.0f;
	float max = 1.0f;

	bool rightToLeft = false;
};
