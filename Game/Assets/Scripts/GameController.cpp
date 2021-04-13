#include "GameController.h"

#include "GameObject.h"
#include "Modules/ModuleCamera.h"
#include "GameplaySystems.h"

#include "Math/float3x3.h"
#include "Geometry/frustum.h"

GENERATE_BODY_IMPL(GameController);

void GameController::Start() {

	speed = 50.f;
	rotationSpeedX = 10.f;
	rotationSpeedY = 10.f;
	focusDistance = 100.f;
	showWireframe = false;


	gameCamera = GameplaySystems::GetGameObject("Game Camera");
	godCamera = GameplaySystems::GetGameObject("God Camera");
	GameplaySystems::SetRenderCamera(gameCamera);
	godCameraActive = false;
	if (gameCamera && godCamera) godModeAvailable = true;
	if (godCamera) {
		transform = godCamera->GetComponent<ComponentTransform>();
		frustum = godCamera->GetComponent<ComponentCamera>()->GetFrustum();
	}
}

void GameController::Update() {
	if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_G)) {
		if (!godModeAvailable) return;

		if (godCameraActive) {
			GameplaySystems::SetRenderCamera(gameCamera);
			godCameraActive = false;
		}
		else {
			GameplaySystems::SetRenderCamera(godCamera);
			godCameraActive = true;
		}
	}

	// Godmode Controls
	if (!transform) return;
	if (!&frustum) return;

	if (godCameraActive) {
		// Movement
		// --- Forward
		if (Input::GetKeyCode(Input::KEYCODE::KEY_UP)) {
			transform->SetPosition(transform->GetPosition() + frustum->Front().Normalized() * speed * Time::GetDeltaTime());
		}
		// --- Left
		if (Input::GetKeyCode(Input::KEYCODE::KEY_LEFT)) {
			transform->SetPosition(transform->GetPosition() + frustum->WorldRight().Normalized() * -speed * Time::GetDeltaTime());
		}
		// --- Backward
		if (Input::GetKeyCode(Input::KEYCODE::KEY_DOWN)) {
			transform->SetPosition(transform->GetPosition() + frustum->Front().Normalized() * -speed * Time::GetDeltaTime());
		}
		// --- Right
		if (Input::GetKeyCode(Input::KEYCODE::KEY_RIGHT)) {
			transform->SetPosition(transform->GetPosition() + frustum->WorldRight().Normalized() * speed * Time::GetDeltaTime());
		}
		// --- Down
		if (Input::GetKeyCode(Input::KEYCODE::KEY_COMMA)) {
			transform->SetPosition(transform->GetPosition() + frustum->Up().Normalized() * -speed * Time::GetDeltaTime());
		}
		if (Input::GetKeyCode(Input::KEYCODE::KEY_PERIOD)) {
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
			}
			else {
				// --- Panning
				Rotate(Input::GetMouseMotion());
			}
		}

		// Func
		// --- Show/Hide DebugDraw
		if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_Z)) {
			Debug::ToggleDebugDraw();
		}
		// --- Show/Hide Wireframe
		if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_X)) {
			if (showWireframe) {
				Debug::UpdateShadingMode("Shaded");
			}
			else {
				Debug::UpdateShadingMode("Wireframe");
			}
			showWireframe = !showWireframe;
		}
		// --- Show/Hide Quadtree
		if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_C)) {
			Debug::ToggleDrawQuadtree();
		}
		// --- Show/Hide Bounding Boxes
		if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_V)) {
			//Debug::ToggleDrawBBoxes(); //TODO: Disabled until better level building
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
		// --- Show/Hide Skybox
		if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_F)) {
			Debug::ToggleDrawSkybox(); // TODO: not implemented
		}
	}
}

void GameController::Rotate(float2 mouseMotion) {
	Quat yIncrement = Quat::RotateY(-mouseMotion.x * rotationSpeedY * DEGTORAD * Time::GetDeltaTime());
	Quat xIncrement = Quat::RotateAxisAngle(frustum->WorldRight().Normalized(), -mouseMotion.y * rotationSpeedX * DEGTORAD * Time::GetDeltaTime());
	transform->SetRotation(yIncrement * xIncrement * transform->GetRotation());
}
