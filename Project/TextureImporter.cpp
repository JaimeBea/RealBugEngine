#include "TextureImporter.h"

#include "Globals.h"
#include "Application.h"
#include "Logging.h"
#include "ModuleResources.h"
#include "ModuleFiles.h"
#include "Texture.h"
#include "CubeMap.h"
#include "Buffer.h"

#include "IL/il.h"
#include "IL/ilu.h"
#include "GL/glew.h"

#include "Leaks.h"

Texture* TextureImporter::ImportTexture(const char* file_path)
{
	LOG("Importing texture from path: \"%s\".", file_path);

	// Generate image handler
	unsigned image;
	ilGenImages(1, &image);
	DEFER
	{
		ilDeleteImages(1, &image);
	};

	// Load image
	ilBindImage(image);
	bool image_loaded = ilLoadImage(file_path);
	if (!image_loaded)
	{
		LOG("Failed to load image.");
		return nullptr;
	}
	bool image_converted = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
	if (!image_converted)
	{
		LOG("Failed to convert image.");
		return nullptr;
	}

	// Flip image if neccessary
	ILinfo info;
	iluGetImageInfo(&info);
	if (info.Origin == IL_ORIGIN_UPPER_LEFT)
	{
		iluFlipImage();
	}

	// Create texture
	Texture* texture = App->resources->ObtainTexture();

	// Save texture to custom DDS file
	texture->file_name = App->files->GetFileName(file_path);
	std::string dds_file_path = std::string(TEXTURES_PATH) + texture->file_name + TEXTURE_EXTENSION;

	LOG("Saving image to \"%s\".", dds_file_path.c_str());
	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
	size_t size = ilSaveL(IL_DDS, nullptr, 0);
	if (size == 0)
	{
		LOG("Failed to save image.");
		return nullptr;
	}
	Buffer<char> buffer = Buffer<char>(size);
	size = ilSaveL(IL_DDS, buffer.Data(), size);
	if (size == 0)
	{
		LOG("Failed to save image.");
		return nullptr;
	}
	App->files->Save(dds_file_path.c_str(), buffer);

	LOG("Texture imported successfuly.");

	return texture;
}

void TextureImporter::LoadTexture(Texture* texture)
{
	if (texture == nullptr) return;

	std::string file_path = std::string(TEXTURES_PATH) + texture->file_name + TEXTURE_EXTENSION;

	LOG("Loading texture from path: \"%s\".", file_path.c_str());

	// Generate image handler
	unsigned image;
	ilGenImages(1, &image);
	DEFER
	{
		ilDeleteImages(1, &image);
	};

	// Load image
	ilBindImage(image);
	bool image_loaded = ilLoad(IL_DDS, file_path.c_str());
	if (!image_loaded)
	{
		LOG("Failed to load image.");
		return;
	}

	// Generate texture from image
	glGenTextures(1, &texture->gl_texture);
	glBindTexture(GL_TEXTURE_2D, texture->gl_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());

	// Generate mipmaps and set filtering and wrapping
	glGenerateMipmap(GL_TEXTURE_2D);
	App->resources->SetWrap(App->resources->GetWrap());
	App->resources->SetMinFilter(App->resources->GetMinFilter());
	App->resources->SetMagFilter(App->resources->GetMagFilter());

	LOG("Texture loaded successfuly.");
}

void TextureImporter::UnloadTexture(Texture* texture)
{
	if (!texture->gl_texture) return;

	glDeleteTextures(1, &texture->gl_texture);
}

CubeMap* TextureImporter::ImportCubeMap(const char* file_paths[6])
{
	// Create cube map
	CubeMap* cube_map = App->resources->ObtainCubeMap();

	for (unsigned i = 0; i < 6; ++i)
	{
		const char* file_path = file_paths[i];

		LOG("Importing cube map texture from path: \"%s\".", file_path);

		// Generate image handler
		unsigned image;
		ilGenImages(1, &image);
		DEFER
		{
			ilDeleteImages(1, &image);
		};

		// Load image
		ilBindImage(image);
		bool image_loaded = ilLoadImage(file_path);
		if (!image_loaded)
		{
			LOG("Failed to load image.");
			return nullptr;
		}
		bool image_converted = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
		if (!image_converted)
		{
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
		cube_map->file_names[i] = App->files->GetFileName(file_path);
		std::string dds_file_path = std::string(TEXTURES_PATH) + cube_map->file_names[i] + TEXTURE_EXTENSION;

		LOG("Saving image to \"%s\".", dds_file_path.c_str());
		ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
		size_t size = ilSaveL(IL_DDS, nullptr, 0);
		if (size == 0)
		{
			LOG("Failed to save image.");
			return nullptr;
		}
		Buffer<char> buffer = Buffer<char>(size);
		size = ilSaveL(IL_DDS, buffer.Data(), size);
		if (size == 0)
		{
			LOG("Failed to save image.");
			return nullptr;
		}
		App->files->Save(dds_file_path.c_str(), buffer);

		LOG("Cube map texture imported successfuly.");
	}

	return cube_map;
}

void TextureImporter::LoadCubeMap(CubeMap* cube_map)
{
	// Create texture handle
	glGenTextures(1, &cube_map->gl_texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cube_map->gl_texture);

	// Load cube map
	for (unsigned i = 0; i < 6; ++i)
	{
		std::string file_path = std::string(TEXTURES_PATH) + cube_map->file_names[i] + TEXTURE_EXTENSION;

		LOG("Loading cubemap texture from path: \"%s\".", file_path.c_str());

		// Generate image handler
		unsigned image;
		ilGenImages(1, &image);
		DEFER
		{
			ilDeleteImages(1, &image);
		};

		// Load image
		ilBindImage(image);
		bool image_loaded = ilLoad(IL_DDS, file_path.c_str());
		if (!image_loaded)
		{
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

void TextureImporter::UnloadCubeMap(CubeMap* cube_map)
{
	if (!cube_map->gl_texture) return;

	glDeleteTextures(1, &cube_map->gl_texture);
}