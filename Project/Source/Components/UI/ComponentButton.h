#pragma once
#include "ComponentSelectable.h"
class GameObject;
class ComponentButton : public ComponentSelectable {
public:
	//REGISTER_COMPONENT(Button, ComponentType::BUTTON, false);

	ComponentButton(GameObject*, UID componentID_, bool active);
	~ComponentButton();
	void OnPressed();
	//void DuplicateComponent(GameObject& owner) override;
};
