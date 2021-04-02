#include "ComponentEventSystem.h"

#include "Application.h"
#include "Modules/ModuleUserInterface.h"

#include "GameObject.h"
#include "Event.h"

#include <Components/UI/ComponentSelectable.h>
#include "Utils/Logging.h"
#include "Utils/Leaks.h"

std::vector<ComponentSelectable*> ComponentEventSystem::m_Selectables;

ComponentEventSystem ::~ComponentEventSystem() {
}

void ComponentEventSystem::Init() {
	currentSelected = nullptr;
	firstSelected = nullptr;
	App->userInterface->SetCurrentEventSystem(this);
	SetSelected(firstSelected);
}

void ComponentEventSystem::Update() {
	// TODO: Input arrowkeys to navigate
	// currentSelected
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
	App->userInterface->SetCurrentEventSystem(this);
}

void ComponentEventSystem::Disable() {
	if (App->userInterface->GetCurrentEventSystem() == this) {
		App->userInterface->SetCurrentEventSystem(nullptr);
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
//
//void ComponentEventSystem::AddPointerEnterHandler(IPointerEnterHandler* newH) {
//	for (std::vector<IPointerEnterHandler*>::const_iterator it = pointerEnterHandlers.begin(); it != pointerEnterHandlers.end(); ++it) {
//		if ((*it) == newH) {
//			return;
//		}
//	}
//	pointerEnterHandlers.push_back(newH);
//}

void ComponentEventSystem::EnteredPointerOnSelectable(ComponentSelectable* newH) {
	for (std::vector<ComponentSelectable*>::const_iterator it = hoveredSelectables.begin(); it != hoveredSelectables.end(); ++it) {
		if ((*it) == newH) {
			return;
		}
	}
	hoveredSelectables.push_back(newH);
}

void ComponentEventSystem::ExitedPointerOnSelectable(ComponentSelectable* newH) {
	std::vector<ComponentSelectable*>::iterator itToRemove;
	ComponentSelectable* selectableToRemove = nullptr;
	for (std::vector<ComponentSelectable*>::iterator it = hoveredSelectables.begin(); it != hoveredSelectables.end() && selectableToRemove == nullptr; ++it) {
		if ((*it) == newH) {
			itToRemove = it;
			selectableToRemove = *it;
		}
	}

	hoveredSelectables.erase(itToRemove);
}

ComponentSelectable* ComponentEventSystem::GetCurrentSelected() const {
	return currentSelected;
}

ComponentSelectable* ComponentEventSystem::GetCurrentlyHovered() const {
	if (hoveredSelectables.size() == 0) return nullptr;
	return hoveredSelectables.front();
}