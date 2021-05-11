#include "PlayerController.h"
#include "GameObject.h"
#include "GameplaySystems.h"
#include "Math/Quat.h"
#include "Geometry/Plane.h"
#include "Geometry/Frustum.h"
#include "Geometry/LineSegment.h"
#include "Math/float3x3.h"
#include "Math/float2.h"
#include <algorithm>
#include <string>

EXPOSE_MEMBERS(PlayerController) {
	// Add members here to expose them to the engine. Example:
	MEMBER(MemberType::GAME_OBJECT_UID, gameObjectUID),
	MEMBER(MemberType::GAME_OBJECT_UID, cameraUID)
};

GENERATE_BODY_IMPL(PlayerController);

void PlayerController::Start() {
	gameObject = GameplaySystems::GetGameObject(gameObjectUID);
	camera = GameplaySystems::GetGameObject(cameraUID);
    if (gameObject) transform = gameObject->GetComponent<ComponentTransform>();
	if (camera) {
		compCamera = camera->GetComponent<ComponentCamera>();
		if (compCamera) GameplaySystems::SetRenderCamera(compCamera);
	}
    if (transform) initialPosition = transform->GetPosition();
	
}

void PlayerController::MoveTo(MovementDirection md){
	if (transform) {
		float modifier = 1.0f;
		float3 newPosition = transform->GetPosition();
		if (Input::GetKeyCode(Input::KEYCODE::KEY_LSHIFT)) {
				modifier = 2.0f;
	    }
		newPosition += GetDirection(md) * movementSpeed * Time::GetDeltaTime() * modifier;
		transform->SetPosition(newPosition);
	}
}

void PlayerController::LookAtMouse(){
	float2 mousePos = Input::GetMousePositionNormalized();
	LineSegment ray = compCamera->frustum.UnProjectLineSegment(mousePos.x, mousePos.y);
	float3 cameraGlobalPos = camera->GetComponent<ComponentTransform>()->GetGlobalPosition();
	Plane p = Plane(transform->GetGlobalPosition(), float3(0, 1, 0));
	float dist;
	float3 facePoint = float3(0,0,0);
	if (p.Intersects(ray, &dist)) {
		facePoint = ray.GetPoint(dist) - (transform->GetGlobalPosition() - cameraGlobalPos);
		Quat quat = transform->GetRotation();
		float angle = Atan2(facePoint.x, facePoint.z);
		Quat rotation = quat.RotateAxisAngle(float3(0, 1, 0), angle);
		transform->SetRotation(rotation);
	}
}

float3 PlayerController::GetDirection(MovementDirection md) {
	float3 direction = float3(0, 0, 0);
	switch (md)
	{
	case MovementDirection::UP:
		direction = float3(0, 0, 1);
		break;
	case MovementDirection::UP_LEFT:
		direction = float3(-1, 0, 1);
		break;
	case MovementDirection::UP_RIGHT:
		direction = float3(1, 0, 1);
		break;
	case MovementDirection::DOWN:
		direction = float3(0, 0, -1);
		break;
	case MovementDirection::DOWN_LEFT:
		direction = float3(-1, 0, -1);
		break;
	case MovementDirection::DOWN_RIGHT:
		direction = float3(1, 0, -1);
		break;
	case MovementDirection::RIGHT:
		direction = float3(1, 0, 0);
		break;
	case MovementDirection::LEFT:
		direction = float3(-1, 0, 0);
		break;
	default:
		break;
	}
	return direction;
}
void PlayerController::InitDash(MovementDirection md){
	dashDirection = GetDirection(md);
	dashDestination = transform->GetPosition();
	dashDestination += dashDistance * dashDirection;
	dashCooldownRemaing = dashCooldown;
	dashInCooldown = true;
	dashing = true;
}

void PlayerController::Dash(){
	if(dashing){
		float3 newPosition = transform->GetPosition();
		newPosition += dashSpeed * Time::GetDeltaTime() * dashDirection;
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
		MovementDirection md = MovementDirection::NONE;
		
		if (!dashing && Input::GetKeyCode(Input::KEYCODE::KEY_W)) {
			md = MovementDirection::UP;
		}
		if (!dashing && Input::GetKeyCode(Input::KEYCODE::KEY_S)) {
			md = MovementDirection::DOWN;
		}
		if (!dashing && Input::GetKeyCode(Input::KEYCODE::KEY_A)) {
			if (md == MovementDirection::UP) md = MovementDirection::UP_LEFT;
			else if(md == MovementDirection::DOWN) md = MovementDirection::DOWN_LEFT;
			else md = MovementDirection::LEFT;
		}
		if (!dashing && Input::GetKeyCode(Input::KEYCODE::KEY_D)) {
			if (md == MovementDirection::UP) md = MovementDirection::UP_RIGHT;
			else if (md == MovementDirection::DOWN) md = MovementDirection::DOWN_RIGHT;
			else md = MovementDirection::RIGHT;
		}
		if (md != MovementDirection::NONE) {
			MoveTo(md);
		}
		if (CanDash() && Input::GetKeyCode(Input::KEYCODE::KEY_SPACE)) {
			InitDash(md);
		}
		LookAtMouse();
		Dash();
	}
}
