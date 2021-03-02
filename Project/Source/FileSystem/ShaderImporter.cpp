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

#include "rapidjson/error/en.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"

#include "Utils/Leaks.h"

bool ShaderImporter::ImportShader(const char* filePath, JsonValue jMeta) {
	// TODO: (Shader resource) Import shaders to a custom resource file
	LOG("Importing texture from path: \"%s\".", filePath);

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

	ResourceShader* shaderResource = App->resources->CreateResource<ResourceShader>(filePath);
	JsonValue jResourceIds = jMeta[JSON_TAG_RESOURCE_IDS];
	jResourceIds[0] = shaderResource->GetId();

	const std::string& resourceFilePath = shaderResource->GetResourceFilePath();
	bool saved = App->files->Save(resourceFilePath.c_str(), buffer);
	if (!saved) {
		LOG("Failed to save shader resource.");
		return false;
	}

	jMeta[JSON_TAG_TIMESTAMP] = App->time->GetCurrentTimestamp();

	unsigned timeMs = timer.Stop();
	LOG("Shader imported in %ums", timeMs);
	return true;
}
