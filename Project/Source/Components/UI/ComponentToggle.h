#pragma once

#include "Components/Component.h"
#include "UI/Interfaces/IMouseClickHandler.h"
#include "Math/float4.h"

class ComponentImage;

class ComponentToggle : public Component
	, public IMouseClickHandler {
public:
	REGISTER_COMPONENT(ComponentToggle, ComponentType::TOGGLE, false);

	~ComponentToggle();

	void Init() override;
	void DuplicateComponent(GameObject& obj) override;
	void OnClicked() override; // Action to perform when the button is clicked
	void Save(JsonValue jComponent) const override;
	void Load(JsonValue jComponent) override;
	void OnEditorUpdate() override;

	void OnValueChanged();
	bool IsChecked() const;
	void SetChecked(bool b);
	ComponentImage* GetEnabledImage() const;
	void SetEnabledImageObj(UID enabledImageObjID_);

	bool IsClicked() const;
	void SetClicked(bool clicked_);

	float4 GetTintColor() const;  // Returns the correspondant color of the current state
	float4 GetClickColor() const; // Returns colorClicked

private:
	bool isChecked = true;
	UID enabledImageObjectID = 0;

	bool clicked = false;									//Is currently being clicked (Not saved nor stored)
	float4 colorClicked = float4(0.64f, 0.64f, 0.64f, 1.f); // The color when the button is clicked
};
