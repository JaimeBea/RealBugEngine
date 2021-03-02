#pragma once

//#include "Resources/Resource.h"

#include "Math/float3.h"
#include "Math/Quat.h"
#include <string>
#include <unordered_map>

class ResourceAnimation {
public:
	struct Channel {
		float3 tranlation = float3::zero;
		Quat rotation = Quat::identity;
	};

	struct MorphChannel {
		std::vector<float> wheights;
	};

	struct KeyFrameChannels {
		std::unordered_map<std::string, Channel> channels;
	};

	//ResourceAnimation(UID id, const char* assetFilePath, const char* resourceFilePath);
	ResourceAnimation() {}

	void Load();   //override;
	void Unload(); //override;

	//void SetDuration(float duration);

public:
	std::string name = "";

	std::vector<KeyFrameChannels> keyFrames;

	unsigned int duration = 0;
};
