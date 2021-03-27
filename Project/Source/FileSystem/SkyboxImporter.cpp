#include "SkyboxImporter.h"

#include "Application.h"
#include "Resources/ResourceSkybox.h"
#include "Modules/ModuleResources.h"
#include "Modules/ModuleFiles.h"
#include "Modules/ModuleTime.h"
#include "Globals.h"

#include "IL/il.h"
#include "IL/ilu.h"
#include "GL/glew.h"

#include "Utils/Logging.h"
#include "Utils/Buffer.h"
#include "Utils/Leaks.h"

#define JSON_TAG_RESOURCES "Resources"
#define JSON_TAG_TYPE "Type"
#define JSON_TAG_ID "Id"

bool SkyboxImporter::ImportSkybox(const char* filePath, JsonValue jMeta) {
	LOG("Importing skybox from path: \"%s\".", filePath);

	MSTimer timer;
	timer.Start();

	// Read from file
	Buffer<char> buffer = App->files->Load(filePath);
	if (buffer.Size() == 0) {
		LOG("Error loading skybox %s", filePath);
		return false;
	}

	ResourceSkybox* resourceSkybox = App->resources->CreateResource<ResourceSkybox>(filePath);

	// Add resource to meta file
	JsonValue jResources = jMeta[JSON_TAG_RESOURCES];
	JsonValue jResource = jResources[0];
	jResource[JSON_TAG_TYPE] = GetResourceTypeName(resourceSkybox->GetType());
	jResource[JSON_TAG_ID] = resourceSkybox->GetId();

	const std::string& resourceFilePath = resourceSkybox->GetResourceFilePath();
	bool saved = App->files->Save(resourceFilePath.c_str(), buffer);
	if (!saved) {
		LOG("Failed to save skybox resource.");
		return false;
	}

	unsigned timeMs = timer.Stop();
	LOG("skybox imported in %ums", timeMs);
	return true;
}