#pragma once

#include "Components/Component.h"
#include "Components/UI/ComponentTransform2D.h"
#include "UI/Interfaces/IMouseClickHandler.h"

enum class DirectionType {
	LEFT_TO_RIGHT,
	RIGHT_TO_LEFT,
	UP_TO_DOWN,
	DOWN_TO_UP
};

class ComponentSlider : public Component
	, public IMouseClickHandler {
public:
	REGISTER_COMPONENT(ComponentSlider, ComponentType::SLIDER, false);

	~ComponentSlider();

	void Init() override;										// Component Initialization
	void Update() override;
	void OnEditorUpdate() override;								// Input for class variables

	void SetValue(float value);

	void OnClicked() override;

	void Save(JsonValue jComponent) const override;				// Serializes the component
	void Load(JsonValue jComponent) override;					// Deserializes the component
	void DuplicateComponent(GameObject& owner) override;		// Duplicates the component

	bool IsClicked() const;										// Returns true if the button is clicked
	void SetClicked(bool clicked_);								// Sets clicked to the value

	const float4& GetTintColor() const;							// Returns the correspondant color of the current state
	const float4& GetClickColor() const;						// Returns colorClicked

private:

	void SetDefaultSliderSize();
	void SetNormalizedValue();

private:
	GameObject* background = nullptr;						
	GameObject* fill = nullptr;
	GameObject* handle = nullptr;

	float maxValue = 100.0f;
	float minValue = 0.0f;

	float currentValue = 50.0f;									// Current value between maxValue and minValue
	float normalizedValue = 0.5f;								// Normalized current value between 0 and 1

	bool clicked = false;										// Clicked state
	float4 colorClicked = float4(0.64f, 0.64f, 0.64f, 1.f);		// The color when the button is clicked

	DirectionType direction = DirectionType::LEFT_TO_RIGHT;		// Slider direction
};
