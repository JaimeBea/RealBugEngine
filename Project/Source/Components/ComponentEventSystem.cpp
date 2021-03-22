#include "ComponentEventSystem.h"
#include "UI/EventSystem/Event.h"
#include "Modules/ModuleUserInterface.h"
#include "Resources/GameObject.h"
#include "UI/Selectables/Selectable.h"
#include "Utils/Logging.h"
#include "Utils/Leaks.h"

ComponentEventSystem* ComponentEventSystem::currentEvSys = nullptr;

ComponentEventSystem::ComponentEventSystem(GameObject& owner, bool active)
	: Component(ComponentType::EVENT_SYSTEM, owner, active)
	, currentSelected(nullptr)
	, firstSelected(nullptr) {
}

ComponentEventSystem ::~ComponentEventSystem() {
}

void ComponentEventSystem::Init() {
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
