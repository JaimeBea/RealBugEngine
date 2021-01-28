#include "TextureImporter.h"

#include "Globals.h"
#include "Application.h"
#include "Utils/Logging.h"
#include "Utils/Buffer.h"
#include "Resources/Texture.h"
#include "Resources/CubeMap.h"
#include "Modules/ModuleResources.h"
#include "Modules/ModuleFiles.h"

#include "IL/il.h"
#include "IL/ilu.h"
#include "GL/glew.h"

#include "Utils/Leaks.h"

Texture* TextureImporter::ImportTexture(const char* filePath) {
	// Timer to measure importing a texture
	MSTimer timer;
	timer.Start();

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
		return nullptr;
	}
	bool imageConverted = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	if (!imageConverted) {
		LOG("Failed to convert image.");
		return nullptr;
	}

	// Flip image if neccessary
	ILinfo info;
	iluGetImageInfo(&info);
	if (info.Origin == IL_ORIGIN_UPPER_LEFT) {
		iluFlipImage();
	}
	// Create texture
	Texture* texture = App->resources->ObtainTexture();

	// Save texture to custom DDS file
	texture->fileName = App->files->GetFileName(filePath);
	std::string ddsFilePath = std::string(TEXTURES_PATH) + "/" + texture->fileName + TEXTURE_EXTENSION;

	LOG("Saving image to \"%s\".", ddsFilePath.c_str());
	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
	size_t size = ilSaveL(IL_DDS, nullptr, 0);
	if (size == 0) {
		LOG("Failed to save image.");
		return nullptr;
	}
	Buffer<char> buffer = Buffer<char>(size);
	size = ilSaveL(IL_DDS, buffer.Data(), size);
	if (size == 0) {
		LOG("Failed to save image.");
		return nullptr;
	}
	App->files->Save(ddsFilePath.c_str(), buffer);

	unsigned timeMs = timer.Stop();
	LOG("Texture imported in %ums.", timeMs);
	return texture;
}

void TextureImporter::LoadTexture(Texture* texture) {
	if (texture == nullptr) return;

	// Timer to measure loading a texture
	MSTimer timer;
	timer.Start();

	std::string filePath = std::string(TEXTURES_PATH) + "/" + texture->fileName + TEXTURE_EXTENSION;

	LOG("Loading texture from path: \"%s\".", filePath.c_str());

	// Generate image handler
	unsigned image;
	ilGenImages(1, &image);
	DEFER {
		ilDeleteImages(1, &image);
	};

	// Load image
	ilBindImage(image);
	bool image_loaded = ilLoad(IL_DDS, filePath.c_str());
	if (!image_loaded) {
		LOG("Failed to load image.");
		return;
	}

	// Generate texture from image
	glGenTextures(1, &texture->glTexture);
	glBindTexture(GL_TEXTURE_2D, texture->glTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());

	// Generate mipmaps and set filtering and wrapping
	glGenerateMipmap(GL_TEXTURE_2D);
	App->resources->SetWrap(App->resources->GetWrap());
	App->resources->SetMinFilter(App->resources->GetMinFilter());
	App->resources->SetMagFilter(App->resources->GetMagFilter());

	unsigned timeMs = timer.Stop();
	LOG("Texture loaded in %ums.", timeMs);
}

void TextureImporter::UnloadTexture(Texture* texture) {
	if (!texture->glTexture) return;

	glDeleteTextures(1, &texture->glTexture);
}

CubeMap* TextureImporter::ImportCubeMap(const char* filePaths[6]) {
	// Create cube map
	CubeMap* cubeMap = App->resources->ObtainCubeMap();

	for (unsigned i = 0; i < 6; ++i) {
		const char* filePath = filePaths[i];

		LOG("Importing cube map texture from path: \"%s\".", filePath);

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
			return nullptr;
		}
		bool imageConverted = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
		if (!imageConverted) {
			LOG("Failed to convert image.");
			return nullptr;
		}

		// Flip image if neccessary
		ILinfo info;
		iluGetImageInfo(&info);
		if (info.Origin != IL_ORIGIN_UPPER_LEFT) // Cube maps are the inverse of textures
		{
			iluFlipImage();
		}

		// Save texture to custom DDS file
		cubeMap->fileNames[i] = App->files->GetFileName(filePath);
		std::string ddsFilePath = std::string(TEXTURES_PATH) + "/" + cubeMap->fileNames[i] + TEXTURE_EXTENSION;

		LOG("Saving image to \"%s\".", ddsFilePath.c_str());
		ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
		size_t size = ilSaveL(IL_DDS, nullptr, 0);
		if (size == 0) {
			LOG("Failed to save image.");
			return nullptr;
		}
		Buffer<char> buffer = Buffer<char>(size);
		size = ilSaveL(IL_DDS, buffer.Data(), size);
		if (size == 0) {
			LOG("Failed to save image.");
			return nullptr;
		}
		App->files->Save(ddsFilePath.c_str(), buffer);

		LOG("Cube map texture imported successfuly.");
	}

	return cubeMap;
}

void TextureImporter::LoadCubeMap(CubeMap* cubeMap) {
	// Create texture handle
	glGenTextures(1, &cubeMap->glTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap->glTexture);

	// Load cube map
	for (unsigned i = 0; i < 6; ++i) {
		std::string filePath = std::string(TEXTURES_PATH) + "/" + cubeMap->fileNames[i] + TEXTURE_EXTENSION;

		LOG("Loading cubemap texture from path: \"%s\".", filePath.c_str());

		// Generate image handler
		unsigned image;
		ilGenImages(1, &image);
		DEFER {
			ilDeleteImages(1, &image);
		};

		// Load image
		ilBindImage(image);
		bool imageLoaded = ilLoad(IL_DDS, filePath.c_str());
		if (!imageLoaded) {
			LOG("Failed to load image.");
			return;
		}

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());
	}

	// Set filtering and wrapping
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void TextureImporter::UnloadCubeMap(CubeMap* cubeMap) {
	if (!cubeMap->glTexture) return;

	glDeleteTextures(1, &cubeMap->glTexture);
}