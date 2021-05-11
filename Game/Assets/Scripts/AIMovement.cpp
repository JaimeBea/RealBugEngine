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
    animation = GetOwner().GetComponent<ComponentAnimation>();    
}

void AIMovement::Update() {
    if (!GetOwner().IsActive()) return;

    hitTaken = HitDetected();

    if (hitTaken) {
        if (state == AIState::IDLE) {
            animation->SendTrigger("IdleHurt");
        }
        else if (state == AIState::RUN) {
            animation->SendTrigger("RunHurt");
        }
        else if (state == AIState::ATTACK) {
            animation->SendTrigger("AttackHurt");
        }
        state = AIState::HURT;
        hitTaken = false;
    }

    switch (state)
    {
    case AIState::SPAWN:
        if (/*owner pos inside game camera frustum*/true) {
            animation->SendTrigger("SpawnIdle");
            state = AIState::IDLE;
        }
        break;
    case AIState::IDLE:
        if (CharacterInSight(fang)) {
            currentTarget = fang;
            animation->SendTrigger("IdleRun");
            state = AIState::RUN;
        }
        /*else if (CharacterInSight(onimaru)) {
            currentTarget = onimaru;
            animation->SendTrigger("IdleRun");
            state = AIState::RUN;
        }*/
        break;
    case AIState::RUN:
        Seek(currentTarget->GetComponent<ComponentTransform>()->GetGlobalPosition());
        if (CharacterInMeleeRange(currentTarget)) {
            state = AIState::ATTACK;
        }
        break;
    case AIState::HURT:
        //play hurt animation       
        lifePoints--;
        if (lifePoints <= 0) {
            state = AIState::DEATH;
        }
        state = AIState::IDLE;        
        break;
    case AIState::ATTACK:
        //play attack animation
        //throws event for protagonist to listen?
        state = AIState::IDLE;
        break;
    case AIState::DEATH:
        //play death animation
        GetOwner().Disable(); //better delete
        break;
    default:
        break;
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

bool AIMovement::CharacterInMeleeRange(const GameObject* character)
{
    ComponentTransform* target = character->GetComponent<ComponentTransform>();
    if (target) {
        float3 posTarget = target->GetGlobalPosition();
        return posTarget.Distance(GetOwner().GetComponent<ComponentTransform>()->GetGlobalPosition()) < meleeRange;
    }

    return false;
}

void AIMovement::Seek(const float3& newPosition)
{

    float3 position = GetOwner().GetComponent<ComponentTransform>()->GetGlobalPosition();
    float3 direction = newPosition - position;

    velocity = direction.Normalized() * maxSpeed;

    position += velocity * Time::GetDeltaTime();

    GetOwner().GetComponent<ComponentTransform>()->SetGlobalPosition(position);    
}

bool AIMovement::HitDetected()
{
    //Listens for an event to signal this enemy has taken a hit
    return false;
}
