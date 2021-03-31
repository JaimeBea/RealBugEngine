#include "ComponentButton.h"
#include "GameObject.h"
#include <Application.h>
#include <Modules/ModuleInput.h>

#include <Utils/Logging.h>

void ComponentButton::Init() {
	ComponentSelectable::Init();
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

bool ComponentButton::GetClicked() const {
	return clicked;
}

void ComponentButton::SetClicked(bool b) {
	clicked = b;
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
