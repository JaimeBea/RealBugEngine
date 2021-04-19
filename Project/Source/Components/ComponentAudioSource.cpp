#include "ComponentAudioSource.h"

#include "GameObject.h"
#include "Application.h"
#include "Modules/ModuleAudio.h"
#include "Modules/ModuleEditor.h"

#include "debugdraw.h"
#include "Math/float3.h"
#include "imgui.h"
#include "Utils/ImGuiUtils.h"
#include "Utils/Leaks.h"

#define JSON_TAG_PITCH "Pitch"
#define JSON_TAG_GAIN "Gain"
#define JSON_TAG_MUTE "Mute"
#define JSON_TAG_LOOPING "Looping"
#define JSON_TAG_BUFFER_ID "BufferId"
#define JSON_TAG_SPATIAL_BLEND "SpatialBlend"
#define JSON_TAG_SOURCE_TYPE "SourceType"
#define JSON_TAG_INNER_ANGLE "InnerAngle"
#define JSON_TAG_OUTER_ANGLE "OuterAngle"
#define JSON_TAG_GAIN_ATTENUATION "GainAttenuation"

void ComponentAudioSource::Init() {
	UpdateAudioSource();
}

void ComponentAudioSource::Update() {
	UpdateAudioSource();
}

void ComponentAudioSource::DrawGizmos() {
	if (IsActiveInHierarchy() && drawGizmos) {
		if (spatialBlend && sourceType) {
			dd::cone(position, direction, dd::colors::White, 1.f, 1.f);
		} else {
			dd::sphere(position, dd::colors::White, 20.f);
		}
	}
}

void ComponentAudioSource::UpdateAudioSource() {
	ComponentTransform* transform = GetOwner().GetComponent<ComponentTransform>();
	position = transform->GetGlobalPosition();
	direction = transform->GetGlobalRotation() * float3::unitZ;
}

