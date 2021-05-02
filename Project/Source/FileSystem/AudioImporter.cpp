#include "AudioImporter.h"

#include "Application.h"
#include "Resources/ResourceAudioClip.h"
#include "Modules/ModuleResources.h"
#include "Modules/ModuleFiles.h"
#include "Modules/ModuleTime.h"
#include "Globals.h"

#include "Utils/Logging.h"
#include "Utils/Buffer.h"
#include "Utils/Leaks.h"

#define JSON_TAG_RESOURCES "Resources"
#define JSON_TAG_TYPE "Type"
#define JSON_TAG_ID "Id"

bool AudioImporter::ImportAudio(const char* filePath, JsonValue jMeta) {
	LOG("Importing audio from path: \"%s\".", filePath);

	MSTimer timer;
	timer.Start();

	// Read from file
	Buffer<char> buffer = App->files->Load(filePath);
	if (buffer.Size() == 0) {
		LOG("Error loading audio %s", filePath);
		return false;
	}

	JsonValue jResources = jMeta[JSON_TAG_RESOURCES];
	JsonValue jResource = jResources[0];
	UID id = jResource[JSON_TAG_ID];
	ResourceAudioClip* resourceAudioClip = App->resources->CreateResource<ResourceAudioClip>(filePath, id ? id : GenerateUID());

	jResource[JSON_TAG_TYPE] = GetResourceTypeName(resourceAudioClip->GetType());
	jResource[JSON_TAG_ID] = resourceAudioClip->GetId();

	const std::string& resourceFilePath = resourceAudioClip->GetResourceFilePath();
	bool saved = App->files->Save(resourceFilePath.c_str(), buffer);
	if (!saved) {
		LOG("Failed to save audio resource.");
		return false;
	}

	unsigned timeMs = timer.Stop();
	LOG("audio imported in %ums", timeMs);
	return true;
}