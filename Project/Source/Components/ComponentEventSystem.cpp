#include "ComponentEventSystem.h"

#include "Modules/ModuleUserInterface.h"

#include "GameObject.h"
#include "Event.h"

#include "UI/Selectables/Selectable.h"
#include "Utils/Logging.h"
#include "Utils/Leaks.h"

ComponentEventSystem* ComponentEventSystem::currentEvSys = nullptr;

//ComponentEventSystem::ComponentEventSystem(GameObject* owner, UID componentID_, bool active)
//	: Component(ComponentType::EVENT_SYSTEM, owner, componentID_, active)
//	, currentSelected(nullptr)
//	, firstSelected(nullptr) {
//}

ComponentEventSystem ::~ComponentEventSystem() {
}

void ComponentEventSystem::Init() {
	currentSelected = nullptr;
	firstSelected = nullptr;
	currentEvSys = this;
	SetSelected(firstSelected);
}

void ComponentEventSystem::Update() {
}

void ComponentEventSystem::OnTransformUpdate() {
}

void ComponentEventSystem::OnEditorUpdate() {
	//TO DO
}

void ComponentEventSystem::Save(JsonValue jComponent) const {
	//TO DO
}

void ComponentEventSystem::Load(JsonValue jComponent) {
	//TO DO
}

void ComponentEventSystem::Enable() {
	currentEvSys = this;
}

void ComponentEventSystem::Disable() {
	if (currentEvSys == this) {
		currentEvSys = nullptr;
	}
}

void ComponentEventSystem::SetSelected(Selectable* newSelected) {
	if (currentSelected != nullptr) {
		currentSelected->OnDeselect();
	}
	currentSelected = newSelected;
	if (newSelected != nullptr) {
		newSelected->OnSelect();
	}
}

void ComponentEventSystem::DuplicateComponent(GameObject& owner) {
	ComponentEventSystem* component = owner.CreateComponent<ComponentEventSystem>();
	//TO DO
}