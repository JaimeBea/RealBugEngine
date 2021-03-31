#pragma once
#include "ComponentSelectable.h"
#include "UI/Interfaces/IMouseClickHandler.h"

#include "Math/float4.h"

class GameObject;

// Component UI Button that allows user interaction by clicking/pressing the button. Inherits ComponentSelectable and implements IMouseClickHandler.
class ComponentButton : public ComponentSelectable 
					  , public IMouseClickHandler {
public:
	REGISTER_COMPONENT_SELECTABLE(ComponentButton, ComponentType::BUTTON, false);
	void Init() override;											// Inits the component
	void OnEditorUpdate() override;									// Works as input of the Colors and the Selectable interface
	
	void OnClicked() override;										// Action to perform when the button is clicked
	void Update() override;											// Updates clicked variable if the button is Left-clicked mouse
	void DuplicateComponent(GameObject& obj) override;				// TODO
	void OnPointerEnter() override;									// Action to perform when the pointer enters
	void OnPointerExit() override;									// Action to perform when the pointer exits

	bool IsClicked() const;											// Returns true if the button is clicked
	void SetClicked(bool b);										// Sets clicked to the value

	const float4 GetHoverColor() const;								// Returns colorHovered
	const float4 GetClickColor() const;								// Returns colorClicked
	const float4 GetDisableColor() const;							// Returns colorDisabled
	const float4 GetTintColor() const;								// Returns the correspondant color of the current state

private:
	bool clicked = false;											// Clicked state

	float4 colorHovered = float4(0.84, 0.84, 0.84, 1);				// The color when the button is hovered
	float4 colorClicked = float4(0.64, 0.64, 0.64, 1);				// The color when the button is clicked
	float4 colorDisabled = float4(0.73, 0.73, 0.73, 1);				// The color when the button is disabled
};
