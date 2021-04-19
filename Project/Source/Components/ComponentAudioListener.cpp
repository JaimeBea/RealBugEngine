#include "ComponentAudioListener.h"

#include "GameObject.h"

#include "AL/al.h"
#include "imgui.h"

#include "Utils/Leaks.h"

#define JSON_TAG_GAIN "Gain"

void ComponentAudioListener::Init() {
	alListenerf(AL_GAIN, gain);
	UpdateAudioListener();
}

void ComponentAudioListener::Update() {
	UpdateAudioListener();
}

void ComponentAudioListener::OnEditorUpdate() {
	bool active = IsActive();
	if (ImGui::Checkbox("Active", &active)) {
		active ? Enable() : Disable();

		if (IsActive()) {
			Init();
		} else {
			alListenerf(AL_GAIN, 0.f);
		}
	}
}

void ComponentAudioListener::Save(JsonValue jComponent) const {
	jComponent[JSON_TAG_GAIN] = gain;
}

void ComponentAudioListener::Load(JsonValue jComponent) {
	gain = jComponent[JSON_TAG_GAIN];
}

void ComponentAudioListener::DuplicateComponent(GameObject& owner) {
	ComponentAudioListener* component = owner.CreateComponent<ComponentAudioListener>();
	component->gain = this->gain;
}

void ComponentAudioListener::UpdateAudioListener() {
	ComponentTransform* transform = GetOwner().GetComponent<ComponentTransform>();
	position = transform->GetPosition();
	float3 directionUp = transform->GetRotation() * float3::unitY;
	float3 directionRight = transform->GetRotation() * float3::unitX;
	orientation[0] = directionUp[0];
	orientation[1] = directionUp[1];
	orientation[2] = directionUp[2];
	orientation[3] = directionRight[0];
	orientation[4] = directionRight[1];
	orientation[5] = directionRight[2];
	alListenerfv(AL_POSITION, position.ptr());
	alListenerfv(AL_ORIENTATION, orientation);
}
