#pragma once
#include "Selectable.h"
class GameObject;
class Button : public Selectable {
public:
	//REGISTER_COMPONENT(Button, ComponentType::BUTTON, false);

	Button(GameObject*, UID componentID_, bool active);
	~Button();
	void OnPressed();
	//void DuplicateComponent(GameObject& owner) override;
};
