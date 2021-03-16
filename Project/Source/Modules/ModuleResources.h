#pragma once

#include "Module.h"
#include "Utils/Pool.h"
#include "Resources/Texture.h"
#include "Resources/CubeMap.h"
#include "Resources/Mesh.h"
#include "Resources/ResourceAnimation.h"
#include "Resources/AnimationController.h"

enum class TextureMinFilter {
	NEAREST,
	LINEAR,
	NEAREST_MIPMAP_NEAREST,
	LINEAR_MIPMAP_NEAREST,
	NEAREST_MIPMAP_LINEAR,
	LINEAR_MIPMAP_LINEAR
};

enum class TextureMagFilter {
	NEAREST,
	LINEAR
};

enum class TextureWrap {
	REPEAT,
	CLAMP_TO_EDGE,
	CLAMP_TO_BORDER,
	MIRROR_REPEAT,
	MIRROR_CLAMP_TO_EDGE
};

class ModuleResources : public Module {
public:
	bool Init() override;
	bool CleanUp() override;

	Texture* ObtainTexture();
	void ReleaseTexture(Texture* texture);

	CubeMap* ObtainCubeMap();
	void ReleaseCubeMap(CubeMap* cubeMap);

	Mesh* ObtainMesh();
	void ReleaseMesh(Mesh* mesh);

	ResourceAnimation* ObtainAnimation();
	void ReleaseAnimation(ResourceAnimation* animation);

	AnimationController* ObtainAnimationController();
	void ReleaseAnimationController(AnimationController* animationController);

	void ReleaseAll();

	void SetMinFilter(TextureMinFilter filter);
	void SetMagFilter(TextureMagFilter filter);
	void SetWrap(TextureWrap wrap);

	TextureMinFilter GetMinFilter() const;
	TextureMagFilter GetMagFilter() const;
	TextureWrap GetWrap() const;

public:
	Pool<Texture> textures;
	Pool<CubeMap> cubeMaps;
	Pool<Mesh> meshes;
	Pool<ResourceAnimation> animations;
	Pool<AnimationController> animationControllers;

private:
	TextureMinFilter minFilter = TextureMinFilter::NEAREST_MIPMAP_LINEAR;
	TextureMagFilter magFilter = TextureMagFilter::LINEAR;
	TextureWrap textureWrap = TextureWrap::REPEAT;
};
