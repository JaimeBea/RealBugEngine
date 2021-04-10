#include "aaaa.h"

#include "Utils/Logging.h"
#include "GameObject.h"
#include "GameplaySystems.h"

GENERATE_BODY_IMPL(aaaa);

void aaaa::Start() {
	LOG("Script start");
	gameObject = GameplaySystems::GetGameObject("Zombunny");
}

void aaaa::Update() {
	
	ComponentTransform* transform = gameObject->GetComponent<ComponentTransform>();

	float3 axis(1, 1, 0);
	float angle = DegToRad(10) * GameplaySystems::GetDeltaTime() * 2.5;
	Quat rot = transform->GetRotation();
	Quat finalRot = rot.RotateAxisAngle(axis, angle);

	transform->SetRotation(rot * finalRot);

}