#include "FontImporter.h"

#include "Application.h"
#include "Modules/ModuleFiles.h"
#include "Modules/ModuleResources.h"
#include "Resources/ResourceFont.h"
#include "Utils/Buffer.h"

#include "Utils/Logging.h"

#include "Utils/Leaks.h"

#define JSON_TAG_RESOURCES "Resources"
#define JSON_TAG_TYPE "Type"
#define JSON_TAG_ID "Id"

bool FontImporter::ImportFont(const char* filePath, JsonValue jMeta) {
	LOG("Importing font from path: \"%s\".", filePath);
	Buffer<char> buffer;
	buffer = App->files->Load(filePath);

	// Create font resource
	JsonValue jResources = jMeta[JSON_TAG_RESOURCES];
	JsonValue jResource = jResources[0];
	UID id = jResource[JSON_TAG_ID];
	ResourceFont* fontResource = App->resources->CreateResource<ResourceFont>(filePath, id ? id : GenerateUID());

	// Add resource to meta file
	jResource[JSON_TAG_TYPE] = GetResourceTypeName(fontResource->GetType());
	jResource[JSON_TAG_ID] = fontResource->GetId();

	// Save to file
	const std::string& resourceFilePath = fontResource->GetResourceFilePath();
	bool saved = App->files->Save(resourceFilePath.c_str(), buffer);
	
	if (!saved) {
		LOG("Failed to save font resource.");
		return false;
	}

	return true;
}