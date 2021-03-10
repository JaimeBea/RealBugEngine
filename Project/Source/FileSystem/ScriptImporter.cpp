#include "ScriptImporter.h"

#include "Application.h"
#include "Resources/ResourceScript.h"

#include "Globals.h"
#include "Utils/Logging.h"
#include "Utils/Buffer.h"

#include "Modules/ModuleResources.h"
#include "Modules/ModuleFiles.h"
#include "Modules/ModuleTime.h"

#include "rapidjson/error/en.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"

#include "Utils/Leaks.h"

bool ScriptImporter::ImportScript(const char* filePath, JsonValue jMeta) {
	LOG("Importing Script from path: \"%s\".", filePath);

	MSTimer timer;
	timer.Start();
	Buffer<char> buffer = App->files->Load(filePath);
	if (buffer.Size() == 0) return false;

	ResourceScript* scriptResource = App->resources->CreateResource<ResourceScript>(filePath);
	JsonValue jResourceIds = jMeta[JSON_TAG_RESOURCE_IDS];
	jResourceIds[0] = scriptResource->GetId();

	const std::string& resourceFilePath = scriptResource->GetResourceFilePath();
	bool saved = App->files->Save(resourceFilePath.c_str(), buffer);
	if (!saved) {
		LOG("Failed to save script resource.");
		return false;
	}

	jMeta[JSON_TAG_TIMESTAMP] = App->time->GetCurrentTimestamp();

	unsigned timeMs = timer.Stop();
	LOG("Script imported in %ums", timeMs);
	return true;
}