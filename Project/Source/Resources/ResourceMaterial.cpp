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

#define JSON_TAG_SHADER "Shader"
#define JSON_TAG_HAS_DIFFUSE_MAP "HasDiffuseMap"
#define JSON_TAG_DIFFUSE_COLOR "DiffuseColor"
#define JSON_TAG_DIFFUSE_MAP "DiffuseMap"
#define JSON_TAG_HAS_SPECULAR_MAP "HasSpecularMap"
#define JSON_TAG_SPECULAR_COLOR "SpecularColor"
#define JSON_TAG_SPECULAR_MAP "SpecularMap"
#define JSON_TAG_METALLIC_MAP "MetallicMap"
#define JSON_TAG_NORMAL_MAP "NormalMap"
#define JSON_TAG_SMOOTHNESS "Smoothness"
#define JSON_TAG_HAS_SMOOTHNESS_IN_ALPHA_CHANNEL "HasSmoothnessInAlphaChannel"

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

	shaderId = jMaterial[JSON_TAG_SHADER];

	hasDiffuseMap = jMaterial[JSON_TAG_HAS_DIFFUSE_MAP];
	diffuseColor = float4(jMaterial[JSON_TAG_DIFFUSE_COLOR][0], jMaterial[JSON_TAG_DIFFUSE_COLOR][1], jMaterial[JSON_TAG_DIFFUSE_COLOR][2], jMaterial[JSON_TAG_DIFFUSE_COLOR][3]);
	diffuseMapId = jMaterial[JSON_TAG_DIFFUSE_MAP];

	hasSpecularMap = jMaterial[JSON_TAG_HAS_SPECULAR_MAP];
	specularColor = float4(jMaterial[JSON_TAG_SPECULAR_COLOR][0], jMaterial[JSON_TAG_SPECULAR_COLOR][1], jMaterial[JSON_TAG_SPECULAR_COLOR][2], jMaterial[JSON_TAG_SPECULAR_COLOR][3]);
	specularMapId = jMaterial[JSON_TAG_SPECULAR_MAP];

	metallicMapId = jMaterial[JSON_TAG_METALLIC_MAP];
	normalMapId = jMaterial[JSON_TAG_NORMAL_MAP];

	smoothness = jMaterial[JSON_TAG_SMOOTHNESS];
	hasSmoothnessInAlphaChannel = jMaterial[JSON_TAG_HAS_SMOOTHNESS_IN_ALPHA_CHANNEL];

	unsigned timeMs = timer.Stop();
	LOG("Material imported in %ums", timeMs);
}

void ResourceMaterial::Unload() {
}

void ResourceMaterial::SaveToFile(const char* filePath) {
	LOG("Saving material to path: \"%s\".", filePath);

	MSTimer timer;
	timer.Start();

	// Create document
	rapidjson::Document document;
	JsonValue jMaterial(document, document);

	// Save JSON values
	jMaterial[JSON_TAG_SHADER] = shaderId;

	jMaterial[JSON_TAG_HAS_DIFFUSE_MAP] = hasDiffuseMap;
	JsonValue jDiffuseColor = jMaterial[JSON_TAG_DIFFUSE_COLOR];
	jDiffuseColor[0] = diffuseColor.x;
	jDiffuseColor[1] = diffuseColor.y;
	jDiffuseColor[2] = diffuseColor.z;
	jDiffuseColor[3] = diffuseColor.w;
	jMaterial[JSON_TAG_DIFFUSE_MAP] = diffuseMapId;

	jMaterial[JSON_TAG_HAS_SPECULAR_MAP] = hasSpecularMap;
	JsonValue jSpecularColor = jMaterial[JSON_TAG_SPECULAR_COLOR];
	jSpecularColor[0] = specularColor.x;
	jSpecularColor[1] = specularColor.y;
	jSpecularColor[2] = specularColor.z;
	jSpecularColor[3] = specularColor.w;
	jMaterial[JSON_TAG_SPECULAR_MAP] = diffuseMapId;

	jMaterial[JSON_TAG_METALLIC_MAP] = metallicMapId;
	jMaterial[JSON_TAG_NORMAL_MAP] = normalMapId;

	jMaterial[JSON_TAG_SMOOTHNESS] = smoothness;
	jMaterial[JSON_TAG_HAS_SMOOTHNESS_IN_ALPHA_CHANNEL] = hasSmoothnessInAlphaChannel;

	// Write document to buffer
	rapidjson::StringBuffer stringBuffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer, rapidjson::UTF8<>, rapidjson::UTF8<>, rapidjson::CrtAllocator, rapidjson::kWriteNanAndInfFlag> writer(stringBuffer);
	document.Accept(writer);

	// Save to file
	bool saved = App->files->Save(filePath, stringBuffer.GetString(), stringBuffer.GetSize());
	if (!saved) {
		LOG("Failed to save material resource.");
		return;
	}

	unsigned timeMs = timer.Stop();
	LOG("Material saved in %ums", timeMs);
}