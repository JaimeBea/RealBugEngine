#include "ComponentButton.h"
#include "GameObject.h"
#include <Application.h>
#include <Utils/Logging.h>
//
//ComponentButton::ComponentButton(GameObject* owner, UID componentID, bool active)
//	: ComponentSelectable(owner, componentID, active) {
//	type = ComponentType::BUTTON;
//}

ComponentButton::~ComponentButton() {
}

void ComponentButton::Init() {
	SelectableInit(&GetOwner());
}

void ComponentButton::OnPressed() {
	//TO DO
}
//
void ComponentButton::OnPointerEnter() {
	ComponentEventSystem::currentEvSys->SetSelected(this);
}

void ComponentButton::OnPointerExit() {
}

void ComponentButton::DuplicateComponent(GameObject& owner) {
	ComponentButton* component = owner.CreateComponent<ComponentButton>();
	//TO DO
}