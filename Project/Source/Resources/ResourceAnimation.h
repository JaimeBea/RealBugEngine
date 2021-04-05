#pragma once

#include "Resources/ResourceType.h"
#include "Resources/Resource.h"

#include "Math/float3.h"
#include "Math/Quat.h"
#include <string>
#include <unordered_map>

class ResourceAnimation : public Resource {
public:
	struct Channel {
		float3 tranlation = float3::zero;
		Quat rotation = Quat::identity;
	};

	/*
	struct MorphChannel {
		std::vector<float> wheights;
	};
	*/

	struct KeyFrameChannels {
		std::unordered_map<std::string, Channel> channels;
	};

public:
	REGISTER_RESOURCE(ResourceAnimation, ResourceType::ANIMATION);

	void Load() override;
	void Unload() override;

public:
	std::vector<KeyFrameChannels> keyFrames;

	float duration = 0.0f;
};
