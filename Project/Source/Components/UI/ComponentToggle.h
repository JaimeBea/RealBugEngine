#pragma once

#include "Components/Component.h"

#define JSON_TAG_IS_ON "IsOn"

class ComponentToggle : public Component {
public:
	REGISTER_COMPONENT(ComponentToggle, ComponentType::TOGGLE, false);

	~ComponentToggle();

	void Init() override;
	void DuplicateComponent(GameObject& obj) override;
	void Save(JsonValue jComponent) const override;
	void Load(JsonValue jComponent) override;

	void OnValueChanged();
	bool IsOn();
	void SetOn(bool b);

private:
	bool isOn = false;
};
