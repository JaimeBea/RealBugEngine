#include "FangMovement.h"

#include "Utils/Logging.h"
#include "GameObject.h"
#include "GameplaySystems.h"

GENERATE_BODY_IMPL(FangMovement);

void FangMovement::Start() {
	gameObject = GameplaySystems::GetGameObject("Fang");
	camera = GameplaySystems::GetGameObject("Game Camera");
}

void FangMovement::Update() {
	if (!gameObject) return;
	if (!camera) return;

	ComponentTransform* cameraTransform = camera->GetComponent<ComponentTransform>();
	LOG("CameraT %f ", cameraTransform->GetPosition().x);
	if (cameraTransform->GetPosition().x < cameraXPosition) {
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
}