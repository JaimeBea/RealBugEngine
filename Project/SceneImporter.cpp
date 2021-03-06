#include "SceneImporter.h"

#include "Logging.h"
#include "Application.h"
#include "MeshImporter.h"
#include "TextureImporter.h"
#include "GameObject.h"
#include "Material.h"
#include "MSTimer.h"
#include "ComponentTransform.h"
#include "ComponentBoundingBox.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "ModuleFiles.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "ModuleResources.h"

#include "assimp/scene.h"
#include "assimp/cimport.h"
#include "assimp/postprocess.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/error/en.h"
#include <string>

#include "Leaks.h"

static void ImportNode(const aiScene* ai_scene, const std::vector<Material>& materials, const aiNode* node, GameObject* parent, const float4x4& accumulated_transform)
{
	std::string name = node->mName.C_Str();
	LOG("Importing node: \"%s\"", name.c_str());

	if (name.find("$AssimpFbx$") != std::string::npos) // Auxiliary node
	{
		// Import children nodes
		for (unsigned int i = 0; i < node->mNumChildren; ++i)
		{
			const float4x4& transform = accumulated_transform * (*(float4x4*) &node->mTransformation);
			ImportNode(ai_scene, materials, node->mChildren[i], parent, transform);
		}
	}
	else // Normal node
	{
		// Create GameObject
		GameObject* game_object = App->scene->CreateGameObject(parent);

		// Load name
		game_object->name = name;

		// Load transform
		ComponentTransform* transform = game_object->CreateComponent<ComponentTransform>();
		const float4x4& matrix = accumulated_transform * (*(float4x4*) &node->mTransformation);
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
			mesh->mesh = MeshImporter::ImportMesh(ai_mesh, i);
			mesh->mesh->material_index = i;

			// TODO: Move mesh loading to a better place
			MeshImporter::LoadMesh(mesh->mesh);

			ComponentMaterial* material = game_object->CreateComponent<ComponentMaterial>();
			if (materials.size() > 0)
			{
				if (ai_mesh->mMaterialIndex >= materials.size())
				{
					material->material = materials.front();
					LOG("Invalid material found", ai_mesh->mMaterialIndex);
				}
				else
				{
					material->material = materials[ai_mesh->mMaterialIndex];
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
		if (min_point.x < max_point.x)
		{
			ComponentBoundingBox* bounding_box = game_object->CreateComponent<ComponentBoundingBox>();
			bounding_box->SetLocalBoundingBox(AABB(min_point, max_point));
			bounding_box->CalculateWorldBoundingBox();
		}

		// Import children nodes
		for (unsigned int i = 0; i < node->mNumChildren; ++i)
		{
			ImportNode(ai_scene, materials, node->mChildren[i], game_object, float4x4::identity);
		}
	}
}

bool SceneImporter::ImportScene(const char* file_path, GameObject* parent)
{
	// Timer to measure importing a scene
	MSTimer timer;
	timer.Start();

	// Check for extension support
	std::string extension = App->files->GetFileExtension(file_path);
	if (!aiIsExtensionSupported(extension.c_str()))
	{
		LOG("Extension is not supported by assimp: \"%s\".", extension);
		return false;
	}

	// Import scene
	LOG("Importing scene from path: \"%s\".", file_path);
	const aiScene* ai_scene = aiImportFile(file_path, aiProcessPreset_TargetRealtime_MaxQuality);
	DEFER
	{
		aiReleaseImport(ai_scene);
	};
	if (!ai_scene)
	{
		LOG("Error importing scene: %s", file_path, aiGetErrorString());
		return false;
	}

	// Load materials
	LOG("Importing %i materials...", ai_scene->mNumMaterials);
	std::vector<Material> materials;
	materials.reserve(ai_scene->mNumMaterials);
	for (unsigned int i = 0; i < ai_scene->mNumMaterials; ++i)
	{
		LOG("Loading material %i...", i);
		aiMaterial* ai_material = ai_scene->mMaterials[i];
		aiString material_file_path;
		aiTextureMapping mapping;
		aiColor4D color;
		unsigned uv_index;
		Material material;
		if (ai_material->GetTexture(aiTextureType_DIFFUSE, 0, &material_file_path, &mapping, &uv_index) == AI_SUCCESS)
		{
			// Check if the material is valid for our purposes
			assert(mapping == aiTextureMapping_UV);
			assert(uv_index == 0);

			// Try to load from the path given in the model file
			LOG("Trying to import diffuse texture...");
			Texture* texture = TextureImporter::ImportTexture(material_file_path.C_Str());

			// Try to load relative to the model folder
			if (texture == nullptr)
			{
				LOG("Trying to import texture relative to model folder...");
				std::string model_folder_path = App->files->GetFileFolder(file_path);
				std::string model_folder_material_file_path = model_folder_path + "/" + material_file_path.C_Str();
				texture = TextureImporter::ImportTexture(model_folder_material_file_path.c_str());
			}

			// Try to load relative to the textures folder
			if (texture == nullptr)
			{
				LOG("Trying to import texture relative to textures folder...");
				std::string material_file = App->files->GetFileNameAndExtension(material_file_path.C_Str());
				std::string textures_folder_material_file_dir = std::string(TEXTURES_PATH) + "/" + material_file;
				texture = TextureImporter::ImportTexture(textures_folder_material_file_dir.c_str());
			}

			if (texture == nullptr)
			{
				LOG("Unable to find diffuse texture file.");
			}
			else
			{
				LOG("Diffuse texture imported successfuly.");
				material.has_diffuse_map = true;
				material.diffuse_map = texture;
				// TODO: Move load to a better place
				TextureImporter::LoadTexture(texture);
			}
		}
		else
		{
			LOG("Diffuse texture not found.");
		}

		if (ai_material->GetTexture(aiTextureType_SPECULAR, 0, &material_file_path, &mapping, &uv_index) == AI_SUCCESS)
		{
			// Check if the material is valid for our purposes
			assert(mapping == aiTextureMapping_UV);
			assert(uv_index == 0);

			// Try to load from the path given in the model file
			LOG("Trying to import specular texture...");
			Texture* texture = TextureImporter::ImportTexture(material_file_path.C_Str());

			// Try to load relative to the model folder
			if (texture == nullptr)
			{
				LOG("Trying to import texture relative to model folder...");
				std::string model_folder_path = App->files->GetFileFolder(file_path);
				std::string model_folder_material_file_path = model_folder_path + "/" + material_file_path.C_Str();
				texture = TextureImporter::ImportTexture(model_folder_material_file_path.c_str());
			}

			// Try to load relative to the textures folder
			if (texture == nullptr)
			{
				LOG("Trying to import texture relative to textures folder...");
				std::string material_file_name = App->files->GetFileName(material_file_path.C_Str());
				std::string textures_folder_material_file_dir = std::string(TEXTURES_PATH) + "/" + material_file_name + TEXTURE_EXTENSION;
				texture = TextureImporter::ImportTexture(textures_folder_material_file_dir.c_str());
			}

			if (texture == nullptr)
			{
				LOG("Unable to find specular texture file.");
			}
			else
			{
				LOG("Specular texture imported successfuly.");
				material.has_specular_map = true;
				material.specular_map = texture;
				// TODO: Move load to a better place
				TextureImporter::LoadTexture(texture);
			}
		}
		else
		{
			LOG("Specular texture not found.");
		}

		ai_material->Get(AI_MATKEY_COLOR_DIFFUSE, material.diffuse_color);
		ai_material->Get(AI_MATKEY_COLOR_SPECULAR, material.specular_color);
		ai_material->Get(AI_MATKEY_SHININESS, material.shininess);

		LOG("Material imported.");
		materials.push_back(material);
	}

	// Create scene tree
	LOG("Importing scene tree.");
	ImportNode(ai_scene, materials, ai_scene->mRootNode, parent, float4x4::identity);

	unsigned time_ms = timer.Stop();
	LOG("Scene imported in %ums.", time_ms);
	return true;
}

bool SceneImporter::LoadScene(const char* file_name)
{
	// Clear scene
	App->scene->ClearScene();
	App->editor->selected_object = nullptr;

	// Timer to measure loading a scene
	MSTimer timer;
	timer.Start();

	// Read from file
	std::string file_path = std::string(SCENES_PATH) + "/" + file_name + SCENE_EXTENSION;
	Buffer<char> buffer = App->files->Load(file_path.c_str());

	if (buffer.Size() == 0) return false;

	// Parse document from file
	rapidjson::Document document;
	document.ParseInsitu<rapidjson::kParseNanAndInfFlag>(buffer.Data());
	if (document.HasParseError())
	{
		LOG("Error parsing JSON: %s (offset: %u)", rapidjson::GetParseError_En(document.GetParseError()), document.GetErrorOffset());
		return false;
	}
	JsonValue j_scene(document, document);

	// Load GameObjects
	JsonValue j_game_objects = j_scene["GameObjects"];
	unsigned j_game_objects_size = j_game_objects.Size();
	Buffer<UID> ids(j_game_objects_size);
	for (unsigned i = 0; i < j_game_objects_size; ++i)
	{
		JsonValue j_game_object = j_game_objects[i];

		GameObject* game_object = App->scene->game_objects.Obtain();
		game_object->Load(j_game_object);

		UID id = game_object->GetID();
		App->scene->game_objects_id_map[id] = game_object;
		ids[i] = id;
	}

	// Post-load
	App->scene->root = App->scene->GetGameObject(j_scene["RootId"]);
	for (unsigned i = 0; i < j_game_objects_size; ++i)
	{
		JsonValue j_game_object = j_game_objects[i];

		UID id = ids[i];
		GameObject* game_object = App->scene->GetGameObject(id);
		game_object->PostLoad(j_game_object);
	}
	
	// Init components
	for (unsigned i = 0; i < j_game_objects_size; ++i)
	{
		JsonValue j_game_object = j_game_objects[i];

		UID id = ids[i];
		GameObject* game_object = App->scene->GetGameObject(id);
		game_object->InitComponents();
	}

	// Quadtree generation
	JsonValue j_quadtree_bounds = j_scene["QuadtreeBounds"];
	App->scene->quadtree_bounds = {{j_quadtree_bounds[0], j_quadtree_bounds[1]}, {j_quadtree_bounds[2], j_quadtree_bounds[3]}};
	App->scene->quadtree_max_depth = j_scene["QuadtreeMaxDepth"];
	App->scene->quadtree_elements_per_node = j_scene["QuadtreeElementsPerNode"];
	App->scene->RebuildQuadtree();

	unsigned time_ms = timer.Stop();
	LOG("Scene loaded in %ums.", time_ms);
	return true;
}

bool SceneImporter::SaveScene(const char* file_name)
{
	// Create document
	rapidjson::Document document;
	document.SetObject();
	JsonValue j_scene(document, document);

	// Save scene information
	j_scene["RootId"] = App->scene->root->GetID();
	JsonValue j_quadtree_bounds = j_scene["QuadtreeBounds"];
	j_quadtree_bounds[0] = App->scene->quadtree_bounds.minPoint.x;
	j_quadtree_bounds[1] = App->scene->quadtree_bounds.minPoint.y;
	j_quadtree_bounds[2] = App->scene->quadtree_bounds.maxPoint.x;
	j_quadtree_bounds[3] = App->scene->quadtree_bounds.maxPoint.y;
	j_scene["QuadtreeMaxDepth"] = App->scene->quadtree_max_depth;
	j_scene["QuadtreeElementsPerNode"] = App->scene->quadtree_elements_per_node;

	// Save GameObjects
	JsonValue j_game_objects = j_scene["GameObjects"];
	unsigned i = 0;
	for (const GameObject& game_object : App->scene->game_objects)
	{
		JsonValue j_game_object = j_game_objects[i];

		GameObject* parent = game_object.GetParent();
		j_game_object["ParentId"] = parent != nullptr ? parent->id : 0;

		game_object.Save(j_game_object);

		i += 1;
	}

	// Write document to buffer
	rapidjson::StringBuffer string_buffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer, rapidjson::UTF8<>, rapidjson::UTF8<>, rapidjson::CrtAllocator, rapidjson::kWriteNanAndInfFlag> writer(string_buffer);
	document.Accept(writer);

	// Save to file
	std::string file_path = std::string(SCENES_PATH) + "/" + file_name + SCENE_EXTENSION;
	App->files->Save(file_path.c_str(), string_buffer.GetString(), string_buffer.GetSize());

	return true;
}
