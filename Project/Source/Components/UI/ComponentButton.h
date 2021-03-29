#pragma once
#include "ComponentSelectable.h"
class GameObject;
class ComponentButton : public ComponentSelectable {
public:
	REGISTER_COMPONENT_SELECTABLE(ComponentButton, ComponentType::BUTTON, false);
	void Init() override;

	void OnPressed();

	void DuplicateComponent(GameObject& obj) override;
	void OnPointerEnter() override;
	void OnPointerExit() override;
};
