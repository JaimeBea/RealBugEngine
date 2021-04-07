#pragma once

#include "Components/Component.h"

class ComponentToggle : public Component {
public:
	REGISTER_COMPONENT(ComponentToggle, ComponentType::TOGGLE, false);

	void Init() override;

	~ComponentToggle();
	void OnValueChanged();
	bool IsOn();
	void SetOn(bool b);
	void DuplicateComponent(GameObject& obj) override;

private:
	bool isOn = false;
};
