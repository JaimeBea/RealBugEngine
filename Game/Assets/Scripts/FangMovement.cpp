#include "FangMovement.h"

#include "GameObject.h"
#include "GameplaySystems.h"

EXPOSE_MEMBERS(FangMovement) {
	// Add members here to expose them to the engine. Example:
	// MEMBER(MemberType::BOOL, exampleMember1),
	// MEMBER(MemberType::PREFAB_RESOURCE_UID, exampleMember2),
	// MEMBER(MemberType::GAME_OBJECT_UID, exampleMember3)
};

GENERATE_BODY_IMPL(FangMovement);

void FangMovement::Start() {
	gameObject = GameplaySystems::GetGameObject("Fang");
	camera = GameplaySystems::GetGameObject("Game Camera");
	Debug::Log("Fang movement Start Log");
}

void FangMovement::Update() {
	if (!gameObject) return;
	if (!camera) return;

	ComponentTransform* cameraTransform = camera->GetComponent<ComponentTransform>();
	if (cameraTransform) {
		float modifier = 1.0f;
		if (cameraTransform->GetPosition().x < cameraXPosition) {
			if (Input::GetKeyCode(Input::KEYCODE::KEY_LSHIFT)) {
				modifier = 2.0f;
			}
			if (Input::GetKeyCode(Input::KEYCODE::KEY_W)) {
				ComponentTransform* transform = gameObject->GetComponent<ComponentTransform>();
				if (transform) {
					float3 newPosition = transform->GetPosition();
					newPosition.z -= speed * Time::GetDeltaTime() * modifier;
					transform->SetPosition(newPosition);
				}
			}
			if (Input::GetKeyCode(Input::KEYCODE::KEY_A)) {
				ComponentTransform* transform = gameObject->GetComponent<ComponentTransform>();
				if (transform) {
					float3 newPosition = transform->GetPosition();
					newPosition.x -= speed * Time::GetDeltaTime() * modifier;
					transform->SetPosition(newPosition);
				}
			}
			if (Input::GetKeyCode(Input::KEYCODE::KEY_S)) {
				ComponentTransform* transform = gameObject->GetComponent<ComponentTransform>();
				if (transform) {
					float3 newPosition = transform->GetPosition();
					newPosition.z += speed * Time::GetDeltaTime() * modifier;
					transform->SetPosition(newPosition);
				}
			}
			if (Input::GetKeyCode(Input::KEYCODE::KEY_D)) {
				ComponentTransform* transform = gameObject->GetComponent<ComponentTransform>();
				if (transform) {
					float3 newPosition = transform->GetPosition();
					newPosition.x += speed * Time::GetDeltaTime() * modifier;
					transform->SetPosition(newPosition);
				}
			}
		}
	}
}