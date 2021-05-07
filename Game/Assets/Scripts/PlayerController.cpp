#include "PlayerController.h"
#include "GameObject.h"
#include "GameplaySystems.h"
#include "Math/Quat.h"
#include "Math/float3x3.h"
#include "Math/float2.h"
#include <string>

GENERATE_BODY_IMPL(PlayerController);

void PlayerController::Start() {
	gameObject = GameplaySystems::GetGameObject("Player");
	camera = GameplaySystems::GetGameObject("Game Camera");
    if (gameObject) transform = gameObject->GetComponent<ComponentTransform>();
    if (transform) initialPosition = transform->GetPosition();
}

void PlayerController::MoveTo(MovementDirection md){
	if (transform) {
		float modifier = 1.0f;
		float3 newPosition = transform->GetPosition();
		if (Input::GetKeyCode(Input::KEYCODE::KEY_LSHIFT)) {
				modifier = 2.0f;
	    }					
		switch (md)
		{
			case MovementDirection::UP:
				newPosition.x -= movementSpeed * Time::GetDeltaTime() * modifier;
				break;
			case MovementDirection::LEFT:
				newPosition.z += movementSpeed * Time::GetDeltaTime() * modifier;
				break;
			case MovementDirection::DOWN:
				newPosition.x += movementSpeed * Time::GetDeltaTime() * modifier;
				break;
			case MovementDirection::RIGHT:
				newPosition.z -= movementSpeed * Time::GetDeltaTime() * modifier;
				break;
			default:
				return;
				break;
		}
		dashMovementDirection = md;
		transform->SetPosition(newPosition);
	}
}

void PlayerController::LookAtMouse(){
	float3 mouseWorld = Input::GetMouseWorldPosition();
	float3 forward = mouseWorld - transform->GetPosition();
	float angle = Atan2(forward.y, forward.z);
	Quat q = transform->GetRotation();
	transform->SetRotation(q.RotateAxisAngle(float3(0, 1, 0), angle));
}

void PlayerController::InitDash(){
	switch (dashMovementDirection)
	{
		case MovementDirection::UP:
			dashDirection = float3(1,0,0);
			break;
		case MovementDirection::LEFT:
			dashDirection = float3(0,0,-1);
			break;
		case MovementDirection::DOWN:
			dashDirection = float3(-1,0,0);
			break;
		case MovementDirection::RIGHT:
			dashDirection = float3(0,0,1);
			break;
		default:
			return;
			break;
	}

	dashDestination = transform->GetPosition();
	dashDestination -= dashDistance * dashDirection;
	dashCooldownRemaing = dashCooldown;
	dashInCooldown = true;
	dashing = true;
}

void PlayerController::Dash(){
	if(dashing){
		float3 newPosition = transform->GetPosition();
		newPosition -= dashSpeed * Time::GetDeltaTime() * dashDirection;
		transform->SetPosition(newPosition);
		if(	std::abs(std::abs(newPosition.x) - std::abs(dashDestination.x)) < dashError &&
		   	std::abs(std::abs(newPosition.z) - std::abs(dashDestination.z)) < dashError ){ 
		   	dashing = false;
		}
	}
}

bool const PlayerController::CanDash() const{
	return !dashing && !dashInCooldown;
}

void PlayerController::CheckCoolDowns(){
	dashCooldownRemaing -= Time::GetDeltaTime();
	if(dashCooldownRemaing <= 0.f){
		dashCooldownRemaing = 0.f;
		dashInCooldown = false;
	}
}

void PlayerController::Update() {
	if (!gameObject) return;
	if (!camera) return;
	if (!transform) return;
	CheckCoolDowns();
	ComponentTransform* cameraTransform = camera->GetComponent<ComponentTransform>();
	if (cameraTransform) {
		if (!dashing && Input::GetKeyCode(Input::KEYCODE::KEY_W)) {
			MoveTo(MovementDirection::UP);
		}
		if (!dashing && Input::GetKeyCode(Input::KEYCODE::KEY_A)) {
			MoveTo(MovementDirection::LEFT);
		}
		if (!dashing && Input::GetKeyCode(Input::KEYCODE::KEY_S)) {
			MoveTo(MovementDirection::DOWN);
		}
		if (!dashing && Input::GetKeyCode(Input::KEYCODE::KEY_D)) {
			MoveTo(MovementDirection::RIGHT);
		}		
		if (CanDash() && Input::GetKeyCode(Input::KEYCODE::KEY_SPACE)) {
			InitDash();
		}
		if(Input::GetKeyCode(Input::KEYCODE::KEY_P)){
			LookAtMouse();
			float3 mousePos = Input::GetMouseWorldPosition();
			std::string mousePosStr = "x: " + std::to_string(mousePos.x) + " y: " + std::to_string(mousePos.y) + " z: " + std::to_string(mousePos.z);
			Debug::Log(mousePosStr.c_str());
		}
		Dash();
	}
}
