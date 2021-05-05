#include "MeleeGrunt_AnimationSet_testStateMachin.h"

#include "GameplaySystems.h"

#include "GameObject.h"
#include "Components/ComponentAnimation.h"

EXPOSE_MEMBERS(MeleeGrunt_AnimationSet_testStateMachin) {
    // Add members here to expose them to the engine. Example:
    // MEMBER(MemberType::BOOL, exampleMember1),
    // MEMBER(MemberType::PREFAB_RESOURCE_UID, exampleMember2),
    // MEMBER(MemberType::GAME_OBJECT_UID, exampleMember3)
};

GENERATE_BODY_IMPL(MeleeGrunt_AnimationSet_testStateMachin);

void MeleeGrunt_AnimationSet_testStateMachin::Start() {
	GameObject* goAnimated = GameplaySystems::GetGameObject("RootNode");
	if (goAnimated) {
		animation = goAnimated->GetComponent<ComponentAnimation>();
	}	
}

void MeleeGrunt_AnimationSet_testStateMachin::Update() {

	if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_1)) {
		animation->SendTrigger("SpawnIdle");
	}
	if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_2)) {
		animation->SendTrigger("IdleRun");
	}
	if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_3)) {
		animation->SendTrigger("RunHurt");
	}
	if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_4)) {
		animation->SendTrigger("HurtAttack");
	}
	if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_5)) {
		animation->SendTrigger("AttackDeath");
	}
}