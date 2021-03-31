#include "ComponentButton.h"
#include "GameObject.h"
#include <Application.h>
#include <Modules/ModuleInput.h>

#include <Utils/Logging.h>

void ComponentButton::Init() {
	ComponentSelectable::Init();
}

void ComponentButton::OnEditorUpdate() {
	ComponentSelectable::OnEditorUpdate();
	ImGui::ColorEdit4("Hover Color##", colorHovered.ptr());
	ImGui::ColorEdit4("Click Color##", colorClicked.ptr());
	ImGui::ColorEdit4("Disable Color##", colorDisabled.ptr());
}

void ComponentButton::OnClicked() {
	clicked = true;
	LOG("I was clicked");
	//TO DO
}

void ComponentButton::OnPointerEnter() {
	ComponentSelectable::OnPointerEnter();
	ComponentEventSystem::currentEvSys->SetSelected(this);
}

void ComponentButton::OnPointerExit() {
	ComponentSelectable::OnPointerExit();
}

bool ComponentButton::IsClicked() const {
	return clicked;
}

void ComponentButton::SetClicked(bool b) {
	clicked = b;
}

const float4 ComponentButton::GetHoverColor() const {
	return colorHovered;
}

const float4 ComponentButton::GetClickColor() const {
	return colorClicked;
}

const float4 ComponentButton::GetDisableColor() const {
	return colorDisabled;
}

const float4 ComponentButton::GetTintColor() const {
	if (IsActive()) {
		if (!IsInteractable()) {
			return colorDisabled;
		} else if (IsClicked()) {
			return colorClicked;
		} else if (IsHovered()) {
			return colorHovered;
		}
	}
	
	return float4::one;
}

void ComponentButton::DuplicateComponent(GameObject& owner) {
	ComponentButton* component = owner.CreateComponent<ComponentButton>();
	//TO DO
}

void ComponentButton::Update() {
	if (clicked) {
		if (!App->input->GetMouseButton(1)) {
			clicked = false;
		}
	}
}
