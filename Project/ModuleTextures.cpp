#include "ModuleTextures.h"

#include "Globals.h"
#include "Application.h"
#include "Logging.h"
#include "ModuleFiles.h"

#include "IL/il.h"
#include "IL/ilu.h"
#include "GL/glew.h"
#include <string>

#include "Leaks.h"

bool ModuleTextures::Init()
{
	textures.Allocate(100);

	ilInit();
	iluInit();

	return true;
}

bool ModuleTextures::CleanUp()
{
	for (Texture& texture : textures)
	{
		glDeleteTextures(1, &texture.gl_texture);
	}

	return true;
}

Texture* ModuleTextures::Import(const char* file_path)
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
	Texture* texture = textures.Obtain();

	// Save texture to custom DDS file
	std::string file_name = App->files->GetFileName(file_path);
	std::string dds_file_path = std::string(TEXTURES_PATH) + file_name + TEXTURE_EXTENSION;

	LOG("Saving image to \"%s\".", dds_file_path.c_str());
	texture->file_id = App->files->CreateUIDForFileName(file_name.c_str());
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

void ModuleTextures::Load(Texture* texture)
{
	if (texture == nullptr) return;

	std::string file_name = App->files->GetFileNameFromUID(texture->file_id);
	std::string file_path = std::string(TEXTURES_PATH) + file_name + TEXTURE_EXTENSION;

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
	SetWrap(texture_wrap);
	SetMinFilter(min_filter);
	SetMagFilter(mag_filter);

	LOG("Texture loaded successfuly.");
}

void ModuleTextures::Release(Texture* texture)
{
	textures.Release(texture);
	glDeleteTextures(1, &texture->gl_texture);
}

void ModuleTextures::SetMinFilter(TextureMinFilter filter)
{
	for (Texture& texture : textures)
	{
		glBindTexture(GL_TEXTURE_2D, texture.gl_texture);
		switch (filter)
		{
		case TextureMinFilter::NEAREST:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			break;
		case TextureMinFilter::LINEAR:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			break;
		case TextureMinFilter::NEAREST_MIPMAP_NEAREST:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			break;
		case TextureMinFilter::LINEAR_MIPMAP_NEAREST:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			break;
		case TextureMinFilter::NEAREST_MIPMAP_LINEAR:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
			break;
		case TextureMinFilter::LINEAR_MIPMAP_LINEAR:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			break;
		}
	}

	min_filter = filter;
}

void ModuleTextures::SetMagFilter(TextureMagFilter filter)
{
	for (Texture& texture : textures)
	{
		glBindTexture(GL_TEXTURE_2D, texture.gl_texture);
		switch (filter)
		{
		case TextureMagFilter::NEAREST:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			break;
		case TextureMagFilter::LINEAR:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			break;
		}
	}

	mag_filter = filter;
}

void ModuleTextures::SetWrap(TextureWrap wrap)
{
	for (Texture& texture : textures)
	{
		glBindTexture(GL_TEXTURE_2D, texture.gl_texture);
		switch (wrap)
		{
		case TextureWrap::REPEAT:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			break;
		case TextureWrap::CLAMP_TO_EDGE:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			break;
		case TextureWrap::CLAMP_TO_BORDER:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			break;
		case TextureWrap::MIRROR_REPEAT:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
			break;
		case TextureWrap::MIRROR_CLAMP_TO_EDGE:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRROR_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRROR_CLAMP_TO_EDGE);
			break;
		}
	}

	texture_wrap = wrap;
}

TextureMinFilter ModuleTextures::GetMinFilter() const
{
	return min_filter;
}

TextureMagFilter ModuleTextures::GetMagFilter() const
{
	return mag_filter;
}

TextureWrap ModuleTextures::GetWrap() const
{
	return texture_wrap;
}
