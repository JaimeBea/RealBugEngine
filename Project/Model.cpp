#include "Model.h"

#include "Application.h"
#include "Logging.h"
#include "ModuleTextures.h"

#include "assimp/cimport.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

void Model::Load(const char* file_name)
{
	// Load model
	const aiScene* scene = aiImportFile(file_name, aiProcessPreset_TargetRealtime_MaxQuality);
	if (!scene)
	{
		LOG("Error loading %s: %s", file_name, aiGetErrorString());
		return;
	}

	// Load materials
	materials.reserve(scene->mNumMaterials);
	for (unsigned i = 0; i < scene->mNumMaterials; ++i)
	{
		aiString material_file_name;
		if (scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &material_file_name) == AI_SUCCESS)
		{
			materials.push_back(App->textures->LoadTexture(material_file_name.data));
		}
	}

	// Load meshes
	meshes.reserve(scene->mNumMeshes);
	for (unsigned i = 0; i < scene->mNumMeshes; ++i)
	{
		Mesh mesh;
		mesh.Load(scene->mMeshes[i]);
		meshes.push_back(mesh);
	}
}

void Model::Release()
{
	// Release meshes
	for (Mesh& mesh : meshes)
	{
		mesh.Release();
	}
	meshes.clear();

	// Release materials
	for (unsigned material : materials)
	{
		App->textures->ReleaseTexture(material);
	}
	materials.clear();
}

void Model::Draw() const
{
	for (const Mesh& mesh : meshes)
	{
		mesh.Draw(materials);
	}
}
