#include "ResourceMaterial.h"

#include "Application.h"

#include "Modules/ModuleFiles.h"
#include "FileSystem/JsonValue.h"
#include "Modules/ModuleTime.h"
#include "Modules/ModuleResources.h"

#include "rapidjson/error/en.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"

#include "Utils/Logging.h"
#include "Utils/Buffer.h"
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

ResourceMaterial::ResourceMaterial(UID id, const char* assetFilePath, const char* resourceFilePath)
	: Resource(id, assetFilePath, resourceFilePath) {}

void ResourceMaterial::Load() {
	std::string filePath = GetResourceFilePath();
	LOG("Importing material from path: \"%s\".", filePath);

	MSTimer timer;
	timer.Start();

	Buffer<char> buffer = App->files->Load(filePath.c_str());
	if (buffer.Size() == 0) return;

	rapidjson::Document document;
	document.ParseInsitu<rapidjson::kParseNanAndInfFlag>(buffer.Data());
	if (document.HasParseError()) {
		LOG("Error parsing JSON: %s (offset: %u)", rapidjson::GetParseError_En(document.GetParseError()), document.GetErrorOffset());
		return;
	}
	JsonValue jMaterial(document, document);

	shader = (ResourceShader*) App->resources->GetResourceByID(jMaterial[JSON_TAG_SHADER]);

	diffuseColor = float4(jMaterial[JSON_TAG_DIFFUSE_COLOR][0], jMaterial[JSON_TAG_DIFFUSE_COLOR][1], jMaterial[JSON_TAG_DIFFUSE_COLOR][2], jMaterial[JSON_TAG_DIFFUSE_COLOR][3]);
	diffuseMap = (ResourceTexture*) App->resources->GetResourceByID(jMaterial[JSON_TAG_DIFFUSE_MAP]);

	specularColor = float4(jMaterial[JSON_TAG_SPECULAR_COLOR][0], jMaterial[JSON_TAG_SPECULAR_COLOR][1], jMaterial[JSON_TAG_SPECULAR_COLOR][2], jMaterial[JSON_TAG_SPECULAR_COLOR][3]);
	specularMap = (ResourceTexture*) App->resources->GetResourceByID(jMaterial[JSON_TAG_SPECULAR_MAP]);

	metallicMap = (ResourceTexture*) App->resources->GetResourceByID(jMaterial[JSON_TAG_METALLIC_MAP]);
	normalMap = (ResourceTexture*) App->resources->GetResourceByID(jMaterial[JSON_TAG_NORMAL_MAP]);

	smoothness = jMaterial[JSON_TAG_SMOOTHNESS];
	hasSmoothnessInAlphaChannel = jMaterial[JSON_TAG_HAS_SMOOTHNESS_IN_ALPHA_CHANNEL];

	unsigned timeMs = timer.Stop();
	LOG("Material imported in %ums", timeMs);
}

void ResourceMaterial::Unload() {
}