#include "GodModeCamera.h"

#include "GameObject.h"
#include "Modules/ModuleCamera.h"
#include "GameplaySystems.h"

#include "Math/float3x3.h"
#include "Geometry/Frustum.h"

GENERATE_BODY_IMPL(GodModeCamera);

void GodModeCamera::Start() {
	godCamera = GameplaySystems::GetGameObject("godCamera");
	frustum = godCamera->GetComponent<ComponentCamera>()->GetFrustum();
	speed = 10.f;
	rotationSpeed = 10.f;
}

void GodModeCamera::Update() {
	if (!godCamera) return;
	if (!&frustum) return;

	// Movement
	// --- Forward
	if (Input::GetKeyCode(Input::KEYCODE::KEY_W)) {
		frustum->SetPos(frustum->Pos() + frustum->Front().Normalized() * speed * Time::GetDeltaTime());
	}
	// --- Left
	if (Input::GetKeyCode(Input::KEYCODE::KEY_A)) {
		frustum->SetPos(frustum->Pos() + frustum->WorldRight().Normalized() * -speed * Time::GetDeltaTime());
	}
	// --- Backward
	if (Input::GetKeyCode(Input::KEYCODE::KEY_S)) {
		frustum->SetPos(frustum->Pos() + frustum->Front().Normalized() * -speed * Time::GetDeltaTime());
	}
	// --- Right
	if (Input::GetKeyCode(Input::KEYCODE::KEY_D)) {
		frustum->SetPos(frustum->Pos() + frustum->WorldRight().Normalized() * speed * Time::GetDeltaTime());
	}
	// --- Down
	if (Input::GetKeyCode(Input::KEYCODE::KEY_Q)) {
		frustum->SetPos(frustum->Pos() + frustum->Up().Normalized() * -speed * Time::GetDeltaTime());
	}
	if (Input::GetKeyCode(Input::KEYCODE::KEY_E)) {
		frustum->SetPos(frustum->Pos() + frustum->Up().Normalized() * speed * Time::GetDeltaTime());
	}

	// Rotation
	// --- Panning
	if (Input::GetMouseButton(3)) {
		Rotate(float3x3::RotateAxisAngle(frustum->WorldRight().Normalized(), -Input::GetMouseMotion().y * rotationSpeed * DEGTORAD));
		Rotate(float3x3::RotateY(-Input::GetMouseMotion().x * rotationSpeed * DEGTORAD));
	}
	// --- Orbit
	if (Input::GetMouseButton(3) && Input::GetKeyCode(Input::KEYCODE::KEY_LALT)) {
		vec oldFocus = frustum->Pos() + frustum->Front().Normalized();
		float3x3 rotationMatrix = float3x3::RotateAxisAngle(frustum->WorldRight().Normalized(), -Input::GetMouseMotion().y * rotationSpeed * DEGTORAD);
		Rotate(rotationMatrix);
		rotationMatrix = float3x3::RotateY(-Input::GetMouseMotion().x * rotationSpeed * DEGTORAD);
		Rotate(rotationMatrix);
		vec newFocus = frustum->Pos() + frustum->Front().Normalized();
		frustum->SetPos(frustum->Pos() + oldFocus - newFocus);
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

void GodModeCamera::Rotate(const float3x3& rotationMatrix) {
	vec oldFront = frustum->Front().Normalized();
	vec oldUp = frustum->Up().Normalized();
	frustum->SetFront(rotationMatrix * oldFront);
	frustum->SetUp(rotationMatrix * oldUp);
}
