#include "ResourceTexture.h"

#include "Globals.h"
#include "Application.h"
#include "Modules/ModuleFiles.h"

#include "Utils/Logging.h"
#include "IL/il.h"
#include "IL/ilu.h"
#include "GL/glew.h"

ResourceTexture::ResourceTexture(UID id, std::string localFilePath)
	: Resource(id, localFilePath) {}

bool ResourceTexture::Import() {
	LOG("Importing texture from path: \"%s\".", localFilePath);

	// Generate image handler
	unsigned image;
	ilGenImages(1, &image);
	DEFER {
		ilDeleteImages(1, &image);
	};

	// Load image
	ilBindImage(image);
	bool imageLoaded = ilLoadImage(localFilePath.c_str());
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

	return App->files->SaveById(id, buffer);
}

void ResourceTexture::Delete() {
}

void ResourceTexture::Load() {
}

void ResourceTexture::Unload() {
}
