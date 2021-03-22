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

bool SkyboxImporter::ImportSkybox(const char* filePath, JsonValue jMeta) {
	LOG("Importing skybox from path: \"%s\".", filePath);

	MSTimer timer;
	timer.Start();
	Buffer<char> buffer = App->files->Load(filePath);
	if (buffer.Size() == 0) return false;

	ResourceSkybox* resourceSkybox = App->resources->CreateResource<ResourceSkybox>(filePath);
	JsonValue jResourceIds = jMeta[JSON_TAG_RESOURCE_IDS];
	jResourceIds[0] = resourceSkybox->GetId();

	const std::string& resourceFilePath = resourceSkybox->GetResourceFilePath();
	bool saved = App->files->Save(resourceFilePath.c_str(), buffer);
	if (!saved) {
		LOG("Failed to save skybox resource.");
		return false;
	}

	jMeta[JSON_TAG_TIMESTAMP] = App->time->GetCurrentTimestamp();

	unsigned timeMs = timer.Stop();
	LOG("skybox imported in %ums", timeMs);
	resourceSkybox->Load();
	return true;
}