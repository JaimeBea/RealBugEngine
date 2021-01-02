#pragma once

#include "Module.h"
#include "Texture.h"
#include "Mesh.h"
#include "Pool.h"

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

class ModuleResources : public Module
{
public:
	bool Init() override;
	bool CleanUp() override;

	Texture* ImportTexture(const char* file_name);
	void LoadTexture(Texture* texture);
	void UnloadTexture(Texture* texture);
	void ReleaseTexture(Texture* texture);

	void LoadMesh(Mesh* mesh);
	void UnloadMesh(Mesh* mesh);
	void ReleaseMesh(Mesh* mesh);

	void SetMinFilter(TextureMinFilter filter);
	void SetMagFilter(TextureMagFilter filter);
	void SetWrap(TextureWrap wrap);

	TextureMinFilter GetMinFilter() const;
	TextureMagFilter GetMagFilter() const;
	TextureWrap GetWrap() const;

public:
	Pool<Texture> textures;
	Pool<Mesh> meshes;

private:
	TextureMinFilter min_filter = TextureMinFilter::NEAREST_MIPMAP_LINEAR;
	TextureMagFilter mag_filter = TextureMagFilter::LINEAR;
	TextureWrap texture_wrap = TextureWrap::REPEAT;
};
