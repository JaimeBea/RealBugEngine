#include "ModuleScene.h"

#include "Globals.h"
#include "Application.h"
#include "Logging.h"
#include "ModuleTextures.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentBoundingBox.h"

#include "Math/myassert.h"
#include "assimp/cimport.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "Math/float4x4.h"
#include "Geometry/Sphere.h"
#include <string>

#include "Leaks.h"

static void AssimpLogCallback(const char* message, char* user)
{
	std::string message_str = message;
	std::string final_message_str = message_str.substr(0, message_str.find_last_of('\n'));
	LOG(final_message_str.c_str());
}

bool ModuleScene::Init()
{
	game_objects.Allocate(10000);

#ifdef _DEBUG
	log_stream.callback = AssimpLogCallback;
	aiAttachLogStream(&log_stream);
#endif

	return true;
}

bool ModuleScene::Start()
{
	Load("Assets/Street_Environment/Street_environment_V01.FBX");

	return true;
}

bool ModuleScene::CleanUp()
{
	DestroyGameObject(root);
	assert(game_objects.Count() == 0);

#ifdef _DEBUG
	aiDetachAllLogStreams();
#endif

	return true;
}

bool ModuleScene::Load(const char* file_name)
{
	// Unload previous scene
	DestroyGameObject(root);
	assert(game_objects.Count() == 0);
	for (Texture& texture : App->textures->textures)
	{
		App->textures->ReleaseTexture(&texture);
	}

	// Load scene
	LOG("Loading scene from path: \"%s\".", file_name);
	const aiScene* scene = aiImportFile(file_name, aiProcessPreset_TargetRealtime_MaxQuality);
	DEFER{ aiReleaseImport(scene); };
	if (!scene)
	{
		LOG("Error loading scene: %s", file_name, aiGetErrorString());
		return false;
	}

	// Load materials
	LOG("Loading %i materials...", scene->mNumMaterials);
	std::vector<Texture*> materials;
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
			Texture* texture = App->textures->LoadTexture(material_file_dir.C_Str());

			// Try to load relative to the model folder
			if (texture == nullptr)
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
			if (texture == nullptr)
			{
				LOG("Trying to load texture relative to textures folder...");
				std::string textures_folder_dir = "Textures\\";
				std::string textures_folder_material_file_dir = textures_folder_dir + material_file_dir.C_Str();
				texture = App->textures->LoadTexture(textures_folder_material_file_dir.c_str());
			}

			if (texture == nullptr)
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
			materials.push_back(nullptr);

			LOG("Diffuse texture not found.");
		}

		LOG("Material loaded.");
	}

	// Create scene tree
	LOG("Loading scene tree.");
	root = LoadNode(scene, materials, scene->mRootNode, nullptr);

	LOG("Scene loaded.");
	return true;
}


GameObject* ModuleScene::CreateGameObject(GameObject* parent)
{
	GameObject* game_object = game_objects.Obtain();
	game_object->SetParent(parent);
	game_object->Init();
	return game_object;
}

void ModuleScene::DestroyGameObject(GameObject* game_object)
{
	if (game_object == nullptr)
	{
		return;
	}

	game_object->CleanUp();

	for (GameObject* child : game_object->GetChildren())
	{
		DestroyGameObject(child);
	}

	game_objects.Release(game_object);
}

GameObject* ModuleScene::LoadNode(const aiScene* scene, const std::vector<Texture*>& materials, const aiNode* node, GameObject* parent)
{
	LOG("Loading node: \"%s\"", node->mName.C_Str());

	// Create GameObje
	GameObject* game_object = CreateGameObject(parent);

	// Load name
	game_object->name = node->mName.C_Str();

	// Load transform
	ComponentTransform* transform = game_object->CreateComponent<ComponentTransform>();
	const float4x4& matrix = *(float4x4*)&node->mTransformation;
	float3 position;
	Quat rotation;
	float3 scale;
	matrix.Decompose(position, rotation, scale);
	transform->SetPosition(position);
	transform->SetRotation(rotation);
	transform->SetScale(scale);
	transform->CalculateGlobalMatrix();
	LOG("Transform: (%f, %f, %f), (%f, %f, %f, %f), (%f, %f, %f)", position.x, position.y, position.z, rotation.x, rotation.y, rotation.z, rotation.w, scale.x, scale.y, scale.z);

	// Save min and max points
	vec min_point = vec(FLOAT_INF, FLOAT_INF, FLOAT_INF);
	vec max_point = vec(-FLOAT_INF, -FLOAT_INF, -FLOAT_INF);

	// Load meshes
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		LOG("Loading mesh %i", i);
		aiMesh* ai_mesh = scene->mMeshes[node->mMeshes[i]];

		ComponentMesh* mesh = game_object->CreateComponent<ComponentMesh>();
		mesh->Load(ai_mesh);
		mesh->material_index = i;

		ComponentMaterial* material = game_object->CreateComponent<ComponentMaterial>();
		if (ai_mesh->mMaterialIndex >= materials.size())
		{
			material->texture = materials.size() > 0 ? *materials.front() : 0;
			LOG("Invalid material found", ai_mesh->mMaterialIndex);
		}
		else
		{
			Texture* texture = materials[ai_mesh->mMaterialIndex];
			if (texture == nullptr)
			{
				material->texture = *materials.front();
				LOG("Material has no texture: %i", ai_mesh->mMaterialIndex);
			}
			else
			{
				material->texture = *texture;
				LOG("Texture applied: %i", *texture);
			}
		}

		// Update min and max points
		for (unsigned int j = 0; j < ai_mesh->mNumVertices; ++j)
		{
			aiVector3D vertex = ai_mesh->mVertices[j];
			if (vertex.x < min_point.x) min_point.x = vertex.x;
			if (vertex.y < min_point.y) min_point.y = vertex.y;
			if (vertex.z < min_point.z) min_point.z = vertex.z;
			if (vertex.x > max_point.x) max_point.x = vertex.x;
			if (vertex.y > max_point.y) max_point.y = vertex.y;
			if (vertex.z > max_point.z) max_point.z = vertex.z;
		}
	}

	// Create bounding box
	ComponentBoundingBox* bounding_box = game_object->CreateComponent<ComponentBoundingBox>();
	bounding_box->SetLocalBoundingBox(AABB(min_point, max_point));
	bounding_box->CalculateWorldBoundingBox();

	// Load children nodes
	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		GameObject* child = LoadNode(scene, materials, node->mChildren[i], game_object);
	}

	return game_object;
}
