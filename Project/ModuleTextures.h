#pragma once

#include "Module.h"

#include <vector>

enum class TextureMinFilter
{
	NEAREST,
	LINEAR,
	NEAREST_MIPMAP_NEAREST,
	LINEAR_MIPMAP_NEAREST,
	NEAREST_MIPMAP_LINEAR,
	LINEAR_MIPMAP_LINEAR
};

enum class TextureMagFilter
{
	NEAREST,
	LINEAR
};

enum class TextureWrap
{
	REPEAT,
	CLAMP_TO_EDGE,
	CLAMP_TO_BORDER,
	MIRROR_REPEAT,
	MIRROR_CLAMP_TO_EDGE
};

class ModuleTextures : public Module
{
public:
	bool Init() override;
	bool CleanUp() override;

	unsigned LoadTexture(const char* file_name);
	void ReleaseTexture(unsigned texture);

	void SetMinFilter(TextureMinFilter filter);
	void SetMagFilter(TextureMagFilter filter);
	void SetWrap(TextureWrap wrap);

	TextureMinFilter GetMinFilter() const;
	TextureMagFilter GetMagFilter() const;
	TextureWrap GetWrap() const;

public:
	std::vector<unsigned> textures;

private:
	TextureMinFilter min_filter = TextureMinFilter::NEAREST_MIPMAP_LINEAR;
	TextureMagFilter mag_filter = TextureMagFilter::LINEAR;
	TextureWrap texture_wrap = TextureWrap::REPEAT;
};

