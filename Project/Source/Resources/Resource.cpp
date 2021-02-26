#include "Resource.h"

#include "Application.h"
#include "Modules/ModuleTime.h"

Resource::Resource(UID id_, const char* assetFilePath_, const char* resourceFilePath_)
	: id(id_)
	, assetFilePath(assetFilePath_)
	, resourceFilePath(resourceFilePath_) {}

Resource::~Resource() {}

void Resource::IncreaseReferenceCount() {
	if (referenceCount == 0) {
		Load();
	}

	referenceCount += 1;
}

void Resource::DecreaseReferenceCount() {
	referenceCount -= 1;

	if (referenceCount == 0) {
		Unload();
	}
}

UID Resource::GetId() const {
	return id;
}

const std::string& Resource::GetAssetFilePath() const {
	return assetFilePath;
}

const std::string& Resource::GetResourceFilePath() const {
	return resourceFilePath;
}

void Resource::Load() {}

void Resource::Unload() {}
