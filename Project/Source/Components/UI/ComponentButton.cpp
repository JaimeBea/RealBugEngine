#include "ComponentButton.h"
#include "GameObject.h"
#include <Application.h>

ComponentButton::ComponentButton(GameObject* owner, UID componentID, bool active)
	: ComponentSelectable(owner, componentID, active) {
	type = ComponentType::SELECTABLE;
}

ComponentButton::~ComponentButton() {
}

void ComponentButton::OnPressed() {
	//TO DO
}

void DuplicateComponent(GameObject& owner) {
	ComponentButton* component = owner.CreateComponent<ComponentButton>();
	//TO DO
}