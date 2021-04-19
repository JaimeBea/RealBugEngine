#pragma once

#include "Resource.h"

#include <string>
#include <vector>

class ResourceAudioClip : public Resource {
public:
	REGISTER_RESOURCE(ResourceAudioClip, ResourceType::AUDIO);

	void Load() override;
	void Unload() override;

public:
	std::vector<ALuint> p_SoundEffectBuffers;
};