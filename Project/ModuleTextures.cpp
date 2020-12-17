#include "ModuleTextures.h"

#include "Globals.h"
#include "Application.h"
#include "Logging.h"

#include "IL/il.h"
#include "IL/ilu.h"
#include "GL/glew.h"

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
		glDeleteTextures(1, &texture);
	}

	return true;
}

Texture* ModuleTextures::LoadTexture(const char* file_name)
{
	LOG("Loading texture from path: \"%s\".", file_name);

	unsigned image;
	ilGenImages(1, &image);
	DEFER{ ilDeleteImages(1, &image); };

	ilBindImage(image);
	bool image_loaded = ilLoadImage(file_name);
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
	glGenTextures(1, texture);

	// Generate texture from image
	glBindTexture(GL_TEXTURE_2D, *texture);
	glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH),
		ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE,
		ilGetData());

	// Generate mipmaps and set filtering and wrapping
	glGenerateMipmap(GL_TEXTURE_2D);
	SetWrap(texture_wrap);
	SetMinFilter(min_filter);
	SetMagFilter(mag_filter);

	LOG("Texture loaded successfuly.");

	return texture;
}

void ModuleTextures::ReleaseTexture(Texture* texture)
{
	textures.Release(texture);
	glDeleteTextures(1, texture);
}

void ModuleTextures::SetMinFilter(TextureMinFilter filter)
{
	for (unsigned texture : textures)
	{
		glBindTexture(GL_TEXTURE_2D, texture);
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
	for (unsigned texture : textures)
	{
		glBindTexture(GL_TEXTURE_2D, texture);
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
	for (unsigned texture : textures)
	{
		glBindTexture(GL_TEXTURE_2D, texture);
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
