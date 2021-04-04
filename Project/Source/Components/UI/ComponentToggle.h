#pragma once

//#include "Components/Component.h"
#include "Components/UI/ComponentSelectable.h"

class ComponentToggle : public ComponentSelectable {
public:
	REGISTER_COMPONENT_SELECTABLE(ComponentToggle, ComponentType::TOGGLE, false);

	void Init() override;

	~ComponentToggle();
	void OnValueChanged();
	bool IsOn();
	void SetOn(bool b);
	void DuplicateComponent(GameObject& obj) override;
	void OnPointerEnter() override;
	void OnPointerExit() override;

private:
	bool isOn = false;
};
