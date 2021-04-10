#include "ComponentEventSystem.h"

#include "Application.h"
#include "Modules/ModuleUserInterface.h"
#include "Modules/ModuleInput.h"

#include "GameObject.h"
#include "Event.h"

#include <Components/UI/ComponentSelectable.h>
#include <imgui.h>
#include "Utils/Logging.h"
#include "Utils/Leaks.h"

ComponentEventSystem ::~ComponentEventSystem() {
}

void ComponentEventSystem::Init() {
	currentSelected = 0;
	firstSelected = 0;
	App->userInterface->SetCurrentEventSystem(this);
	SetSelected(firstSelected);
}

void ComponentEventSystem::Update() {
	float2 selectionDir = float2(0.f, 0.f);
	bool keyPressed = false;
	if (App->input->GetKey(SDL_SCANCODE_UP) == KS_DOWN) {
		selectionDir = float2(0.f, 1.f);
		keyPressed = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KS_DOWN) {
		selectionDir = float2(0.f, -1.f);
		keyPressed = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KS_DOWN) {
		selectionDir = float2(-1.f, 0.f);
		keyPressed = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KS_DOWN) {
		selectionDir = float2(1.f, 0.f);
		keyPressed = true;
	}

	if (keyPressed) {
		if (currentSelected != 0) {
			ComponentSelectable* newSel = GetCurrentSelected()->FindSelectableOnDir(selectionDir);
			if (newSel) SetSelected(newSel->GetID());
		}
	}
}

void ComponentEventSystem::OnEditorUpdate() {
	ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "Current Selected:");

	if (currentSelected != 0) {
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 1.f), GetCurrentSelected()->GetOwner().name.c_str());
	}

	//TO DO FIRST SELECTED RESOURCE SLOT <GAMEOBJECT>
}

void ComponentEventSystem::Save(JsonValue jComponent) const {
	jComponent[JSON_TAG_FIRST_SELECTED_ID] = firstSelected;
}

void ComponentEventSystem::Load(JsonValue jComponent) {
	firstSelected = jComponent[JSON_TAG_FIRST_SELECTED_ID];
}

void ComponentEventSystem::Enable() {
	App->userInterface->SetCurrentEventSystem(this);
}

void ComponentEventSystem::Disable() {
	if (App->userInterface->GetCurrentEventSystem() == this) {
		App->userInterface->SetCurrentEventSystem(nullptr);
	}
}

void ComponentEventSystem::SetSelected(UID newSelectableComponentID) {
	ComponentSelectable* currentSel = GetCurrentSelected();
	if (currentSel != nullptr) {
		currentSel->OnDeselect();
	}
	currentSelected = newSelectableComponentID;

	ComponentSelectable* newSelectableComponent = GetOwner().scene->GetComponent<ComponentSelectable>(newSelectableComponentID);

	if (newSelectableComponent != nullptr) {
		newSelectableComponent->OnSelect();
	}
}

void ComponentEventSystem::DuplicateComponent(GameObject& owner) {
	ComponentEventSystem* component = owner.CreateComponent<ComponentEventSystem>();
	component->firstSelected = firstSelected;
}

void ComponentEventSystem::EnteredPointerOnSelectable(ComponentSelectable* newHoveredComponent) {
	for (std::vector<UID>::const_iterator it = hoveredSelectables.begin(); it != hoveredSelectables.end(); ++it) {
		if ((*it) == newHoveredComponent->GetID()) {
			return;
		}
	}
	hoveredSelectables.push_back(newHoveredComponent->GetID());
}

void ComponentEventSystem::ExitedPointerOnSelectable(ComponentSelectable* newUnHoveredComponent) {
	std::vector<UID>::iterator itToRemove;
	ComponentSelectable* selectableToRemove = nullptr;
	for (std::vector<UID>::iterator it = hoveredSelectables.begin(); it != hoveredSelectables.end() && selectableToRemove == nullptr; ++it) {
		if ((*it) == newUnHoveredComponent->GetID()) {
			itToRemove = it;
			selectableToRemove = GetOwner().scene->GetComponent<ComponentSelectable>(*it);
		}
	}

	if (selectableToRemove != nullptr) {
		hoveredSelectables.erase(itToRemove);
	}
}

ComponentSelectable* ComponentEventSystem::GetCurrentSelected() const {
	if (currentSelected == 0) return nullptr;

	return GetOwner().scene->GetComponent<ComponentSelectable>(currentSelected);
}

ComponentSelectable* ComponentEventSystem::GetCurrentlyHovered() const {
	if (hoveredSelectables.size() == 0) return nullptr;
	return GetOwner().scene->GetComponent<ComponentSelectable>(hoveredSelectables.front());
}