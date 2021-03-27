#include "Button.h"
#include "GameObject.h"
#include <Application.h>

Button::Button(GameObject* owner, UID componentID, bool active)
	: Selectable(owner, componentID, active) {
	type = ComponentType::SELECTABLE;
}

Button::~Button() {
}

void Button::OnPressed() {
	//TO DO
}

void DuplicateComponent(GameObject& owner) {
	Button* component = owner.CreateComponent<Button>();
	//TO DO
}