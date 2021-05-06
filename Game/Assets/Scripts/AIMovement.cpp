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
    fang = GameplaySystems::GetGameObject("Fang");
    //onimaru = GameplaySystems::GetGameObject("Onimaru");    
}

void AIMovement::Update() {
    if (!GetOwner().IsActive()) return;
    
    if (CharacterInSight(fang)) {
        Seek(fang->GetComponent<ComponentTransform>()->GetPosition());
    }
    
    /*else if (CharacterInSight(onimaru)) {
        Seek(onimaru->GetComponent<ComponentTransform>()->GetPosition());
    }*/

    else {
        //wander
    }


    



	
}

bool AIMovement::CharacterInSight(const GameObject* character)
{
    ComponentTransform* target = character->GetComponent<ComponentTransform>();
    if (target) {
        float3 posTarget = target->GetGlobalPosition();
        return posTarget.Distance(GetOwner().GetComponent<ComponentTransform>()->GetGlobalPosition()) < searchRadius;
    }

    return false;
}

void AIMovement::Seek(const float3& newPosition)
{

    float3 position = GetOwner().GetComponent<ComponentTransform>()->GetPosition();
    float3 direction = newPosition - position;

    velocity = direction.Normalized() * maxSpeed;

    position += velocity * Time::GetDeltaTime();

    GetOwner().GetComponent<ComponentTransform>()->SetPosition(position);

    /*Quat desiredRotation = Quat::LookAt(float3(0, 0, 1), velocity.Normalized(), float3(0, 1, 0), float3(0, 1, 0));
    Quat newRotation = Quat::Slerp(GetOwner().GetComponent<ComponentTransform>()->GetRotation(), desiredRotation, Time::GetDeltaTime() * 0.01F);
    GetOwner().GetComponent<ComponentTransform>()->SetRotation(newRotation);*/
}
