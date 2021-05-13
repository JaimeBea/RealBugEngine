#include "PlayerController.h"
#include "GameObject.h"
#include "GameplaySystems.h"
#include "TesseractEvent.h"
#include "Math/Quat.h"
#include "Geometry/Plane.h"
#include "Geometry/Frustum.h"
#include "Geometry/LineSegment.h"
#include "Math/float3x3.h"
#include "Math/float2.h"
#include <algorithm>
#include <string>

#define PI 3.14159

EXPOSE_MEMBERS(PlayerController) {
	// Add members here to expose them to the engine. Example:
	MEMBER(MemberType::GAME_OBJECT_UID, fangUID),
	MEMBER(MemberType::GAME_OBJECT_UID, onimaruUID),
	MEMBER(MemberType::GAME_OBJECT_UID, mainNodeUID),
	MEMBER(MemberType::GAME_OBJECT_UID, cameraUID),
	MEMBER(MemberType::GAME_OBJECT_UID, fangParticleUID),
	MEMBER(MemberType::GAME_OBJECT_UID, onimaruParticleUID),
	MEMBER(MemberType::FLOAT, switchCooldown),
	MEMBER(MemberType::FLOAT, dashCooldown),
	MEMBER(MemberType::FLOAT, dashSpeed),
	MEMBER(MemberType::FLOAT, dashDistance),
	MEMBER(MemberType::FLOAT, cameraOffsetZ),
	MEMBER(MemberType::FLOAT, cameraOffsetY),
	MEMBER(MemberType::FLOAT, movementSpeed),
	MEMBER(MemberType::FLOAT, timeToShoot)
};

GENERATE_BODY_IMPL(PlayerController);

void PlayerController::Start() {
	gameObject = GameplaySystems::GetGameObject(mainNodeUID);
	fang = GameplaySystems::GetGameObject(fangUID);
	onimaru = GameplaySystems::GetGameObject(onimaruUID);
	camera = GameplaySystems::GetGameObject(cameraUID);
	//animation
	fangParticle = GameplaySystems::GetGameObject(fangParticleUID);
	onimaruParticle = GameplaySystems::GetGameObject(onimaruParticleUID);

	if (gameObject) {
		transform = gameObject->GetComponent<ComponentTransform>();
		audioSource = gameObject->GetComponent<ComponentAudioSource>();
	}
	if (camera) {
		compCamera = camera->GetComponent<ComponentCamera>();
		if (compCamera) GameplaySystems::SetRenderCamera(compCamera);
	}
	if (transform) {
		initialPosition = transform->GetPosition();
	}
	if (fang) {
		fang->Enable();
		fangAnimation = fang->GetComponent<ComponentAnimation>();
		if(fangAnimation){
			fangCurrentState = fangAnimation->GetCurrentState();
			fangAnimation->SendTrigger("RunBackwardHurt");
		}
	}
	if (onimaru) {
		onimaru->Disable();
		onimaruAnimation = onimaru->GetComponent<ComponentAnimation>();
		if(onimaruAnimation){
			onimaruCurrentState = onimaruAnimation->GetCurrentState();
			onimaruAnimation->SendTrigger("");
		}
	}
	if(fangParticle){
		fangCompParticle = fangParticle->GetComponent<ComponentParticleSystem>();
	}
	if(onimaruParticle){
		onimaruCompParticle = onimaruParticle->GetComponent<ComponentParticleSystem>();
	}

}

