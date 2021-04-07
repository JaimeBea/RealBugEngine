#include "ComponentButton.h"
#include "GameObject.h"
#include <Application.h>
#include <Modules/ModuleInput.h>
#include <Modules/ModuleUserInterface.h>

#include <Utils/Logging.h>

#define JSON_TAG_COLOR_HOVER "ColorHover"
#define JSON_TAG_COLOR_CLICK "ColorClick"
#define JSON_TAG_COLOR_DISABLE "ColorDisable"
#define JSON_TAG_CLICKED "Clicked"

void ComponentButton::Init() {
	//ComponentSelectable::Init();
}

void ComponentButton::OnEditorUpdate() {
	//ComponentSelectable::OnEditorUpdate();
	ImGui::ColorEdit4("Click Color##", colorClicked.ptr());
}

void ComponentButton::Save(JsonValue jComponent) const {
	jComponent[JSON_TAG_CLICKED] = clicked;

	JsonValue jColorClick = jComponent[JSON_TAG_COLOR_CLICK];
	jColorClick[0] = colorClicked.x;
	jColorClick[1] = colorClicked.y;
	jColorClick[2] = colorClicked.z;
	jColorClick[3] = colorClicked.w;
}

void ComponentButton::Load(JsonValue jComponent) {
	clicked = jComponent[JSON_TAG_CLICKED];

	JsonValue jColorClick = jComponent[JSON_TAG_COLOR_CLICK];
	colorClicked.Set(jColorClick[0], jColorClick[1], jColorClick[2], jColorClick[3]);
}

void ComponentButton::OnClicked() {
	clicked = true;
	App->userInterface->GetCurrentEventSystem()->SetSelected(GetOwner().GetComponent<ComponentSelectable>());
	LOG("I was clicked");
	//TO DO
}

bool ComponentButton::IsClicked() const {
	return clicked;
}

void ComponentButton::SetClicked(bool b) {
	clicked = b;
}

const float4 ComponentButton::GetClickColor() const {
	return colorClicked;
}

const float4 ComponentButton::GetTintColor() const {
	if (IsActive()) {
		ComponentSelectable* sel = GetOwner().GetComponent<ComponentSelectable>();
		if (!sel->IsInteractable()) {
			return sel->GetDisabledColor();
		} else if (IsClicked()) {
			return colorClicked;
		} else if (sel->IsSelected()) {
			return sel->GetSelectedColor();
		} else if (sel->IsHovered()) {
			return sel->GetHoverColor();
		}
	}
	return float4::one;
}

void ComponentButton::DuplicateComponent(GameObject& owner) {
	ComponentButton* component = owner.CreateComponent<ComponentButton>();
	component->colorClicked = colorClicked;
}

void ComponentButton::Update() {
	if (clicked) {
		if (!App->input->GetMouseButton(1)) {
			clicked = false;
		}
	}
}
