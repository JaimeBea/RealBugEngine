#include "SkyboxImporter.h"
#include "Globals.h"
#include "Application.h"
#include "Utils/Logging.h"
#include "Utils/Buffer.h"
#include "Resources/ResourceSkybox.h"
#include "Modules/ModuleResources.h"
#include "Modules/ModuleFiles.h"
#include "Modules/ModuleTime.h"

#include "IL/il.h"
#include "IL/ilu.h"
#include "GL/glew.h"

#include "Utils/Leaks.h"

bool SkyboxImporter::ImportSkybox(const char* filePath, JsonValue jMeta) {
	LOG("Importing texture from path: \"%s\".", filePath);

	// Generate image handler
	unsigned image;
	ilGenImages(1, &image);
	DEFER {
		ilDeleteImages(1, &image);
	};

	// Load image
	ilBindImage(image);
	bool imageLoaded = ilLoadImage(filePath);
	if (!imageLoaded) {
		LOG("Failed to load image.");
		return false;
	}
	bool imageConverted = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	if (!imageConverted) {
		LOG("Failed to convert image.");
		return false;
	}

	// Flip image if neccessary
	ILinfo info;
	iluGetImageInfo(&info);
	if (info.Origin == IL_ORIGIN_UPPER_LEFT) {
		iluFlipImage();
	}

	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
	size_t size = ilSaveL(IL_DDS, nullptr, 0);
	if (size == 0) {
		LOG("Failed to save image.");
		return false;
	}
	Buffer<char> buffer = Buffer<char>(size);
	size = ilSaveL(IL_DDS, buffer.Data(), size);
	if (size == 0) {
		LOG("Failed to save image.");
		return false;
	}

	ResourceSkybox* resourceSkybox = App->resources->CreateResource<ResourceSkybox>(filePath);
	JsonValue jResourceIds = jMeta[JSON_TAG_RESOURCE_IDS];
	jResourceIds[0] = resourceSkybox->GetId();

	const std::string& resourceFilePath = resourceSkybox->GetResourceFilePath();
	bool saved = App->files->Save(resourceFilePath.c_str(), buffer);
	if (!saved) {
		LOG("Failed to save texture resource.");
		return false;
	}

	return true;
}