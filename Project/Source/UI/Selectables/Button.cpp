#include "Button.h"
#include <Resources/GameObject.h>
#include <Application.h>
#include <Modules/ModuleEventSystem.h>

Button::Button(GameObject& owner)
	: Selectable(owner) {
}

Button::~Button() {
}

void Button::OnPressed() {
	App->eventSystem->AddEvent(Event(Event::EventType::Button_Pressed, &GetOwner()));
}
