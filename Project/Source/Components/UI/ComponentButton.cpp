#include "ComponentButton.h"
#include "GameObject.h"
#include <Application.h>
#include <Modules/ModuleInput.h>

#include <Utils/Logging.h>

#define JSON_TAG_COLOR_HOVER "ColorHover"
#define JSON_TAG_COLOR_CLICK "ColorClick"
#define JSON_TAG_COLOR_DISABLE "ColorDisable"
#define JSON_TAG_CLICKED "Clicked"


void ComponentButton::Init() {
	ComponentSelectable::Init();
}

void ComponentButton::OnEditorUpdate() {
	ComponentSelectable::OnEditorUpdate();
	ImGui::ColorEdit4("Hover Color##", colorHovered.ptr());
	ImGui::ColorEdit4("Click Color##", colorClicked.ptr());
	ImGui::ColorEdit4("Disable Color##", colorDisabled.ptr());
}

void ComponentButton::Save(JsonValue jComponent) const {
	jComponent[JSON_TAG_CLICKED] = clicked;

	JsonValue jColorHover = jComponent[JSON_TAG_COLOR_HOVER];
	jColorHover[0] = colorHovered.x;
	jColorHover[1] = colorHovered.y;
	jColorHover[2] = colorHovered.z;
	jColorHover[3] = colorHovered.w;

	JsonValue jColorClick = jComponent[JSON_TAG_COLOR_CLICK];
	jColorClick[0] = colorClicked.x;
	jColorClick[1] = colorClicked.y;
	jColorClick[2] = colorClicked.z;
	jColorClick[3] = colorClicked.w;

	JsonValue jColorDisable = jComponent[JSON_TAG_COLOR_DISABLE];
	jColorDisable[0] = colorDisabled.x;
	jColorDisable[1] = colorDisabled.y;
	jColorDisable[2] = colorDisabled.z;
	jColorDisable[3] = colorDisabled.w;
}

void ComponentButton::Load(JsonValue jComponent) {
	clicked = jComponent[JSON_TAG_CLICKED];
	
	JsonValue jColorHover = jComponent[JSON_TAG_COLOR_HOVER];
	colorHovered.Set(jColorHover[0], jColorHover[1], jColorHover[2], jColorHover[3]);

	JsonValue jColorClick = jComponent[JSON_TAG_COLOR_CLICK];
	colorClicked.Set(jColorClick[0], jColorClick[1], jColorClick[2], jColorClick[3]);

	JsonValue jColorDisable = jComponent[JSON_TAG_COLOR_DISABLE];
	colorDisabled.Set(jColorDisable[0], jColorDisable[1], jColorDisable[2], jColorDisable[3]);
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