void ComponentAudioSource::OnEditorUpdate() {
	bool active = IsActive();
	if (ImGui::Checkbox("Active", &active)) {
		active ? Enable() : Disable();
	}
	ImGui::Separator();
	ImGui::Checkbox("Draw Gizmos", &drawGizmos);
	ImGui::Separator();
	ImGui::TextColored(App->editor->titleColor, "General Settings");

	ImGui::ResourceSlot<ResourceAudioClip>("AudioClip", &audioClipId);

	ImGui::Checkbox("Mute", &mute);
	ImGui::Checkbox("Loop", &loopSound);
	ImGui::DragFloat("Gain", &gain, App->editor->dragSpeed2f, 0, 1);
	ImGui::DragFloat("Pitch", &pitch, App->editor->dragSpeed2f, 0.5, 2);

	ImGui::Separator();
	ImGui::TextColored(App->editor->titleColor, "Position Settings");
	ImGui::Text("Spatial Blend");
	ImGui::SameLine();
	ImGui::RadioButton("2D", &spatialBlend, 0);
	ImGui::SameLine();
	ImGui::RadioButton("3D", &spatialBlend, 1);
	if (spatialBlend) {
		const char* sourceTypes[] = {"Omnidirectional", "Directional"};
		const char* sourceTypesCurrent = sourceTypes[sourceType];
		if (ImGui::BeginCombo("Source Type", sourceTypesCurrent)) {
			for (int n = 0; n < IM_ARRAYSIZE(sourceTypes); ++n) {
				bool isSelected = (sourceTypesCurrent == sourceTypes[n]);
				if (ImGui::Selectable(sourceTypes[n], isSelected)) {
					sourceType = n;
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
	}

	if (sourceType) {
		ImGui::DragFloat("Outter Angle", &outerAngle, 1.f, 0.0f, 360.0f);
		ImGui::DragFloat("Inner Angle", &innerAngle, 1.f, 0.0f, outerAngle);
		ImGui::DragFloat("Gain Attenuation", &gainAttenuation, App->editor->dragSpeed2f, 0.f, 1.f);
	}

	ImGui::Separator();

	// -- Temporally to reproduce sounds from Component. TODO: move to Resource Audio UI
	if (ImGui::Button("Play")) {
		Play();
	}
	ImGui::SameLine();
	if (ImGui::Button("Pause")) {
		Pause();
	}
	ImGui::SameLine();
	if (ImGui::Button("Stop")) {
		Stop();
	}
	// --
}

void ComponentAudioSource::UpdateSourceParameters() const {
	ResourceAudioClip* audioResource = (ResourceAudioClip*) App->resources->GetResource(audioClipId);
	if (audioResource == nullptr) return;

	alSourcef(sourceId, AL_PITCH, pitch);
	alSourcei(sourceId, AL_LOOPING, loopSound);
	alSourcei(sourceId, AL_BUFFER, audioResource->buffer);

	if (!spatialBlend) {
		alSourcei(sourceId, AL_SOURCE_RELATIVE, AL_TRUE);
		alSource3f(sourceId, AL_POSITION, 0.0f, 0.0f, 0.0f);
		alSource3f(sourceId, AL_DIRECTION, 0.0f, 0.0f, 0.0f);
	} else {
		alSourcei(sourceId, AL_SOURCE_RELATIVE, AL_FALSE);
		alSourcefv(sourceId, AL_POSITION, position.ptr());

		if (sourceType) {
			alSourcefv(sourceId, AL_DIRECTION, direction.ptr());
		} else {
			alSourcef(sourceId, AL_CONE_INNER_ANGLE, 360);
			alSource3f(sourceId, AL_DIRECTION, 0.0f, 0.0f, 0.0f);
		}
	}
	if (mute) {
		alSourcef(sourceId, AL_GAIN, 0.0f);
	} else {
		alSourcef(sourceId, AL_GAIN, gain);
	}
}

void ComponentAudioSource::Play() {
	if (IsActive()) {
		if (!sourceId) {
			sourceId = App->audio->GetAvailableSource();
			UpdateSourceParameters();
		}
		alSourcePlay(sourceId);
	}
}

void ComponentAudioSource::Stop() {
	if (isPlaying()) {
		alSourceStop(sourceId);
		sourceId = 0;
	}
}

void ComponentAudioSource::Pause() const {
	if (isPlaying()) {
		alSourcePause(sourceId);
	}
}

bool ComponentAudioSource::isPlaying() const {
	ALint state;
	alGetSourcei(sourceId, AL_SOURCE_STATE, &state);
	return (state == AL_PLAYING);
}

void ComponentAudioSource::Save(JsonValue jComponent) const {
	jComponent[JSON_TAG_PITCH] = pitch;
	jComponent[JSON_TAG_GAIN] = gain;
	jComponent[JSON_TAG_MUTE] = mute;
	jComponent[JSON_TAG_LOOPING] = loopSound;
	jComponent[JSON_TAG_BUFFER_ID] = audioClipId;
	jComponent[JSON_TAG_SPATIAL_BLEND] = spatialBlend;
	jComponent[JSON_TAG_SOURCE_TYPE] = sourceType;
	jComponent[JSON_TAG_INNER_ANGLE] = innerAngle;
	jComponent[JSON_TAG_OUTER_ANGLE] = outerAngle;
	jComponent[JSON_TAG_GAIN_ATTENUATION] = gainAttenuation;
}

void ComponentAudioSource::Load(JsonValue jComponent) {
	pitch = jComponent[JSON_TAG_PITCH];
	gain = jComponent[JSON_TAG_GAIN];
	mute = jComponent[JSON_TAG_MUTE];
	loopSound = jComponent[JSON_TAG_LOOPING];
	audioClipId = jComponent[JSON_TAG_BUFFER_ID];
	spatialBlend = jComponent[JSON_TAG_SPATIAL_BLEND];
	sourceType = jComponent[JSON_TAG_SOURCE_TYPE];
	innerAngle = jComponent[JSON_TAG_INNER_ANGLE];
	outerAngle = jComponent[JSON_TAG_OUTER_ANGLE];
	gainAttenuation = jComponent[JSON_TAG_GAIN_ATTENUATION];
}

void ComponentAudioSource::DuplicateComponent(GameObject& owner) {
	ComponentAudioSource* component = owner.CreateComponent<ComponentAudioSource>();
	component->pitch = this->pitch;
	component->gain = this->gain;
	component->mute = this->mute;
	component->loopSound = this->loopSound;
	component->audioClipId = this->audioClipId;
	component->spatialBlend = this->spatialBlend;
	component->sourceType = this->sourceType;
	component->innerAngle = this->innerAngle;
	component->outerAngle = this->outerAngle;
	component->gainAttenuation = this->gainAttenuation;
	component->gainAttenuation = this->gainAttenuation;
}