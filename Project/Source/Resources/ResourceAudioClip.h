#pragma once

#include "Resource.h"

#include "FileSystem/JsonValue.h"

#include "AL/al.h"
#include <string>
#include <vector>

class ResourceAudioClip : public Resource {
public:
	REGISTER_RESOURCE(ResourceAudioClip, ResourceType::AUDIO);

	void Load() override;
	void Unload() override;

public:
	ALuint buffer;
};