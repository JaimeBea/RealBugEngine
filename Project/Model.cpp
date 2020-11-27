#include "Model.h"

#include "Application.h"
#include "Logging.h"
#include "ModuleTextures.h"

#include "Math/myassert.h"
#include "assimp/cimport.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include <vector>
#include <string>

#include "Leaks.h"

bool Model::Load(const char* file_name)
{
	// Load model
	LOG("Loading model from path: \"%s\".", file_name);
	const aiScene* scene = aiImportFile(file_name, aiProcessPreset_TargetRealtime_MaxQuality);
	if (!scene)
	{
		LOG("Error loading model: %s", file_name, aiGetErrorString());
		return false;
	}

	// Load materials
	LOG("Loading %i materials...", scene->mNumMaterials);
	materials.reserve(scene->mNumMaterials);
	for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
	{
		LOG("Loading material %i...", i);
		aiString material_file_dir;
		aiTextureMapping mapping;
		unsigned uv_index;
		if (scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &material_file_dir, &mapping, &uv_index) == AI_SUCCESS)
		{
			// Check if the material is valid for our purposes
			assert(mapping == aiTextureMapping_UV);
			assert(uv_index == 0);

			// Try to load from the path given in the model file
			LOG("Trying to load texture...");
			unsigned texture = App->textures->LoadTexture(material_file_dir.C_Str());

			// Try to load relative to the model folder
			if (texture == 0)
			{
				LOG("Trying to load texture relative to model folder...");
				std::string model_file_dir = file_name;
				size_t last_slash = model_file_dir.find_last_of('\\');
				if (last_slash == std::string::npos)
				{
					last_slash = model_file_dir.find_last_of('/');
				}
				std::string model_folder_dir = model_file_dir.substr(0, last_slash + 1);
				std::string model_folder_material_file_dir = model_folder_dir + material_file_dir.C_Str();
				texture = App->textures->LoadTexture(model_folder_material_file_dir.c_str());
			}

			// Try to load relative to the textures folder
			if (texture == 0)
			{
				LOG("Trying to load texture relative to textures folder...");
				std::string textures_folder_dir = "Textures\\";
				std::string textures_folder_material_file_dir = textures_folder_dir + material_file_dir.C_Str();
				texture = App->textures->LoadTexture(textures_folder_material_file_dir.c_str());
			}

			if (texture == 0)
			{
				LOG("Unable to find texture file.");
			}
			else
			{
				LOG("Texture loaded successfuly.");
			}

			materials.push_back(texture);
		}
		else
		{
			LOG("Diffuse texture not found.");
		}

		LOG("Material loaded.");
	}

	// Create an auxiliary vertex array
	std::vector<vec> vertices;
	int num_vertices = 0;
	for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
	{
		num_vertices += scene->mMeshes[i]->mNumVertices;
	}
	vertices.reserve(num_vertices);

	// Load meshes
	LOG("Loading %i meshes...", scene->mNumMeshes);
	meshes.reserve(scene->mNumMeshes);
	for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
	{
		const aiMesh* ai_mesh = scene->mMeshes[i];

		Mesh mesh;
		mesh.Load(ai_mesh);
		meshes.push_back(mesh);

		// Add vertices to auxiliary vertex array
		for (unsigned int j = 0; j < ai_mesh->mNumVertices; ++j)
		{
			const aiVector3D& ai_vertex = ai_mesh->mVertices[j];
			vertices.push_back(vec(ai_vertex.x, ai_vertex.y, ai_vertex.z));
		}
	}

	// Calculate bounding sphere with auxiliary vertex array
	bounding_sphere = Sphere::FastEnclosingSphere(vertices.data(), vertices.size());

	LOG("Model loaded.");
	return true;
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
	for (int material : materials)
	{
		App->textures->ReleaseTexture(material);
	}
	materials.clear();
}

void Model::Draw(const float4x4& model_matrix) const
{
	for (const Mesh& mesh : meshes)
	{
		mesh.Draw(materials, model_matrix);
	}
}
