#include "ComponentToggle.h"

#include "Utils/Logging.h"
#include "GameObject.h"
#include "Components/UI/ComponentEventSystem.h"
#include "Application.h"
#include "Modules/ModuleUserInterface.h"

#include "Utils/Leaks.h"

#define JSON_TAG_IS_ON "IsOn"

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

void ComponentToggle::DuplicateComponent(GameObject& owner) {
	ComponentToggle* component = owner.CreateComponentDeferred<ComponentToggle>();
	component->isOn = isOn;
}

void ComponentToggle::Save(JsonValue jComponent) const {
	jComponent[JSON_TAG_IS_ON] = isOn;
}

void ComponentToggle::Load(JsonValue jComponent) {
	isOn = jComponent[JSON_TAG_IS_ON];
}
