#include "ModuleScene.h"

#include "Globals.h"
#include "Application.h"
#include "Logging.h"
#include "ModuleResources.h"
#include "ModuleFiles.h"
#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentLight.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentBoundingBox.h"
#include "Texture.h"

#include "Math/myassert.h"
#include "assimp/cimport.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "Math/float4x4.h"
#include "Geometry/Sphere.h"
#include <string>

#include "Leaks.h"

static aiLogStream log_stream = {nullptr, nullptr};

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

	// Create Scene
	root = CreateGameObject(nullptr);
	root->name = "Scene";
	ComponentTransform* transform = root->CreateComponent<ComponentTransform>();
	transform->SetPosition(float3(0, 0, 0));
	transform->SetRotation(Quat::identity);
	transform->SetScale(float3(1, 1, 1));
	transform->CalculateGlobalMatrix();

	return true;
}

bool ModuleScene::Start()
{
	Import("Assets/BakerHouse.fbx");

	// Create Directional Light
	GameObject* game_object = CreateGameObject(root);
	game_object->name = "Directional Light";

	ComponentTransform* transform = game_object->CreateComponent<ComponentTransform>();
	transform->SetPosition(float3(0, 0, 0));
	transform->SetRotation(Quat::identity);
	transform->SetScale(float3(1, 1, 1));
	transform->CalculateGlobalMatrix();
	ComponentLight* light = game_object->CreateComponent<ComponentLight>();

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

static Mesh* ImportMesh(const aiMesh* ai_mesh)
{
	// Create mesh
	Mesh* mesh = App->resources->meshes.Obtain();
	mesh->num_vertices = ai_mesh->mNumVertices;
	mesh->num_indices = ai_mesh->mNumFaces * 3;
	mesh->material_index = ai_mesh->mMaterialIndex;

	// Save to custom format buffer
	unsigned position_size = sizeof(float) * 3;
	unsigned normal_size = sizeof(float) * 3;
	unsigned uv_size = sizeof(float) * 2;
	unsigned index_size = sizeof(unsigned);

	unsigned header_size = sizeof(unsigned) * 2;
	unsigned vertex_size = position_size + normal_size + uv_size;
	unsigned vertex_buffer_size = vertex_size * mesh->num_vertices;
	unsigned index_buffer_size = index_size * mesh->num_indices;

	size_t size = header_size + vertex_buffer_size + index_buffer_size;
	Buffer<char> buffer = Buffer<char>(size);
	char* cursor = buffer.Data();

	*((unsigned*) cursor) = mesh->num_vertices;
	cursor += sizeof(unsigned);
	*((unsigned*) cursor) = mesh->num_indices;
	cursor += sizeof(unsigned);

	for (unsigned i = 0; i < ai_mesh->mNumVertices; ++i)
	{
		aiVector3D& vertex = ai_mesh->mVertices[i];
		aiVector3D& normal = ai_mesh->mNormals[i];
		aiVector3D* texture_coords = ai_mesh->mTextureCoords[0];

		*((float*) cursor) = vertex.x;
		cursor += sizeof(float);
		*((float*) cursor) = vertex.y;
		cursor += sizeof(float);
		*((float*) cursor) = vertex.z;
		cursor += sizeof(float);
		*((float*) cursor) = normal.x;
		cursor += sizeof(float);
		*((float*) cursor) = normal.y;
		cursor += sizeof(float);
		*((float*) cursor) = normal.z;
		cursor += sizeof(float);
		*((float*) cursor) = texture_coords != nullptr ? texture_coords[i].x : 0;
		cursor += sizeof(float);
		*((float*) cursor) = texture_coords != nullptr ? texture_coords[i].y : 0;
		cursor += sizeof(float);
	}

	for (unsigned i = 0; i < ai_mesh->mNumFaces; ++i)
	{
		aiFace& ai_face = ai_mesh->mFaces[i];

		// Assume triangles = 3 indices per face
		if (ai_face.mNumIndices != 3)
		{
			LOG("Found a face with %i vertices. Discarded.", ai_face.mNumIndices);
			continue;
		}

		*((unsigned*) cursor) = ai_face.mIndices[0];
		cursor += sizeof(unsigned);
		*((unsigned*) cursor) = ai_face.mIndices[1];
		cursor += sizeof(unsigned);
		*((unsigned*) cursor) = ai_face.mIndices[2];
		cursor += sizeof(unsigned);
	}

	// Save buffer to file
	mesh->file_id = GenerateUID();
	std::string file_name = std::to_string(mesh->file_id);
	std::string file_path = std::string(MESHES_PATH) + file_name + MESH_EXTENSION;
	LOG("Saving mesh to \"%s\".", file_path.c_str());
	App->files->Save(file_path.c_str(), buffer);

	return mesh;
}

static GameObject* ImportNode(const aiScene* ai_scene, const std::vector<Texture*>& materials, const aiNode* node, GameObject* parent)
{
	LOG("Importing node: \"%s\"", node->mName.C_Str());

	// Create GameObject
	GameObject* game_object = App->scene->CreateGameObject(parent);

	// Load name
	game_object->name = node->mName.C_Str();

	// Load transform
	ComponentTransform* transform = game_object->CreateComponent<ComponentTransform>();
	const float4x4& matrix = *(float4x4*) &node->mTransformation;
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
		LOG("Importing mesh %i", i);
		aiMesh* ai_mesh = ai_scene->mMeshes[node->mMeshes[i]];

		ComponentMesh* mesh = game_object->CreateComponent<ComponentMesh>();
		mesh->mesh = ImportMesh(ai_mesh);
		mesh->mesh->material_index = i;

		// TODO: Move mesh loading to a better place
		App->resources->LoadMesh(mesh->mesh);

		ComponentMaterial* material = game_object->CreateComponent<ComponentMaterial>();
		if (ai_mesh->mMaterialIndex >= materials.size())
		{
			material->diffuse_map = materials.size() > 0 ? materials.front() : nullptr;
			LOG("Invalid material found", ai_mesh->mMaterialIndex);
		}
		else
		{
			Texture* texture = materials[ai_mesh->mMaterialIndex];
			if (texture == nullptr)
			{
				material->diffuse_map = materials.front();
				LOG("Material has no texture: %i", ai_mesh->mMaterialIndex);
			}
			else
			{
				material->diffuse_map = texture;
				LOG("Texture applied: %i", texture->gl_texture);
			}
		}
		material->has_diffuse_map = material->diffuse_map ? true : false;

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
		GameObject* child = ImportNode(ai_scene, materials, node->mChildren[i], game_object);
	}

	return game_object;
}

