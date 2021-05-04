#include "AIMovement.h"

#include "GameObject.h"
#include "GameplaySystems.h"

EXPOSE_MEMBERS(AIMovement) {
    // Add members here to expose them to the engine. Example:
    // MEMBER(MemberType::BOOL, exampleMember1),
    // MEMBER(MemberType::PREFAB_RESOURCE_UID, exampleMember2),
    // MEMBER(MemberType::GAME_OBJECT_UID, exampleMember3)
};

GENERATE_BODY_IMPL(AIMovement);

void AIMovement::Start() {
    owner = GetOwner();
    fang = GameplaySystems::GetGameObject("Fang");
    //onimaru = GameplaySystems::GetGameObject("Onimaru");
    Debug::Log("AI movement for %s Start Log", owner.name);
}

void AIMovement::Update() {
    if (!owner.IsActive()) return;
    if (!fang /* && !onimaru*/) return;

    if (CharacterInSight(fang)) {
        Seek(fang);
    }
    
    /*else if (CharacterInSight(onimaru)) {
        Seek(onimaru);
    }*/

    else {
        //wander
    }


    



	
}

bool AIMovement::CharacterInSight(GameObject* character)
{
    return false;
}

void AIMovement::Seek(GameObject* target)
{
}
