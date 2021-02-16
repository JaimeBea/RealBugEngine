#include "Resource.h"

#include "Application.h"
#include "Modules/ModuleTime.h"

#define JSON_TAG_ID "ID"
#define JSON_TAG_TIMESTAMP "Timestamp"

Resource::Resource(UID id_, std::string localFilePath_)
	: id(id_)
	, localFilePath(localFilePath_) {}

Resource::~Resource() {}

bool Resource::Import() {
	return true;
}

void Resource::Delete() {}

void Resource::SaveImportSettings(JsonValue jMeta) {
	jMeta[JSON_TAG_ID] = id;
	jMeta[JSON_TAG_TIMESTAMP] = App->time->GetCurrentTimestamp();
}

void Resource::LoadImportSettings(JsonValue jMeta) {
	unsigned long long id = jMeta[JSON_TAG_ID];
	id = jMeta[JSON_TAG_ID];
}

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

UID Resource::GetId() {
	return id;
}

const std::string& Resource::GetLocalFilePath() {
	return localFilePath;
}

void Resource::Load() {}

void Resource::Unload() {}
