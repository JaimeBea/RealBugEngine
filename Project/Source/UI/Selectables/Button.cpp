#include "Button.h"
#include <Resources/GameObject.h>
#include <Application.h>

Button::Button(GameObject& owner)
	: Selectable(owner) {
}

Button::~Button() {
}

void Button::OnPressed() {
	//TO DO
}
