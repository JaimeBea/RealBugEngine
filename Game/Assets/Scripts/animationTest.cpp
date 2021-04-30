#include "animationTest.h"

#include "GameplaySystems.h"

#include "GameObject.h"
#include "Components/ComponentAnimation.h"

GENERATE_BODY_IMPL(animationTest);

void animationTest::Start() {
	GameObject* goAnimated = GameplaySystems::GetGameObject("RootNode");
	if (goAnimated) {
		animation = goAnimated->GetComponent<ComponentAnimation>();
	}

}

void animationTest::Update() {

	if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_1)) {
		animation->SendTrigger("s1Ts2");
	}
	if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_2)) {
		animation->SendTrigger("s2Ts3");
	}
	if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_3)) {
		animation->SendTrigger("s3Ts1");
	}
}