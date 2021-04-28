#include "ChangeCharacter.h"

#include "GameObject.h"
#include "GameplaySystems.h"

GENERATE_BODY_IMPL(ChangeCharacter);

void ChangeCharacter::Start() {
	fang = GameplaySystems::GetGameObject("Fang");
	robot = GameplaySystems::GetGameObject("Robot");

	if (robot) {
		robot->Disable();
	}
	
	Debug::Log("Ending Start function");
}

void ChangeCharacter::Update() {
	if (!fang) return;
	if (!robot) return;

	if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_SPACE)) {
		if (fang->IsActive()) {
			Debug::Log("Swaping to robot...");
			fang->Disable();
			robot->Enable();
		}
		else {
			Debug::Log("Swaping to fang...");
			robot->Disable();
			fang->Enable();
		}
	}
}