bool ModuleScene::Import(const char* file_name)
{
	// Check for extension support
	std::string extension = App->files->GetFileExtension(file_name);
	if (!aiIsExtensionSupported(extension.c_str()))
	{
		LOG("Extension is not supported by assimp: \"%s\".", extension);
		return false;
	}

	// Import scene
	LOG("Importing scene from path: \"%s\".", file_name);
	const aiScene* scene = aiImportFile(file_name, aiProcessPreset_TargetRealtime_MaxQuality);
	DEFER
	{
		aiReleaseImport(scene);
	};
	if (!scene)
	{
		LOG("Error importing scene: %s", file_name, aiGetErrorString());
		return false;
	}

	// TODO: Add Specular Texture Loading logic
	// Load materials
	LOG("Importing %i materials...", scene->mNumMaterials);
	std::vector<Texture*> materials;
	materials.reserve(scene->mNumMaterials);
	for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
	{
		LOG("Loading material %i...", i);
		aiMaterial* ai_material = scene->mMaterials[i];
		aiString material_file_dir;
		aiTextureMapping mapping;
		unsigned uv_index;
		if (ai_material->GetTexture(aiTextureType_DIFFUSE, 0, &material_file_dir, &mapping, &uv_index) == AI_SUCCESS)
		{
			// Check if the material is valid for our purposes
			assert(mapping == aiTextureMapping_UV);
			assert(uv_index == 0);

			// Try to load from the path given in the model file
			LOG("Trying to import texture...");
			Texture* texture = App->resources->ImportTexture(material_file_dir.C_Str());

			// Try to load relative to the model folder
			if (texture == nullptr)
			{
				LOG("Trying to import texture relative to model folder...");
				std::string model_file_dir = file_name;
				size_t last_slash = model_file_dir.find_last_of('\\');
				if (last_slash == std::string::npos)
				{
					last_slash = model_file_dir.find_last_of('/');
				}
				std::string model_folder_dir = model_file_dir.substr(0, last_slash + 1);
				std::string model_folder_material_file_dir = model_folder_dir + material_file_dir.C_Str();
				texture = App->resources->ImportTexture(model_folder_material_file_dir.c_str());
			}

			// Try to load relative to the textures folder
			if (texture == nullptr)
			{
				LOG("Trying to import texture relative to textures folder...");
				std::string textures_folder_dir = "Textures\\";
				std::string textures_folder_material_file_dir = textures_folder_dir + material_file_dir.C_Str();
				texture = App->resources->ImportTexture(textures_folder_material_file_dir.c_str());
			}

			if (texture == nullptr)
			{
				LOG("Unable to find texture file.");
			}
			else
			{
				LOG("Texture imported successfuly.");
			}

			// TODO: Move load to a better place
			if (texture != nullptr)
			{
				App->resources->LoadTexture(texture);
			}

			materials.push_back(texture);
		}
		else
		{
			LOG("Diffuse texture not found.");

			materials.push_back(nullptr);
		}

		LOG("Material imported.");
	}

	// Create scene tree
	LOG("Importing scene tree.");
	GameObject* game_object = ImportNode(scene, materials, scene->mRootNode, root);

	LOG("Scene imported.");
	return true;
}

bool ModuleScene::Load(const char* file_name)
{
	return true;
}

bool ModuleScene::Save(const char* file_name) const
{
	return true;
}

GameObject* ModuleScene::CreateGameObject(GameObject* parent)
{
	GameObject* game_object = game_objects.Obtain();
	game_object->SetParent(parent);
	game_object->Init();

	game_objects_id_map[game_object->GetID()] = game_object;

	return game_object;
}

GameObject* ModuleScene::DuplicateGameObject(GameObject* game_object)
{
	// NTH: Duplicate Game Objects
	return game_object;
}

void ModuleScene::DestroyGameObject(GameObject* game_object)
{
	if (game_object == nullptr) return;

	game_object->CleanUp();

	for (GameObject* child : game_object->GetChildren())
	{
		DestroyGameObject(child);
	}

	game_objects_id_map.erase(game_object->GetID());

	game_objects.Release(game_object);
}

GameObject* ModuleScene::GetGameObject(UID id) const
{
	if (game_objects_id_map.count(id) == 0) return nullptr;

	return game_objects_id_map.at(id);
}
