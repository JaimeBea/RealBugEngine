#include "ComponentToggle.h"
#include "GameObject.h"
#include "Components/ComponentEventSystem.h"
#include "Application.h"
#include "Modules/ModuleUserInterface.h"

#include "Utils/Logging.h"

ComponentToggle ::~ComponentToggle() {}

void ComponentToggle::Init() {
}

void ComponentToggle ::OnValueChanged() {
	LOG("Toggle value changed");
}
bool ComponentToggle ::IsOn() {
	return isOn;
}
void ComponentToggle ::SetOn(bool b) {
	isOn = b;
	OnValueChanged();
}

void ComponentToggle::OnPointerEnter() {
	ComponentSelectable::OnPointerEnter();
}

void ComponentToggle::OnPointerExit() {
	ComponentSelectable::OnPointerExit();
}

void ComponentToggle::DuplicateComponent(GameObject& owner) {
	ComponentToggle* component = owner.CreateComponent<ComponentToggle>();
	//TO DO
}