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
    Debug::Log("AI movement for %s Start Log", GetOwner().name);
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
    ComponentTransform* target = fang->GetComponent<ComponentTransform>();
    if (target) {
        float3 posTarget = target->GetPosition();
        return posTarget.Distance(GetOwner().GetComponent<ComponentTransform>()->GetPosition()) < searchRadius;
    }

    /*target = onimaru->GetComponent<ComponentTransform>();
    if (target) {
        float3 posTarget = target->GetPosition();
        return posTarget.Distance(GetOwner().GetComponent<ComponentTransform>()->GetPosition()) < searchRadius;
    }*/

    return false;
}

void AIMovement::Seek(const float3& newPosition)
{

    float3 position = GetOwner().GetComponent<ComponentTransform>()->GetPosition();
    float3 direction = newPosition - position;

    velocity = direction.Normalized() * maxSpeed;

    position += velocity * Time::GetDeltaTime();

    GetOwner().GetComponent<ComponentTransform>()->SetPosition(position);

    Quat desiredRotation = Quat::LookAt(float3::unitZ, velocity.Normalized(), float3::unitY, float3::unitY);
    Quat newRotation = Quat::Slerp(GetOwner().GetComponent<ComponentTransform>()->GetRotation(), desiredRotation, Time::GetDeltaTime() * 0.01F);
    GetOwner().GetComponent<ComponentTransform>()->SetRotation(newRotation);
}
