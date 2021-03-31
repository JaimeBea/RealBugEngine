#include "ComponentSelectable.h"
#include "Application.h"
#include "Components/ComponentEventSystem.h"
#include "Modules/ModuleEditor.h"
#include "GameObject.h"
#include "imgui.h"

#include "Utils/Logging.h"

//TO DO ACTUALLY TRANSITION	WHEN HIGHLIGHTING HAPPENS, REQUIRES CLASS GRAPHICS FOR SPRITES/IMAGES
void ComponentSelectable::Highlight(bool b) {
	switch (m_Transition) {
	case TransitionType::COLOR_CHANGE:
		break;
	case TransitionType::ANIMATION:
		break;
	default:
		break;
	}

	highlighted = b;
}

ComponentSelectable::ComponentSelectable(ComponentType type_, GameObject* owner, UID componentID_, bool active)
	: Component(type_, owner, componentID_, active) {
}

ComponentSelectable::~ComponentSelectable() {
	//TO DO IF SELECTED SET SELECTED TO NULL
	if (selectableIndex > -1) {
		//Subtitute my position in vector with last element of vector
		ComponentEventSystem::currentEvSys->m_Selectables[selectableIndex] = ComponentEventSystem::currentEvSys->m_Selectables[ComponentEventSystem::currentEvSys->m_Selectables.size()];
		//Remove last position from vector, effectively removing myself
		ComponentEventSystem::currentEvSys->m_Selectables.pop_back();
		selectableIndex = -1;
	}
}

bool ComponentSelectable::GetInteractable() {
	return interactable;
}

void ComponentSelectable::SetInteractable(bool b) {
	interactable = b;
}

ComponentSelectable* ComponentSelectable::FindSelectableOnDir(float2 dir) {
	switch (m_NavigationType) {
	case NavigationType::AUTOMATIC:
		for (std::vector<ComponentSelectable*>::iterator it = ComponentEventSystem::currentEvSys->m_Selectables.begin(); it != ComponentEventSystem::currentEvSys->m_Selectables.end(); ++it) {
			//TO DO Compare all selectables by distance and assign closest to retSelectable
		}
		break;
	case NavigationType::MANUAL:
		if (dir.x > 0.6f) {
			return onAxisRight;
		} else if (dir.x < -0.6f) {
			return onAxisLeft;
		} else if (dir.y > 0.6f) {
			return onAxisUp;
		} else if (dir.y < -0.6f) {
			return onAxisDown;
		}
		break;
	default:
		break;
	}

	return nullptr;
}

void ComponentSelectable::OnSelect() {
	selected = true;
	Highlight(true);
}

void ComponentSelectable::OnDeselect() {
	selected = false;
	Highlight(false);
}

void ComponentSelectable::Init() {
	ComponentEventSystem::currentEvSys->m_Selectables.push_back(this);
	interactable = false;
	highlighted = false;
	selected = false;
	m_NavigationType = NavigationType::AUTOMATIC;
	onAxisDown = nullptr;
	onAxisLeft = nullptr;
	onAxisRight = nullptr;
	onAxisUp = nullptr;
	selectableIndex = -1;
	m_Transition = TransitionType::NONE;

	//TO DO add as listener	to MouseMoved event?
}

void ComponentSelectable::Update() {
}

void ComponentSelectable::OnEditorUpdate() {
	bool isInteractable = interactable;
	if (ImGui::Checkbox("Interactable", &isInteractable)) {
		SetInteractable(isInteractable);
	}

	// Navigation Type combo box
	const char* navigationTypeItems[] = {"None", "Automatic", "Manual"};
	const char* navigationCurrent = navigationTypeItems[(int) m_NavigationType];
	if (ImGui::BeginCombo("Navigation Mode", navigationCurrent)) {
		for (int n = 0; n < IM_ARRAYSIZE(navigationTypeItems); ++n) {
			bool isSelected = (navigationCurrent == navigationTypeItems[n]);
			if (ImGui::Selectable(navigationTypeItems[n], isSelected)) {
				m_NavigationType = NavigationType(n);
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	//Transition Type combo box
	const char* transitionTypeItems[] = {"None", "Color Transition", "Animation"};
	const char* transitionCurrent = transitionTypeItems[(int) m_Transition];
	if (ImGui::BeginCombo("Transition", transitionCurrent)) {
		for (int n = 0; n < IM_ARRAYSIZE(transitionTypeItems); ++n) {
			bool isSelected = (transitionCurrent == transitionTypeItems[n]);
			if (ImGui::Selectable(transitionTypeItems[n], isSelected)) {
				m_Transition = TransitionType(n);
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	//TO DO Drag/Drop for manual navigation references (4 ComponentSelectable pointers)
}

void ComponentSelectable::Enable() {
	selectableIndex = ComponentEventSystem::currentEvSys->m_Selectables.size();
	ComponentEventSystem::currentEvSys->m_Selectables.push_back(this);
}

void ComponentSelectable::Disable() {
	if (selected) {
		ComponentEventSystem::currentEvSys->SetSelected(nullptr);
	}

	//Subtitute my position in vector with last element of vector
	ComponentEventSystem::currentEvSys->m_Selectables[selectableIndex] = ComponentEventSystem::currentEvSys->m_Selectables[ComponentEventSystem::currentEvSys->m_Selectables.size()];

	//Remove last position from vector, effectively removing myself
	ComponentEventSystem::currentEvSys->m_Selectables.pop_back();
	selectableIndex = -1;
}

void ComponentSelectable::OnPointerEnter() {
	hovered = true;
	if (ComponentEventSystem::currentEvSys != nullptr) {
		ComponentEventSystem::currentEvSys->EnteredPointerOnSelectable(this);
	}
}

void ComponentSelectable::OnPointerExit() {
	hovered = false;
	if (ComponentEventSystem::currentEvSys != nullptr) {
		ComponentEventSystem::currentEvSys->ExitedPointerOnSelectable(this);
	}
}

bool ComponentSelectable::IsHovered() const {
	return hovered;
}
