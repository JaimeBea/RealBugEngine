#include "ComponentSelectable.h"
#include "Application.h"
#include "Components/ComponentEventSystem.h"
#include "Components/UI/ComponentSelectable.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleInput.h"
#include "Modules/ModuleUserInterface.h"

#include "GameObject.h"
#include "imgui.h"

#include "Utils/Logging.h"

//TODO: ACTUALLY TRANSITION WHEN HIGHLIGHTING HAPPENS, REQUIRES CLASS GRAPHICS FOR SPRITES/IMAGES
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
		ComponentEventSystem::m_Selectables[selectableIndex] = ComponentEventSystem::m_Selectables[ComponentEventSystem::m_Selectables.size()];
		//Remove last position from vector, effectively removing myself
		ComponentEventSystem::m_Selectables.pop_back();
		selectableIndex = -1;
	}
}

bool ComponentSelectable::IsInteractable() const {
	return interactable;
}

void ComponentSelectable::SetInteractable(bool b) {
	interactable = b;
}

ComponentSelectable* ComponentSelectable::FindSelectableOnDir(float2 dir) {
	if (!App->userInterface->GetCurrentEventSystem()) return nullptr;

	switch (m_NavigationType) {
	case NavigationType::AUTOMATIC: {
		ComponentSelectable* bestCandidate = nullptr;
		float minDistance = FLT_MAX;
		float3 thisPos = this->GetOwner().GetComponent<ComponentTransform2D>()->GetPosition();

		// Get Gameobjects with the same parent
		/*for (GameObject* brother : this->GetOwner().GetParent()->GetChildren()) {
		ComponentSelectable* selectable = brother->GetComponent<ComponentSelectable>();
		if (!selectable) continue;*/
		// TODO: This is a hotfix because GetComponent doesn't support Component hierarchy. Ideally, should use the code commented above.
		for (ComponentSelectable* selectable : ComponentEventSystem::m_Selectables) {
			GameObject selectableObject = selectable->GetOwner();
			if (selectableObject.GetParent()->GetID() != this->GetOwner().GetParent()->GetID()) continue;

			// Get relative direction and distance to this Element
			float3 direction = selectableObject.GetComponent<ComponentTransform2D>()->GetPosition() - thisPos;
			float distance = direction.LengthSq();

			// Compare best candidate
			if (distance < minDistance) {
				if (dir.x > 0.6f && direction.x > 0.6f) {
					bestCandidate = selectable;
					minDistance = distance;
				} else if (dir.x < -0.6f && direction.x > -0.6f) {
					bestCandidate = selectable;
					minDistance = distance;
				} else if (dir.y > 0.6f && direction.y > 0.6f) {
					bestCandidate = selectable;
					minDistance = distance;
				} else if (dir.y < -0.6f && direction.y > -0.6f) {
					bestCandidate = selectable;
					minDistance = distance;
				}
			}
		}
		return bestCandidate;
		break;
	}
	case NavigationType::MANUAL: {
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
	}
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
	ComponentEventSystem::m_Selectables.push_back(this);

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

	//TODO: add as listener to MouseMoved event?
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
	if (ComponentEventSystem* evSyst = App->userInterface->GetCurrentEventSystem()) {
		selectableIndex = evSyst->m_Selectables.size();
		evSyst->m_Selectables.push_back(this);
	}
}

void ComponentSelectable::Disable() {
	if (ComponentEventSystem* evSys = App->userInterface->GetCurrentEventSystem()) {
		if (selected) {
			evSys->SetSelected(nullptr);
		}
	}
	//Subtitute my position in vector with last element of vector
	ComponentEventSystem::m_Selectables[selectableIndex] = ComponentEventSystem::m_Selectables[ComponentEventSystem::m_Selectables.size()];

	//Remove last position from vector, effectively removing myself
	ComponentEventSystem::m_Selectables.pop_back();
	selectableIndex = -1;
}

void ComponentSelectable::OnPointerEnter() {
	if (ComponentEventSystem* evSys = App->userInterface->GetCurrentEventSystem()) {
		hovered = true;
		if (evSys != nullptr) {
			evSys->EnteredPointerOnSelectable(this);
		}
	}
}

void ComponentSelectable::OnPointerExit() {
	if (ComponentEventSystem* evSys = App->userInterface->GetCurrentEventSystem()) {
		hovered = false;
		if (evSys != nullptr) {
			evSys->ExitedPointerOnSelectable(this);
		}
	}
}

bool ComponentSelectable::IsHovered() const {
	return hovered;
}
