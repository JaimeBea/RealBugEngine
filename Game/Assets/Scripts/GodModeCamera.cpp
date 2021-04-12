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


	//if (Input::GetMouseButton(1)) LOG("Mouse1Pressed");
	//if (Input::GetMouseButton(2)) LOG("Mouse2Pressed");
	//if (Input::GetMouseButton(3)) LOG("Mouse3Pressed");
	//if (Input::GetMouseButton(4)) LOG("Mouse4Pressed");
	//if (Input::GetMouseButton(5)) LOG("Mouse5Pressed");


	// Rotation
	if (Input::GetMouseButton(2)) {
		
		if (Input::GetKeyCode(Input::KEYCODE::KEY_LALT)) {
			// --- Orbiting
			/*vec oldFocus = transform->GetPosition() + frustum->Front().Normalized() * 1000;
			LOG("%f,%f,%f", frustum->Front().Normalized().x, frustum->Front().Normalized().y, frustum->Front().Normalized().z);
			Quat yIncrement = Quat::RotateY(-Input::GetMouseMotion().x * rotationSpeedY * DEGTORAD * Time::GetDeltaTime());
			Quat xIncrement = Quat::RotateAxisAngle(frustum->WorldRight().Normalized(), -Input::GetMouseMotion().y * rotationSpeedX * DEGTORAD * Time::GetDeltaTime());
			transform->SetRotation(yIncrement * xIncrement * transform->GetRotation());

			vec newFocus = transform->GetPosition() + frustum->Front().Normalized() * 1000;
			LOG("%f,%f,%f", frustum->Front().Normalized().x, frustum->Front().Normalized().y, frustum->Front().Normalized().z);
			transform->SetPosition(transform->GetPosition() + (oldFocus - newFocus));*/

			/*
			float3 origin = float3(0, 0, 0);
			float3 oldPos = transform->GetPosition();
			float3 distVector = oldPos - origin;
			transform->SetPosition(origin);

			Quat yIncrement = Quat::RotateY(-Input::GetMouseMotion().x * rotationSpeedY * DEGTORAD * Time::GetDeltaTime());
			Quat xIncrement = Quat::RotateAxisAngle(frustum->WorldRight().Normalized(), -Input::GetMouseMotion().y * rotationSpeedX * DEGTORAD * Time::GetDeltaTime());
			transform->SetRotation(yIncrement * xIncrement * transform->GetRotation());
			distVector = yIncrement * xIncrement * distVector;

			transform->SetPosition(transform->GetPosition() - distVector);
			*/

			float3 origin = float3(0, 0, 0);
			float3 oldPos = transform->GetPosition();
			float3 distVector = oldPos - origin;

			float4x4 localMatrix = transform->GetLocalMatrix();
			localMatrix.SetCol3(0, float3(0, 0, 0));
			float3x3 yIncrement = float3x3::RotateY(-Input::GetMouseMotion().x * rotationSpeedY * DEGTORAD * Time::GetDeltaTime());
			float3x3 xIncrement = float3x3::RotateAxisAngle(frustum->WorldRight().Normalized(), -Input::GetMouseMotion().y * rotationSpeedX * DEGTORAD * Time::GetDeltaTime());
			
			localMatrix.SetRotatePart(yIncrement * xIncrement * transform->GetRotation());
			distVector = yIncrement * xIncrement * distVector;

			localMatrix.SetCol3(0, origin-distVector);
			transform->SetTRS(localMatrix);
			
		/*
		vec oldFocus = activeFrustum->Pos() + activeFrustum->Front().Normalized() * focusDistance;
		Rotate(float3x3::RotateAxisAngle(activeFrustum->WorldRight().Normalized(), -mouseMotion.y * rotationSpeed * DEGTORAD));
		Rotate(float3x3::RotateY(-mouseMotion.x * rotationSpeed * DEGTORAD));
		vec newFocus = activeFrustum->Pos() + activeFrustum->Front().Normalized() * focusDistance;
		activeFrustum->SetPos(activeFrustum->Pos() + (oldFocus - newFocus));
		*/
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
