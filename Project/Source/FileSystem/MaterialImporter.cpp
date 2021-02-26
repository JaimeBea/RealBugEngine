#include "MaterialImporter.h"

#include "Utils/Logging.h"
#include "Utils/MSTimer.h"
#include "Utils/Buffer.h"

#include "Application.h"
#include "Modules/ModuleResources.h"
#include "Resources/ResourceMaterial.h"

#include "Modules/ModuleFiles.h"
#include "Modules/ModuleTime.h"

#include <assimp/material.h>
#include "rapidjson/error/en.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"

#include "Utils/Leaks.h"

#define JSON_TAG_SHADER "shader"
#define JSON_TAG_DIFFUSE_COLOR "diffuseColor"
#define JSON_TAG_DIFFUSE_MAP "diffuseMap"
#define JSON_TAG_SPECULAR_COLOR "specularColor"
#define JSON_TAG_SPECULAR_MAP "specularMap"
#define JSON_TAG_METALLIC_MAP "metallicMap"
#define JSON_TAG_NORMAL_MAP "normalMap"
#define JSON_TAG_SMOOTHNESS "smoothness"
#define JSON_TAG_HAS_SMOOTHNESS_IN_ALPHA_CHANNEL "hasSmoothnessInAlphaChannel"

bool MaterialImporter::ImportMaterial(const char* filePath, JsonValue jMeta) {
	LOG("Importing material from path: \"%s\".", filePath);

	MSTimer timer;
	timer.Start();

	Buffer<char> buffer = App->files->Load(filePath);
	if (buffer.Size() == 0) return false;

	rapidjson::Document document;

	document.ParseInsitu<rapidjson::kParseNanAndInfFlag>(buffer.Data());
	if (document.HasParseError()) {
		LOG("Error parsing JSON: %s (offset: %u)", rapidjson::GetParseError_En(document.GetParseError()), document.GetErrorOffset());
		return false;
	}

	// Material resource creation
	ResourceMaterial* textureMaterial = App->resources->CreateResource<ResourceMaterial>(filePath);
	JsonValue jResourceIds = jMeta[JSON_TAG_RESOURCE_IDS];
	jResourceIds[0] = textureMaterial->GetId();

	const std::string& resourceFilePath = textureMaterial->GetResourceFilePath();

	rapidjson::StringBuffer stringBuffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer, rapidjson::UTF8<>, rapidjson::UTF8<>, rapidjson::CrtAllocator, rapidjson::kWriteNanAndInfFlag> writer(stringBuffer);
	document.Accept(writer);

	// Save to file
	bool saved = App->files->Save(resourceFilePath.c_str(), stringBuffer.GetString(), stringBuffer.GetSize());

	if (!saved) {
		LOG("Failed to save material resource.");
		return false;
	}

	jMeta[JSON_TAG_TIMESTAMP] = App->time->GetCurrentTimestamp();

	unsigned timeMs = timer.Stop();
	LOG("Material imported in %ums", timeMs);

	return false;
}