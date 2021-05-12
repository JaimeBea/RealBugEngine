#include "AIMovement.h"

#include "GameObject.h"
#include "GameplaySystems.h"
#include "TesseractEvent.h"

EXPOSE_MEMBERS(AIMovement) {
    // Add members here to expose them to the engine. Example:
    // MEMBER(MemberType::BOOL, exampleMember1),
    // MEMBER(MemberType::PREFAB_RESOURCE_UID, exampleMember2),
    // MEMBER(MemberType::GAME_OBJECT_UID, exampleMember3)
    MEMBER(MemberType::GAME_OBJECT_UID, fangUID),
    MEMBER(MemberType::GAME_OBJECT_UID, onimaruUID),
    MEMBER(MemberType::INT, maxSpeed),
    MEMBER(MemberType::INT, lifePoints),
    MEMBER(MemberType::FLOAT, searchRadius),
    MEMBER(MemberType::FLOAT, meleeRange)

};

GENERATE_BODY_IMPL(AIMovement);

void AIMovement::Start() {
    fang = GameplaySystems::GetGameObject(fangUID);
    onimaru = GameplaySystems::GetGameObject(onimaruUID);
    animation = GetOwner().GetComponent<ComponentAnimation>();    
}

void AIMovement::Update() {
    if (!GetOwner().IsActive()) return;

    hitTaken = HitDetected();

    if (hitTaken && lifePoints > 0) {
        if (state == AIState::IDLE) {
            animation->SendTrigger("IdleHurt");
        }
        else if (state == AIState::RUN) {
            animation->SendTrigger("RunHurt");
        }
        else if (state == AIState::ATTACK) {
            animation->SendTrigger("AttackHurt");
        }
        lifePoints--;
        state = AIState::HURT;
        hitTaken = false;
    }

    switch (state)
    {
    case AIState::START:
        if (Camera::CheckObjectInsideFrustum(&GetOwner())) {
            Seek(float3(GetOwner().GetComponent<ComponentTransform>()->GetGlobalPosition().x, 0, GetOwner().GetComponent<ComponentTransform>()->GetGlobalPosition().z), fallingSpeed);
            if (GetOwner().GetComponent<ComponentTransform>()->GetGlobalPosition().y == 0) {
                animation->SendTrigger("StartSpawn");
                state = AIState::SPAWN;
            }
        }
        break;
    case AIState::SPAWN:                
        break;
    case AIState::IDLE:
        if (CharacterInSight(fang)) {
            currentTarget = fang;
            animation->SendTrigger("IdleRun");
            state = AIState::RUN;
        }
        else if (CharacterInSight(onimaru)) {
            currentTarget = onimaru;
            animation->SendTrigger("IdleRun");
            state = AIState::RUN;
        }
        break;
    case AIState::RUN:
        Seek(currentTarget->GetComponent<ComponentTransform>()->GetGlobalPosition(), maxSpeed);
        if (CharacterInMeleeRange(currentTarget)) {
            animation->SendTrigger("RunAttack");
            state = AIState::ATTACK;
        }
        break;
    case AIState::HURT:                
        break;
    case AIState::ATTACK:
        //throws event for protagonist to listen?        
        break;
    case AIState::DEATH:
        break;
    }
    	
}

void AIMovement::ReceiveEvent(TesseractEvent& e)
{
    switch (e.type)
    {
    case TesseractEventType::ANIMATION_FINISHED:

        if (state == AIState::SPAWN) {
            animation->SendTrigger("SpawnIdle");
            state = AIState::IDLE;
        }

        else if(state == AIState::ATTACK)
        {
            animation->SendTrigger("AttackIdle");
            state = AIState::IDLE;
        }
        else if (state == AIState::HURT && lifePoints > 0) {
            animation->SendTrigger("HurtIdle");
            state = AIState::IDLE;
        }

        else if (state == AIState::HURT && lifePoints <= 0) {
            animation->SendTrigger("HurtDeath");
            state = AIState::DEATH;
        }
        else if (state == AIState::DEATH) {
            GameplaySystems::DestroyGameObject(&GetOwner());
        }
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

void AIMovement::Seek(const float3& newPosition, int speed)
{

    float3 position = GetOwner().GetComponent<ComponentTransform>()->GetGlobalPosition();
    float3 direction = newPosition - position;

    velocity = direction.Normalized() * speed;

    position += velocity * Time::GetDeltaTime();

    GetOwner().GetComponent<ComponentTransform>()->SetGlobalPosition(position);

    Quat newRotation = Quat::LookAt(float3(0, 0, 1), direction.Normalized(), float3(0, 1, 0), float3(0, 1, 0));
    GetOwner().GetComponent<ComponentTransform>()->SetGlobalRotation(newRotation);
}

bool AIMovement::HitDetected()
{
    //Listens for an event to signal this enemy has taken a hit
    return false;
}
