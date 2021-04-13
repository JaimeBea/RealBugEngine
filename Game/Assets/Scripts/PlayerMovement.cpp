#include "PlayerMovement.h"

#include "Utils/Logging.h"
#include "GameObject.h"
#include "GameplaySystems.h"

GENERATE_BODY_IMPL(PlayerMovement);

void PlayerMovement::Start() {
	gameObject = GameplaySystems::GetGameObject("Hearse");
}

void PlayerMovement::Update() {
	if (Input::GetKeyCode(Input::KEYCODE::KEY_W)) {
		ComponentTransform* transform = gameObject->GetComponent<ComponentTransform>();
		float3 newPosition = transform->GetPosition();
		newPosition.z += speed * GameplaySystems::GetDeltaTime();
		LOG("%f", speed * GameplaySystems::GetDeltaTime());
		transform->SetPosition(newPosition);
	}
	if (Input::GetKeyCode(Input::KEYCODE::KEY_A)) {
		LOG("A");
		ComponentTransform* transform = gameObject->GetComponent<ComponentTransform>();
		float3 newPosition = transform->GetPosition();
		newPosition.x -= speed * GameplaySystems::GetDeltaTime();
		transform->SetPosition(newPosition);
	}
	if (Input::GetKeyCode(Input::KEYCODE::KEY_S)) {
		LOG("S");
		ComponentTransform* transform = gameObject->GetComponent<ComponentTransform>();
		float3 newPosition = transform->GetPosition();
		newPosition.z -= speed * GameplaySystems::GetDeltaTime();
		transform->SetPosition(newPosition);
	}
	if (Input::GetKeyCode(Input::KEYCODE::KEY_D)) {
		LOG("D");
		ComponentTransform* transform = gameObject->GetComponent<ComponentTransform>();
		float3 newPosition = transform->GetPosition();
		newPosition.x += speed * GameplaySystems::GetDeltaTime();
		transform->SetPosition(newPosition);
	}
}