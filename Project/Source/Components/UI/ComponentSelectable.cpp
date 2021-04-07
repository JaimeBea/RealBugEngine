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
	switch (transitionType) {
	case TransitionType::COLOR_CHANGE:
		break;
	case TransitionType::ANIMATION:
		break;
	default:
		break;
	}

	highlighted = b;
}

//ComponentSelectable::ComponentSelectable(ComponentType type_, GameObject* owner, UID componentID_, bool active)
//	: Component(type_, owner, componentID_, active) {
//}

ComponentSelectable::~ComponentSelectable() {
	//TO DO IF SELECTED SET SELECTED TO NULL
}

bool ComponentSelectable::IsInteractable() const {
	return interactable;
}

void ComponentSelectable::SetInteractable(bool b) {
	interactable = b;
}

ComponentSelectable* ComponentSelectable::FindSelectableOnDir(float2 dir) {
	if (!App->userInterface->GetCurrentEventSystem()) return nullptr;

	switch (navigationType) {
	case NavigationType::AUTOMATIC: {
		ComponentSelectable* bestCandidate = nullptr;
		float minDistance = FLT_MAX;
		float3 thisPos = this->GetOwner().GetComponent<ComponentTransform2D>()->GetPosition();

		// Get Gameobjects with the same parent
		for (GameObject* brother : this->GetOwner().GetParent()->GetChildren()) {
			ComponentSelectable* selectable = brother->GetComponent<ComponentSelectable>();
			if (!selectable) continue;

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
				} else if (dir.x < -0.6f && direction.x < -0.6f) {
					bestCandidate = selectable;
					minDistance = distance;
				} else if (dir.y > 0.6f && direction.y > 0.6f) {
					bestCandidate = selectable;
					minDistance = distance;
				} else if (dir.y < -0.6f && direction.y < -0.6f) {
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
			return GetOwner().scene->GetGameObject(onAxisRight)->GetComponent<ComponentSelectable>();
		} else if (dir.x < -0.6f) {
			return GetOwner().scene->GetGameObject(onAxisLeft)->GetComponent<ComponentSelectable>();
		} else if (dir.y > 0.6f) {
			return GetOwner().scene->GetGameObject(onAxisUp)->GetComponent<ComponentSelectable>();
		} else if (dir.y < -0.6f) {
			return GetOwner().scene->GetGameObject(onAxisDown)->GetComponent<ComponentSelectable>();
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
	interactable = false;
	highlighted = false;
	selected = false;
	navigationType = NavigationType::AUTOMATIC;
	onAxisDown = 0;
	onAxisLeft = 0;
	onAxisRight = 0;
	onAxisUp = 0;
	transitionType = TransitionType::NONE;

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
	const char* navigationCurrent = navigationTypeItems[(int) navigationType];
	if (ImGui::BeginCombo("Navigation Mode", navigationCurrent)) {
		for (int n = 0; n < IM_ARRAYSIZE(navigationTypeItems); ++n) {
			bool isSelected = (navigationCurrent == navigationTypeItems[n]);
			if (ImGui::Selectable(navigationTypeItems[n], isSelected)) {
				navigationType = NavigationType(n);
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	//Transition Type combo box
	const char* transitionTypeItems[] = {"None", "Color Transition", "Animation"};
	const char* transitionCurrent = transitionTypeItems[(int) transitionType];
	if (ImGui::BeginCombo("Transition", transitionCurrent)) {
		for (int n = 0; n < IM_ARRAYSIZE(transitionTypeItems); ++n) {
			bool isSelected = (transitionCurrent == transitionTypeItems[n]);
			if (ImGui::Selectable(transitionTypeItems[n], isSelected)) {
				transitionType = TransitionType(n);
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	ImGui::ColorEdit4("Disable Color##", colorDisabled.ptr());
	ImGui::ColorEdit4("Hovered Color##", colorHovered.ptr());
	ImGui::ColorEdit4("Selected Color##", colorSelected.ptr());

	//TO DO Drag/Drop for manual navigation references (4 ComponentSelectable pointers)
}

void ComponentSelectable::Enable() {
}

void ComponentSelectable::Disable() {
	if (ComponentEventSystem* evSys = App->userInterface->GetCurrentEventSystem()) {
		if (selected) {
			evSys->SetSelected(0);
		}
	}
}

void ComponentSelectable::OnPointerEnter() {
	if (ComponentEventSystem* evSys = App->userInterface->GetCurrentEventSystem()) {
		hovered = true;
		if (evSys != nullptr) {
			evSys->EnteredPointerOnSelectable(this);
		}
	}
}

const float4 ComponentSelectable::GetDisabledColor() const {
	return colorDisabled;
}

void ComponentSelectable::OnPointerExit() {
	if (ComponentEventSystem* evSys = App->userInterface->GetCurrentEventSystem()) {
		hovered = false;
		if (evSys != nullptr) {
			evSys->ExitedPointerOnSelectable(this);
		}
	}
}

void ComponentSelectable::DuplicateComponent(GameObject& owner) {
	ComponentSelectable* component = owner.CreateComponent<ComponentSelectable>();
	component->interactable = interactable;
	component->colorDisabled = colorDisabled;
	component->colorHovered = colorHovered;
	component->onAxisDown = onAxisDown;
	component->onAxisUp = onAxisUp;
	component->onAxisRight = onAxisRight;
	component->onAxisLeft = onAxisLeft;
	component->selectableType = selectableType;
	component->navigationType = navigationType;
	component->transitionType = transitionType;
}

bool ComponentSelectable::IsHovered() const {
	return hovered;
}

bool ComponentSelectable::IsSelected() const {
	return selected;
}

void ComponentSelectable::Save(JsonValue jsonVal) const {
	JsonValue jColorHover = jsonVal[JSON_TAG_COLOR_HOVERED];
	jColorHover[0] = colorHovered.x;
	jColorHover[1] = colorHovered.y;
	jColorHover[2] = colorHovered.z;
	jColorHover[3] = colorHovered.w;

	JsonValue jColorDisable = jsonVal[JSON_TAG_COLOR_DISABLED];
	jColorDisable[0] = colorDisabled.x;
	jColorDisable[1] = colorDisabled.y;
	jColorDisable[2] = colorDisabled.z;
	jColorDisable[3] = colorDisabled.w;

	JsonValue jColorSelected = jsonVal[JSON_TAG_COLOR_SELECTED];
	jColorSelected[0] = colorSelected.x;
	jColorSelected[1] = colorSelected.y;
	jColorSelected[2] = colorSelected.z;
	jColorSelected[3] = colorSelected.w;

	//TO DO RESEARCH WHY THIS RETURNS ERROR
	JsonValue jInteractable = jsonVal[JSON_TAG_INTERACTABLE];
	jInteractable = interactable;

	JsonValue jNavigationType = jsonVal[JSON_TAG_NAVIGATION_TYPE];
	jNavigationType = (int) navigationType;

	JsonValue jTransitiontype = jsonVal[JSON_TAG_TRANSITION_TYPE];
	jTransitiontype = (int) transitionType;

	jsonVal[JSON_TAG_ON_AXIS_DOWN] = onAxisDown;

	jsonVal[JSON_TAG_ON_AXIS_UP] = onAxisUp;

	jsonVal[JSON_TAG_ON_AXIS_RIGHT] = onAxisRight;

	jsonVal[JSON_TAG_ON_AXIS_LEFT] = onAxisLeft;
}

void ComponentSelectable::Load(JsonValue jsonVal) {
	JsonValue jColorHover = jsonVal[JSON_TAG_COLOR_HOVERED];
	colorHovered.Set(jColorHover[0], jColorHover[1], jColorHover[2], jColorHover[3]);

	JsonValue jColorDisabled = jsonVal[JSON_TAG_COLOR_DISABLED];
	colorDisabled.Set(jColorDisabled[0], jColorDisabled[1], jColorDisabled[2], jColorDisabled[3]);

	JsonValue jColorSelected = jsonVal[JSON_TAG_COLOR_SELECTED];
	colorDisabled.Set(jColorSelected[0], jColorSelected[1], jColorSelected[2], jColorSelected[3]);

	JsonValue jInteractable = jsonVal[JSON_TAG_INTERACTABLE];
	interactable = jInteractable;

	JsonValue jNavigationType = jsonVal[JSON_TAG_NAVIGATION_TYPE];
	navigationType = (NavigationType)((int) jsonVal[JSON_TAG_NAVIGATION_TYPE]);

	JsonValue jTransitionType = jsonVal[JSON_TAG_TRANSITION_TYPE];
	transitionType = (TransitionType)((int) jsonVal[JSON_TAG_TRANSITION_TYPE]);

	//TO DO MOVE THESE THANGS TO POST LOAD

	onAxisDown = jsonVal[JSON_TAG_ON_AXIS_DOWN];

	onAxisUp = jsonVal[JSON_TAG_ON_AXIS_UP];

	onAxisRight = jsonVal[JSON_TAG_ON_AXIS_RIGHT];

	onAxisLeft = jsonVal[JSON_TAG_ON_AXIS_LEFT];
}

const float4 ComponentSelectable::GetHoverColor() const {
	return colorHovered;
}

const float4 ComponentSelectable::GetSelectedColor() const {
	return colorSelected;
}

Component* ComponentSelectable::GetSelectableComponent() {
	switch (selectableType) {
	case ComponentType::BUTTON:
		return GetOwner().GetComponent<ComponentButton>();
	case ComponentType::TOGGLE:
		return GetOwner().GetComponent<ComponentToggle>();
	default:
		return nullptr;
	}
}

void ComponentSelectable::SetSelectableType(ComponentType type_) {
	selectableType = type_;
}