void PlayerController::MoveTo(MovementDirection md) {
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

void PlayerController::LookAtMouse() {
	float2 mousePos = Input::GetMousePositionNormalized();
	LineSegment ray = compCamera->frustum.UnProjectLineSegment(mousePos.x, mousePos.y);
	float3 cameraGlobalPos = camera->GetComponent<ComponentTransform>()->GetGlobalPosition();
	Plane p = Plane(transform->GetGlobalPosition(), float3(0, 1, 0));
	facePointDir = float3(0, 0, 0);
	cameraGlobalPos.z = 0;
	facePointDir = p.ClosestPoint(ray) - (transform->GetGlobalPosition());
	Quat quat = transform->GetRotation();
	float angle = Atan2(facePointDir.x, facePointDir.z);
	Quat rotation = quat.RotateAxisAngle(float3(0, 1, 0), angle);
	transform->SetRotation(rotation);
}

float3 PlayerController::GetDirection(MovementDirection md) const {
	float3 direction = float3(0, 0, 0);
	switch (md)
	{
	case MovementDirection::UP:
		direction = float3(0, 0, -1);
		break;
	case MovementDirection::UP_LEFT:
		direction = float3(-1, 0, -1);
		break;
	case MovementDirection::UP_RIGHT:
		direction = float3(1, 0, -1);
		break;
	case MovementDirection::DOWN:
		direction = float3(0, 0, 1);
		break;
	case MovementDirection::DOWN_LEFT:
		direction = float3(-1, 0, 1);
		break;
	case MovementDirection::DOWN_RIGHT:
		direction = float3(1, 0, 1);
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

void PlayerController::InitDash(MovementDirection md) {
	dashDirection = GetDirection(md);
	dashMovementDirection = md;
	dashDestination = transform->GetPosition();
	dashDestination += dashDistance * dashDirection;
	dashCooldownRemaing = dashCooldown;
	dashInCooldown = true;
	dashing = true;
}

void PlayerController::Dash() {
	if (dashing) {
		float3 newPosition = transform->GetPosition();
		newPosition += dashSpeed * Time::GetDeltaTime() * dashDirection;
		transform->SetPosition(newPosition);
		if (std::abs(std::abs(newPosition.x) - std::abs(dashDestination.x)) < dashError &&
			std::abs(std::abs(newPosition.z) - std::abs(dashDestination.z)) < dashError) {
			dashing = false;
		}
	}
}

bool PlayerController::CanDash() {
	return !dashing && !dashInCooldown;
}

void PlayerController::CheckCoolDowns() {
	dashCooldownRemaing -= Time::GetDeltaTime();
	switchCooldownRemaing -= Time::GetDeltaTime();

	if (dashCooldownRemaing <= 0.f) {
		dashCooldownRemaing = 0.f;
		dashInCooldown = false;
		dashMovementDirection = MovementDirection::NONE;
	}

	if (switchCooldownRemaing <= 0.f) {
		switchCooldownRemaing = 0.f;
		switchInCooldown = false;
	}
}

bool PlayerController::CanSwitch() {
	return !switchInCooldown;
}

void PlayerController::SwitchCharacter() {
	if (!fang) return;
	if (!onimaru) return;
	if (CanSwitch()) {
		switchInCooldown = true;
		if (fang->IsActive()) {
			Debug::Log("Swaping to onimaru...");
			fang->Disable();
			onimaru->Enable();
		}
		else {
			Debug::Log("Swaping to fang...");
			onimaru->Disable();
			fang->Enable();
		}
		switchCooldownRemaing = switchCooldown;
	}
}

void PlayerController::ReceiveEvent(TesseractEvent& e){

}

MovementDirection PlayerController::GetInputMovementDirection() const{
	
	MovementDirection md = MovementDirection::NONE;
	if (Input::GetKeyCode(Input::KEYCODE::KEY_W)) {
		md = MovementDirection::UP;
	}
	if (Input::GetKeyCode(Input::KEYCODE::KEY_S)) {
		md = MovementDirection::DOWN;
	}
	if (Input::GetKeyCode(Input::KEYCODE::KEY_A)) {
		if (md == MovementDirection::UP) md = MovementDirection::UP_LEFT;
		else if (md == MovementDirection::DOWN) md = MovementDirection::DOWN_LEFT;
		else md = MovementDirection::LEFT;
	}
	if (Input::GetKeyCode(Input::KEYCODE::KEY_D)) {
		if (md == MovementDirection::UP) md = MovementDirection::UP_RIGHT;
		else if (md == MovementDirection::DOWN) md = MovementDirection::DOWN_RIGHT;
		else md = MovementDirection::RIGHT;
	}
	return md;
}

int PlayerController::GetMouseDirectionState(MovementDirection input){
	float3 inputDirection = GetDirection(input);
	float dot = Dot(inputDirection.Normalized() , facePointDir.Normalized());
	Debug::Log(std::to_string(sin(acos(dot))).c_str());
	if(sin(acos(dot)) >= 0){
		if(cos(7 * PI/4) < dot && dot <= cos(PI/4)){
			return 2; //RunForward
		}else if(cos(PI/4) < dot && dot <= cos(3 * PI/4)){
			return 4; //RunRight  V
		}
	}else 
	{
		if(cos(3 * PI/4) < dot && dot <= cos(5 * PI/4)){
			return 1; //RunBackward V
		}else if(cos(5 * PI/4) < dot && dot <= cos(7 * PI/4)){
			return 3; //RunLeft
		}
	}
	return 0;
}

void PlayerController::PlayAnimation(MovementDirection md, bool isFang){
	
	ComponentAnimation* animation = nullptr;
	State* currentState = nullptr;

	if(isFang){
		animation = fangAnimation;
		currentState = fangCurrentState;
	}else{
		animation = onimaruAnimation;
		currentState = onimaruCurrentState;
	}
	if (currentState != animation->GetCurrentState()){
		currentState = animation->GetCurrentState();
	}
	int dashAnimation = 0;
	if (dashing){
		dashAnimation = 4;
		md = dashMovementDirection;
	}
	switch(md){
		case MovementDirection::NONE:
			animation->SendTrigger(currentState->name + PlayerController::states[0]);
			break;
		case MovementDirection::LEFT:
			animation->SendTrigger(currentState->name + PlayerController::states[GetMouseDirectionState(md) + dashAnimation]);
			break;
		case MovementDirection::RIGHT:
			animation->SendTrigger(currentState->name + PlayerController::states[GetMouseDirectionState(md) + dashAnimation]);
			break;
		case MovementDirection::UP:
			animation->SendTrigger(currentState->name + PlayerController::states[GetMouseDirectionState(md) + dashAnimation]);
			break;
		case MovementDirection::DOWN:
			animation->SendTrigger(currentState->name + PlayerController::states[GetMouseDirectionState(md) + dashAnimation]);
			break;
	}

}

void PlayerController::Update() {
	if (!gameObject) return;
	if (!camera) return;
	if (!transform) return;
	if (!fangCompParticle) return;
	if (!onimaruCompParticle) return;
	if (!fangParticle) return;
	if (!onimaruParticle) return;
	if (!audioSource) return;

	CheckCoolDowns();
	ComponentTransform* cameraTransform = camera->GetComponent<ComponentTransform>();
	gameObject = GameplaySystems::GetGameObject(mainNodeUID);
	cameraTransform->SetPosition(float3(transform->GetGlobalPosition().x, 
										transform->GetGlobalPosition().y + cameraOffsetY, 
									   (transform->GetGlobalPosition().z + cameraOffsetZ)));
	if (cameraTransform) {
		MovementDirection md = MovementDirection::NONE;
		md = GetInputMovementDirection();
		if (CanDash() && Input::GetKeyCode(Input::KEYCODE::KEY_SPACE)) {
			InitDash(md);
		}
		Dash();

		if (!dashing) {
			LookAtMouse();
			if (md != MovementDirection::NONE) {
				MoveTo(md);
			}
			if (CanSwitch() && Input::GetKeyCode(Input::KEYCODE::KEY_T)) {
				SwitchCharacter();
			}
		}
		PlayAnimation(md,fang);
	}
	if(timeRestToShoot <= 0){
		if(Input::GetMouseButtonRepeat(0)){
			audioSource->Play();
			timeRestToShoot = timeToShoot;
			if(fang->IsActive()){
				fangCompParticle->Play();
			}else{
				onimaruCompParticle->Play();
			}
		}
	}else{
		timeRestToShoot -= Time::GetDeltaTime();
	}

}