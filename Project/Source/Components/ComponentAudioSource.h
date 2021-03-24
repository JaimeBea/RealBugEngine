#pragma once

#include "Component.h"

#include "AL/al.h"
#include "Math/float3.h"

class ComponentAudioSource : public Component {
public:
	REGISTER_COMPONENT(ComponentAudioSource, ComponentType::AUDIO_SOURCE, true); // Refer to ComponentType for the Constructor

	// ------- Core Functions ------ //
	void Init() override;
	void DrawGizmos() override;
	void OnTransformUpdate() override;
	void OnEditorUpdate() override;
	void Save(JsonValue jComponent) const override;
	void Load(JsonValue jComponent) override;
	void DuplicateComponent(GameObject& owner) override;

	void UpdateSourceParameters() const;
	void Play();
	void Stop();
	void Pause() const;
	bool isPlaying() const;

private:
	bool drawGizmos = true;

	ALuint sourceId = 0;
	float pitch = 1.f;
	float gain = 1.f;
	bool mute = false;
	float3 position = {0.f, 0.f, 0.f};
	float3 direction = {0.f, 0.f, 0.f};
	bool loopSound = false;
	ALuint bufferId = 0;
	int spatialBlend = 0;		  // 2D = 0; 3D = 1;
	int sourceType = 0;			  // Omnidirectional = 0; Directional = 1;
	float innerAngle = 90.f;	  // In Degrees
	float outerAngle = 180.f;	  // In Degrees
	float gainAttenuation = 0.2f; // Attenuation inside the Outer Angle in Directional Source
};
