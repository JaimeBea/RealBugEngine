#include "Camera.h"

#include "Utils/Logging.h"
#include "GameObject.h"
#include "GameplaySystems.h"

GENERATE_BODY_IMPL(Camera);

void Camera::Start() {
	gameObject = GameplaySystems::GetGameObject("Game Camera");
	player = GameplaySystems::GetGameObject("Hearse");
}

void Camera::Update() {
	if ((player != nullptr) && (gameObject != nullptr)) {
		ComponentTransform* transform = gameObject->GetComponent<ComponentTransform>();
		ComponentTransform* playerTransform = player->GetComponent<ComponentTransform>();
		float3 newPosition = playerTransform->GetPosition();
		newPosition.x -= 6.5f;
		newPosition.y = transform->GetPosition().y;
		newPosition.z += 0.667f;
		transform->SetPosition(newPosition);
	}
}