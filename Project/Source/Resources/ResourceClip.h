#pragma once
#include "Resources/ResourceType.h"
#include "Resources/Resource.h"

#include "Application.h"
#include "ResourceAnimation.h"
#include "Modules/ModuleResources.h"

#include "Utils/UID.h"
#include <string>

class ResourceClip : public Resource {
public:

	REGISTER_RESOURCE(ResourceClip, ResourceType::CLIP);

	void Load() override;
	void Unload() override;
	void SaveToFile(const char* filePath);

	void Init(std::string& mName, UID mAnimationUID = 0, unsigned int mBeginIndex = 0, unsigned int mEndIndex = 0, bool mLoop = false, UID mid = 0);

	void SetBeginIndex(unsigned int index);
	void SetEndIndex(unsigned int index);

	ResourceAnimation* GetResourceAnimation();

public:
	std::string name = "";
	UID animationUID = 0;
	bool loop = false;
	unsigned int beginIndex = 0;
	unsigned int endIndex = 0;
	float duration = 0;
	unsigned int keyFramesSize = 0;
};
