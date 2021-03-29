#include "ComponentButton.h"
#include "GameObject.h"
#include <Application.h>
#include <Utils/Logging.h>

void ComponentButton::Init() {

}

void ComponentButton::OnPressed() {
	//TO DO
}

void ComponentButton::OnPointerEnter() {
	ComponentEventSystem::currentEvSys->SetSelected(this);
}

void ComponentButton::OnPointerExit() {
}

void ComponentButton::DuplicateComponent(GameObject& owner) {
	ComponentButton* component = owner.CreateComponent<ComponentButton>();
	//TO DO
}