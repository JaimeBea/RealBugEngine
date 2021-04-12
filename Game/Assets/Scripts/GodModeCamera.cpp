#include "GodModeCamera.h"

#include "GameObject.h"
#include "Modules/ModuleCamera.h"
#include "GameplaySystems.h"

#include "Math/float3x3.h"
#include "Geometry/frustum.h"

GENERATE_BODY_IMPL(GodModeCamera);

void GodModeCamera::Start() {
	godCamera = GameplaySystems::GetGameObject("Game Camera");
	transform = godCamera->GetComponent<ComponentTransform>();
	frustum = godCamera->GetComponent<ComponentCamera>()->GetFrustum();
	speed = 10.f;
	rotationSpeedX = 10.f;
	rotationSpeedY = 10.f;
	focusDistance = 10.f;
}

void GodModeCamera::Update() {
	if (!godCamera) return;
	if (!&frustum) return;

	// Movement
	// --- Forward
	if (Input::GetKeyCode(Input::KEYCODE::KEY_W)) {
		transform->SetPosition(transform->GetPosition() + frustum->Front().Normalized() * speed * Time::GetDeltaTime());
	}
	// --- Left
	if (Input::GetKeyCode(Input::KEYCODE::KEY_A)) {
		transform->SetPosition(transform->GetPosition() + frustum->WorldRight().Normalized() * -speed * Time::GetDeltaTime());
	}
	// --- Backward
	if (Input::GetKeyCode(Input::KEYCODE::KEY_S)) {
		transform->SetPosition(transform->GetPosition() + frustum->Front().Normalized() * -speed * Time::GetDeltaTime());
	}
	// --- Right
	if (Input::GetKeyCode(Input::KEYCODE::KEY_D)) {
		transform->SetPosition(transform->GetPosition() + frustum->WorldRight().Normalized() * speed * Time::GetDeltaTime());
	}
	// --- Down
	if (Input::GetKeyCode(Input::KEYCODE::KEY_Q)) {
		transform->SetPosition(transform->GetPosition() + frustum->Up().Normalized() * -speed * Time::GetDeltaTime());
	}
	if (Input::GetKeyCode(Input::KEYCODE::KEY_E)) {
		transform->SetPosition(transform->GetPosition() + frustum->Up().Normalized() * speed * Time::GetDeltaTime());
	}

	// Rotation
	if (Input::GetMouseButton(2)) {
		
		if (Input::GetKeyCode(Input::KEYCODE::KEY_LALT)) {
			// --- Orbiting
			// TODO: focusdistance!!!
			vec oldFocus = transform->GetPosition() + transform->GetLocalMatrix().Col3(2) * focusDistance;
			Quat yIncrement = Quat::RotateY(-Input::GetMouseMotion().x * rotationSpeedY * DEGTORAD * Time::GetDeltaTime());
			Quat xIncrement = Quat::RotateAxisAngle(frustum->WorldRight().Normalized(), -Input::GetMouseMotion().y * rotationSpeedX * DEGTORAD * Time::GetDeltaTime());
			transform->SetRotation(yIncrement * xIncrement * transform->GetRotation());
			vec newFocus = transform->GetPosition() + transform->GetLocalMatrix().Col3(2) * focusDistance;
			transform->SetPosition(transform->GetPosition() + (oldFocus - newFocus));
		} else {
			// --- Panning
			Quat yIncrement = Quat::RotateY(-Input::GetMouseMotion().x * rotationSpeedY * DEGTORAD * Time::GetDeltaTime());
			Quat xIncrement = Quat::RotateAxisAngle(frustum->WorldRight().Normalized(), -Input::GetMouseMotion().y * rotationSpeedX * DEGTORAD * Time::GetDeltaTime());
			transform->SetRotation(yIncrement * xIncrement * transform->GetRotation());
		}
	}

	// Func
	// --- Show Quadtree
	if (Input::GetKeyCode(Input::KEYCODE::KEY_Q)) {
		// Show Quad func
	}
	// --- Show Bounding Boxes
	if (Input::GetKeyCode(Input::KEYCODE::KEY_B)) {
		// Show/Hide BB for all GO
	}
	// --- Show animation bones
	if (Input::GetKeyCode(Input::KEYCODE::KEY_V)) {
		// TODO: Show/Hide bones
	}
	// --- Pause Frame / Continue play
	if (Input::GetKeyCode(Input::KEYCODE::KEY_P)) {

	}
	// --- Step Frame
	if (Input::GetKeyCode(Input::KEYCODE::KEY_F)) {

	}
}

void GodModeCamera::Rotate(float2 mouseMotion) {
	Quat yIncrement = Quat::RotateY(-mouseMotion.x * rotationSpeedY * DEGTORAD * Time::GetDeltaTime());
	Quat xIncrement = Quat::RotateAxisAngle(frustum->WorldRight().Normalized(), -mouseMotion.y * rotationSpeedX * DEGTORAD * Time::GetDeltaTime());
	transform->SetRotation(yIncrement * xIncrement * transform->GetRotation());
}
/*void GodModeCamera::Rotate(const float3x3& rotationMatrix) {
	vec oldFront = frustum->Front().Normalized();
	vec oldUp = frustum->Up().Normalized();
	frustum->SetFront(rotationMatrix * oldFront);
	frustum->SetUp(rotationMatrix * oldUp);
}*/
