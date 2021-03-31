#include "ComponentButton.h"
#include "GameObject.h"
#include <Application.h>
#include <Utils/Logging.h>

void ComponentButton::Init() {
	ComponentSelectable::Init();
}

void ComponentButton::OnClicked() {
	LOG("I was clicked");
	//TO DO
}

void ComponentButton::OnPointerEnter() {
	ComponentSelectable::OnPointerEnter();
	ComponentEventSystem::currentEvSys->SetSelected(this);
}

void ComponentButton::OnPointerExit() {
	ComponentSelectable::OnPointerExit();
}

void ComponentButton::DuplicateComponent(GameObject& owner) {
	ComponentButton* component = owner.CreateComponent<ComponentButton>();
	//TO DO
}