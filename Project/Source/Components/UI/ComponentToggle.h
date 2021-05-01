#pragma once

#include "Components/Component.h"
#include "UI/Interfaces/IMouseClickHandler.h"

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

	void OnValueChanged();
	bool IsOn() const;
	void SetOn(bool b);
	ComponentImage* GetEnabledImage() const;
	void SetEnabledImage(ComponentImage* enabledImage_);

private:
	bool isOn = false;
	UID enabledImageID = 0;
};
