#pragma once
#include "ComponentSelectable.h"
class GameObject;
class ComponentButton : public Component
	, ComponentSelectable {
public:
	REGISTER_COMPONENT(ComponentButton, ComponentType::BUTTON, false);
	void Init() override;
	~ComponentButton();

	void OnPressed();

	void DuplicateComponent(GameObject& obj) override;
	void OnPointerEnter() override;
	void OnPointerExit() override;
};
