#include "FangMovement.h"

#include "Utils/Logging.h"
#include "GameObject.h"
#include "GameplaySystems.h"

GENERATE_BODY_IMPL(FangMovement);

void FangMovement::Start() {
	gameObject = GameplaySystems::GetGameObject("Fang");
	speed = 5;
}

void FangMovement::Update() {
	if (!gameObject) return;

	if (Input::GetKeyCode(Input::KEYCODE::KEY_W)) {
		ComponentTransform* transform = gameObject->GetComponent<ComponentTransform>();
		float3 newPosition = transform->GetPosition();
		newPosition.z -= speed * Time::GetDeltaTime();
		transform->SetPosition(newPosition);
	}
	if (Input::GetKeyCode(Input::KEYCODE::KEY_A)) {
		ComponentTransform* transform = gameObject->GetComponent<ComponentTransform>();
		float3 newPosition = transform->GetPosition();
		newPosition.x -= speed * Time::GetDeltaTime();
		transform->SetPosition(newPosition);
	}
	if (Input::GetKeyCode(Input::KEYCODE::KEY_S)) {
		ComponentTransform* transform = gameObject->GetComponent<ComponentTransform>();
		float3 newPosition = transform->GetPosition();
		newPosition.z += speed * Time::GetDeltaTime();
		transform->SetPosition(newPosition);
	}
	if (Input::GetKeyCode(Input::KEYCODE::KEY_D)) {
		ComponentTransform* transform = gameObject->GetComponent<ComponentTransform>();
		float3 newPosition = transform->GetPosition();
		newPosition.x += speed * Time::GetDeltaTime();
		transform->SetPosition(newPosition);
	}
}