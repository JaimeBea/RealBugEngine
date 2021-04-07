#include "ComponentEventSystem.h"

#include "Application.h"
#include "Modules/ModuleUserInterface.h"
#include "Modules/ModuleInput.h"

#include "GameObject.h"
#include "Event.h"

#include <Components/UI/ComponentSelectable.h>
#include "Utils/Logging.h"
#include "Utils/Leaks.h"

ComponentEventSystem ::~ComponentEventSystem() {
}

void ComponentEventSystem::Init() {
	currentSelected = nullptr;
	firstSelected = nullptr;
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
		if (currentSelected) {
			ComponentSelectable* newSel = currentSelected->FindSelectableOnDir(selectionDir);
			if (newSel) SetSelected(newSel);
		}
	}
}

void ComponentEventSystem::OnTransformUpdate() {
}

void ComponentEventSystem::OnEditorUpdate() {
	ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "Current Selected:");

	if (currentSelected != nullptr) {
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 1.f), currentSelected->GetOwner().name.c_str());
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

	if (selectableToRemove != nullptr) {
		hoveredSelectables.erase(itToRemove);
	}
}

ComponentSelectable* ComponentEventSystem::GetCurrentSelected() const {
	return currentSelected;
}

ComponentSelectable* ComponentEventSystem::GetCurrentlyHovered() const {
	if (hoveredSelectables.size() == 0) return nullptr;
	return hoveredSelectables.front();
}