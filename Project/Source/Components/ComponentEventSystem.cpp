#include "ComponentEventSystem.h"

#include "Modules/ModuleUserInterface.h"

#include "GameObject.h"
#include "Event.h"

#include <Components/UI/ComponentSelectable.h>
#include "Utils/Logging.h"
#include "Utils/Leaks.h"

ComponentEventSystem* ComponentEventSystem::currentEvSys = nullptr;

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
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Current Selected:");

	if (currentSelected != nullptr) {
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 1, 1), currentSelected->GetOwner().name.c_str());
	}

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

void ComponentEventSystem::SetSelected(ComponentSelectable* newSelected) {
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