#pragma once

#include "Components/Component.h"
#include "Components/UI/ComponentTransform2D.h"

enum class DirectionType {
	LEFT_TO_RIGHT,
	RIGHT_TO_LEFT,
	UP_TO_DOWN,
	DOWN_TO_UP
};

class ComponentSlider : public Component {
public:
	REGISTER_COMPONENT(ComponentSlider, ComponentType::SLIDER, false);

	~ComponentSlider();

	void Init() override;										// Component Initialization
	void Update() override;
	void OnEditorUpdate() override;								// Input for class variables

	void Save(JsonValue jComponent) const override;				// Serializes the component
	void Load(JsonValue jComponent) override;					// Deserializes the component
	void DuplicateComponent(GameObject& owner) override;		// Duplicates the component

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

	DirectionType direction = DirectionType::LEFT_TO_RIGHT;		// Slider direction
};
