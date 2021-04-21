#include "ShaderImporter.h"

#include "Resources/ResourceShader.h"
#include "Globals.h"
#include "Application.h"
#include "Utils/Logging.h"
#include "Utils/Buffer.h"
#include "Modules/ModuleResources.h"
#include "Modules/ModuleFiles.h"
#include "Modules/ModuleTime.h"

#include "IL/il.h"
#include "IL/ilu.h"
#include "GL/glew.h"

#include "Utils/Leaks.h"

#define JSON_TAG_RESOURCES "Resources"
#define JSON_TAG_TYPE "Type"
#define JSON_TAG_ID "Id"
#define JSON_TAG_SHADER_TYPE "ShaderType"

bool ShaderImporter::ImportShader(const char* filePath, JsonValue jMeta) {
	LOG("Importing Shader from path: \"%s\".", filePath);

	// Timer to measure importing a shader
	MSTimer timer;
	timer.Start();

	// Read from file
	Buffer<char> buffer = App->files->Load(filePath);
	if (buffer.Size() == 0) {
		LOG("Error loading shader %s", filePath);
		return false;
	}

	// Create shader resource
	JsonValue jResources = jMeta[JSON_TAG_RESOURCES];
	JsonValue jResource = jResources[0];
	UID id = jResource[JSON_TAG_ID];
	ResourceShader* shaderResource = App->resources->CreateResource<ResourceShader>(filePath, id ? id : GenerateUID());

	// Add resource to meta file
	jResource[JSON_TAG_TYPE] = GetResourceTypeName(shaderResource->GetType());
	jResource[JSON_TAG_ID] = shaderResource->GetId();
	jResource[JSON_TAG_SHADER_TYPE] = (int) shaderResource->GetShaderType();

	// Save to file
	const std::string& resourceFilePath = shaderResource->GetResourceFilePath();
	bool saved = App->files->Save(resourceFilePath.c_str(), buffer);
	if (!saved) {
		LOG("Failed to save shader resource.");
		return false;
	}

	unsigned timeMs = timer.Stop();
	LOG("Shader imported in %ums", timeMs);
	return true;
}
