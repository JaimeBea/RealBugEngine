#pragma once
#include "ComponentSelectable.h"
#include "UI/Interfaces/IMouseClickHandler.h"

#include "Math/float4.h"

class GameObject;
class ComponentButton : public ComponentSelectable //Class that allows user interaction by clicking/pressing
					  , public IMouseClickHandler {
public:
	REGISTER_COMPONENT_SELECTABLE(ComponentButton, ComponentType::BUTTON, false);
	void Init() override;
	void OnEditorUpdate() override;
	
	void OnClicked() override;
	void Update() override;
	void DuplicateComponent(GameObject& obj) override;
	void OnPointerEnter() override;
	void OnPointerExit() override;

	bool GetClicked() const;
	void SetClicked(bool b);

	const float4 GetHoverColor() const;
	const float4 GetClickColor() const;
	const float4 GetDisableColor() const;
	const float4 GetTintColor() const;

private:
	bool clicked = false;

	float4 colorHovered = float4::one;
	float4 colorClicked = float4::one;
	float4 colorDisabled = float4::one;
};
