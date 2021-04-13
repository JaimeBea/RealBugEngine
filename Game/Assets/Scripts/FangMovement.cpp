#include "FangMovement.h"

#include "Utils/Logging.h"
#include "GameObject.h"
#include "GameplaySystems.h"

GENERATE_BODY_IMPL(FrangMovement);

void FrangMovement::Start() {
	gameObject = GameplaySystems::GetGameObject("Fang");
}

void FrangMovement::Update() {
	if (Input::GetKeyCode(Input::KEYCODE::KEY_W)) {
		ComponentTransform* transform = gameObject->GetComponent<ComponentTransform>();
		float3 newPosition = transform->GetPosition();
		newPosition.z += speed * Time::GetDeltaTime();
		transform->SetPosition(newPosition);
	}
	if (Input::GetKeyCode(Input::KEYCODE::KEY_A)) {
		LOG("A");
		ComponentTransform* transform = gameObject->GetComponent<ComponentTransform>();
		float3 newPosition = transform->GetPosition();
		newPosition.x -= speed * Time::GetDeltaTime();
		transform->SetPosition(newPosition);
	}
	if (Input::GetKeyCode(Input::KEYCODE::KEY_S)) {
		LOG("S");
		ComponentTransform* transform = gameObject->GetComponent<ComponentTransform>();
		float3 newPosition = transform->GetPosition();
		newPosition.z -= speed * Time::GetDeltaTime();
		transform->SetPosition(newPosition);
	}
	if (Input::GetKeyCode(Input::KEYCODE::KEY_D)) {
		LOG("D");
		ComponentTransform* transform = gameObject->GetComponent<ComponentTransform>();
		float3 newPosition = transform->GetPosition();
		newPosition.x += speed * Time::GetDeltaTime();
		transform->SetPosition(newPosition);
	}
}