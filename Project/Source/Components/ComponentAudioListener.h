#pragma once

#include "Component.h"

#include "AL/al.h"
#include "Math/float3.h"

class ComponentAudioListener : public Component {
public:
	REGISTER_COMPONENT(ComponentAudioListener, ComponentType::AUDIO_LISTENER, false); // Refer to ComponentType for the Constructor

	// ------- Core Functions ------ //
	void Init() override;
	void OnTransformUpdate() override;
	void OnEditorUpdate() override;
	void Save(JsonValue jComponent) const override;
	void Load(JsonValue jComponent) override;
	void DuplicateComponent(GameObject& owner) override;

private:
	float gain = 1.0f;
	float3 position = {0, 0, 0};
	float orientation[6] = {0, 0, 0, 0, 0, 0};
};