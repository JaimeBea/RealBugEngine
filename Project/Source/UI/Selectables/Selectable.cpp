#include "Selectable.h"
#include "Application.h"
#include "Components/ComponentEventSystem.h"
#include "Modules/ModuleEditor.h"
#include <Resources/GameObject.h>
#include "imgui.h"

#include "Utils/Logging.h"

//TO DO ACTUALLY TRANSITION	WHEN HIGHLIGHTING HAPPENS, REQUIRES CLASS GRAPHICS FOR SPRITES/IMAGES
void Selectable::Highlight(bool b) {
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

Selectable::Selectable(GameObject& owner, bool active)
	: Component(ComponentType::SELECTABLE, owner, active)
	, interactable(false)
	, highlighted(false)
	, selected(false)
	, m_NavigationType(NavigationType::AUTOMATIC)
	, onAxisDown(nullptr)
	, onAxisLeft(nullptr)
	, onAxisRight(nullptr)
	, onAxisUp(nullptr)
	, selectableIndex(-1)
	, m_Transition(TransitionType::NONE) {
}

Selectable::~Selectable() {
	//TO DO IF SELECTED SET SELECTED TO NULL
	if (selectableIndex > -1) {
		//Subtitute my position in vector with last element of vector
		ComponentEventSystem::currentEvSys->m_Selectables[selectableIndex] = ComponentEventSystem::currentEvSys->m_Selectables[ComponentEventSystem::currentEvSys->m_Selectables.size()];
		//Remove last position from vector, effectively removing myself
		ComponentEventSystem::currentEvSys->m_Selectables.pop_back();
		selectableIndex = -1;
	}
}

bool Selectable::GetInteractable() {
	return interactable;
}

void Selectable::SetInteractable(bool b) {
	interactable = b;
}

Selectable* Selectable::FindSelectableOnDir(float2 dir) {
	switch (m_NavigationType) {
	case NavigationType::AUTOMATIC:
		for (std::vector<Selectable*>::iterator it = ComponentEventSystem::currentEvSys->m_Selectables.begin(); it != ComponentEventSystem::currentEvSys->m_Selectables.end(); ++it) {
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

void Selectable::OnSelect() {
	selected = true;
	Highlight(true);
}

void Selectable::OnDeselect() {
	selected = false;
	Highlight(false);
}

void Selectable::Init() {
	ComponentEventSystem::currentEvSys->m_Selectables.push_back(this);
	//TO DO add as listener	to MouseMoved event?
}

void Selectable::Update() {
}

void Selectable::OnEditorUpdate() {
	if (ImGui::CollapsingHeader("Selectable")) {
		bool isInteractable = interactable;
		if (ImGui::Checkbox("Interactable", &isInteractable)) {
			SetInteractable(isInteractable);
		}

		// Navigation Type combo box
		const char* navigationTypeItems[] = {"Automatic", "Manual"};
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
		const char* transitionTypeItems[] = {"Automatic", "Manual"};
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

		//TO DO Drag/Drop for manual navigation references (4 Selectable pointers)
	}
}

void Selectable::Enable() {
	selectableIndex = ComponentEventSystem::currentEvSys->m_Selectables.size();
	ComponentEventSystem::currentEvSys->m_Selectables.push_back(this);
}

void Selectable::Disable() {
	//TO DO IF SELECTED SET SELECTED TO NULL
	if (selected) {
		ComponentEventSystem::currentEvSys->SetSelected(nullptr);
	}

	//Subtitute my position in vector with last element of vector
	ComponentEventSystem::currentEvSys->m_Selectables[selectableIndex] = ComponentEventSystem::currentEvSys->m_Selectables[ComponentEventSystem::currentEvSys->m_Selectables.size()];

	//Remove last position from vector, effectively removing myself
	ComponentEventSystem::currentEvSys->m_Selectables.pop_back();
	selectableIndex = -1;
}

void Selectable::OnPointerEnter() {
	LOG("PointerEntered");
}
