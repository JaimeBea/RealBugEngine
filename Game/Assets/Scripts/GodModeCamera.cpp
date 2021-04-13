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
	if (Input::GetMouseButton(2)) { // TODO: Why a 2?! It should be a 3!
		if (Input::GetKeyCode(Input::KEYCODE::KEY_LALT)) {
			// --- Orbiting
			vec oldFocus = transform->GetPosition() + transform->GetLocalMatrix().Col3(2) * focusDistance;
			Rotate(Input::GetMouseMotion());
			vec newFocus = transform->GetPosition() + transform->GetLocalMatrix().Col3(2) * focusDistance;
			transform->SetPosition(transform->GetPosition() + (oldFocus - newFocus));
		} else {
			// --- Panning
			Rotate(Input::GetMouseMotion());
		}
	}

	// Func
	// --- Show/Hide DebugDraw
	if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_Z)) {
		Debug::ToggleDebugDraw();
	}
	// --- Show/Hide Skybox
	if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_X)) {
		Debug::ToggleDrawSkybox();
	}
	// --- Show/Hide Quadtree
	if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_C)) {
		Debug::ToggleDrawQuadtree();
	}
	// --- Show/Hide Bounding Boxes
	if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_V)) {
		Debug::ToggleDrawBBoxes();
	}
	// --- Show/Hide Animation Bones
	if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_B)) {
		Debug::ToggleDrawAnimationBones();
	}
	// --- Show/Hide All Light Gizmos
	if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_N)) {
		Debug::ToggleDrawLightGizmos();
	}
	// --- Show/Hide All Camera Frustums
	if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_M)) {
		Debug::ToggleDrawCameraFrustums();
	}
}

void GodModeCamera::Rotate(float2 mouseMotion) {
	Quat yIncrement = Quat::RotateY(-mouseMotion.x * rotationSpeedY * DEGTORAD * Time::GetDeltaTime());
	Quat xIncrement = Quat::RotateAxisAngle(frustum->WorldRight().Normalized(), -mouseMotion.y * rotationSpeedX * DEGTORAD * Time::GetDeltaTime());
	transform->SetRotation(yIncrement * xIncrement * transform->GetRotation());
}
