#pragma once

#include "Module.h"

#include <vector>

enum class TextureFilter
{
	NEAREST,
	LINEAR,
	NEAREST_MIPMAP_NEAREST,
	LINEAR_MIPMAP_NEAREST,
	NEAREST_MIPMAP_LINEAR,
	LINEAR_MIPMAP_LINEAR
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

	void SetMinFilter(TextureFilter filter);
	void SetMagFilter(TextureFilter filter);
	void SetWrap(TextureWrap wrap);

	TextureFilter GetMinFilter() const;
	TextureFilter GetMagFilter() const;
	TextureWrap GetWrap() const;

public:
	std::vector<unsigned> textures;

private:
	TextureFilter min_filter = TextureFilter::NEAREST_MIPMAP_LINEAR;
	TextureFilter mag_filter = TextureFilter::LINEAR;
	TextureWrap texture_wrap = TextureWrap::REPEAT;
};

