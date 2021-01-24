#include "ModuleScene.h"

#include "Globals.h"
#include "Application.h"
#include "Utils/Logging.h"
#include "FileSystem/SceneImporter.h"
#include "FileSystem/TextureImporter.h"
#include "FileSystem/JsonValue.h"
#include "Resources/Texture.h"
#include "Resources/CubeMap.h"
#include "Components/Component.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentDirectionalLight.h"
#include "Components/ComponentPointLight.h"
#include "Components/ComponentSpotLight.h"
#include "Components/ComponentMesh.h"
#include "Components/ComponentMaterial.h"
#include "Components/ComponentBoundingBox.h"
#include "Components/ComponentCamera.h"
#include "Modules/ModuleInput.h"
#include "Modules/ModulePrograms.h"
#include "Modules/ModuleCamera.h"
#include "Modules/ModuleResources.h"
#include "Modules/ModuleFiles.h"
#include "Modules/ModuleEditor.h"
#include "Panels/PanelHierarchy.h"

#include "GL/glew.h"
#include "Math/myassert.h"
#include "assimp/cimport.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "Math/float4x4.h"
#include "Geometry/Sphere.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/reader.h"
#include "rapidjson/error/en.h"
#include <string>
#include "Brofiler.h"

#include "Utils/Leaks.h"

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

	return true;
}

bool ModuleScene::Start()
{
	App->files->CreateFolder("Library");
	App->files->CreateFolder(TEXTURES_PATH);
	App->files->CreateFolder(MESHES_PATH);
	App->files->CreateFolder(SCENES_PATH);

	CreateEmptyScene();

	SceneImporter::LoadScene("survival_shooter");

	// Load skybox
	// clang-format off
	float skybox_vertices[] = {
		// Front (x, y, z)
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
	
		// Left (x, y, z)
		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,
		
		// Right (x, y, z)
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 
		// Back (x, y, z)
		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,
	
		// Top (x, y, z)
		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,
		
		// Bottom (x, y, z)
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	}; // clang-format on

	// Skybox VAO
	glGenVertexArrays(1, &skybox_vao);
	glGenBuffers(1, &skybox_vbo);
	glBindVertexArray(skybox_vao);
	glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
	glBindVertexArray(0);

	skybox_cube_map = App->resources->ObtainCubeMap();
	skybox_cube_map->file_names[0] = "right";
	skybox_cube_map->file_names[1] = "left";
	skybox_cube_map->file_names[2] = "top";
	skybox_cube_map->file_names[3] = "bottom";
	skybox_cube_map->file_names[4] = "front";
	skybox_cube_map->file_names[5] = "back";
	TextureImporter::LoadCubeMap(skybox_cube_map);

	return true;
}

UpdateStatus ModuleScene::Update()
{
	BROFILER_CATEGORY("ModuleScene - Update", Profiler::Color::Green)

	// Load scene/fbx if one gets dropped
	const char* dropped_file_path = App->input->GetDroppedFilePath();
	if (dropped_file_path != nullptr)
	{
		std::string dropped_file_extension = App->files->GetFileExtension(dropped_file_path);
		std::string dropped_file_name = App->files->GetFileName(dropped_file_path);
		if (dropped_file_extension == SCENE_EXTENSION)
		{
			SceneImporter::LoadScene(dropped_file_name.c_str());

			LOG("Scene loaded");
		}
		else if (dropped_file_extension == ".fbx")
		{
			SceneImporter::ImportScene(dropped_file_path, root);

			LOG("Scene imported");
		}
		else if (dropped_file_extension == ".png" || dropped_file_extension == ".tif" || dropped_file_extension == ".dds")
		{
			Texture* texture = TextureImporter::ImportTexture(dropped_file_path);
			TextureImporter::LoadTexture(texture);

			LOG("Texture imported");
		}

		App->input->ReleaseDroppedFilePath();
	}

	// Update GameObjects
	for (GameObject& game_object : game_objects)
	{
		game_object.Update();
	}

	return UpdateStatus::CONTINUE;
}

bool ModuleScene::CleanUp()
{
	glDeleteVertexArrays(1, &skybox_vao);
	glDeleteBuffers(1, &skybox_vbo);

	ClearScene();

#ifdef _DEBUG
	aiDetachAllLogStreams();
#endif

	return true;
}

void ModuleScene::CreateEmptyScene()
{
	ClearScene();

	// Create Scene root node
	root = CreateGameObject(nullptr);
	root->name = "Scene";
	ComponentTransform* scene_transform = root->CreateComponent<ComponentTransform>();
	scene_transform->SetPosition(float3(0, 0, 0));
	scene_transform->SetRotation(Quat::identity);
	scene_transform->SetScale(float3(1, 1, 1));
	root->InitComponents();

	// Create Directional Light
	GameObject* dir_light = CreateGameObject(root);
	dir_light->name = "Directional Light";
	ComponentTransform* dir_light_transform = dir_light->CreateComponent<ComponentTransform>();
	dir_light_transform->SetPosition(float3(0, 300, 0));
	dir_light_transform->SetRotation(Quat::FromEulerXYZ(pi / 2, 0.0f, 0.0));
	dir_light_transform->SetScale(float3(1, 1, 1));
	ComponentDirectionalLight* dir_light_light = dir_light->CreateComponent<ComponentDirectionalLight>();
	dir_light->InitComponents();

	// Create Point Light
	GameObject* point_light = CreateGameObject(root);
	point_light->name = "Point Light";
	ComponentTransform* point_light_transform = point_light->CreateComponent<ComponentTransform>();
	point_light_transform->SetPosition(float3(0, 10, 0));
	point_light_transform->SetRotation(Quat::identity);
	point_light_transform->SetScale(float3(1, 1, 1));
	ComponentPointLight* point_light_light = point_light->CreateComponent<ComponentPointLight>();
	point_light->InitComponents();

	// Create Spot Light
	GameObject* spot_light = CreateGameObject(root);
	spot_light->name = "Spot Light";
	ComponentTransform* spot_light_transform = spot_light->CreateComponent<ComponentTransform>();
	spot_light_transform->SetPosition(float3(0, 10, 0));
	spot_light_transform->SetRotation(Quat::FromEulerXYZ(pi / 2, 0.0f, 0.0));
	spot_light_transform->SetScale(float3(1, 1, 1));
	ComponentSpotLight* spot_light_light = spot_light->CreateComponent<ComponentSpotLight>();
	spot_light->InitComponents();

	// Create Game Camera
	GameObject* game_camera = CreateGameObject(root);
	game_camera->name = "Game Camera";
	ComponentTransform* game_camera_transform = game_camera->CreateComponent<ComponentTransform>();
	game_camera_transform->SetPosition(float3(2, 3, -5));
	game_camera_transform->SetRotation(Quat::identity);
	game_camera_transform->SetScale(float3(1, 1, 1));
	ComponentCamera* game_camera_camera = game_camera->CreateComponent<ComponentCamera>();
	game_camera->InitComponents();
}

void ModuleScene::ClearScene()
{
	DestroyGameObject(root);
	root = nullptr;
	quadtree.Clear();

	assert(game_objects.Count() == 0);
}

void ModuleScene::RebuildQuadtree()
{
	quadtree.Initialize(quadtree_bounds, quadtree_max_depth, quadtree_elements_per_node);
	for (GameObject& game_object : game_objects)
	{
		ComponentBoundingBox* bounding_box = game_object.GetComponent<ComponentBoundingBox>();
		if (bounding_box == nullptr) continue;

		bounding_box->CalculateWorldBoundingBox();
		const AABB& world_aabb = bounding_box->GetWorldAABB();
		quadtree.Add(&game_object, AABB2D(world_aabb.minPoint.xz(), world_aabb.maxPoint.xz()));
		game_object.is_in_quadtree = true;
	}
	quadtree.Optimize();
}

void ModuleScene::ClearQuadtree()
{
	quadtree.Clear();
	for (GameObject& game_object : game_objects)
	{
		game_object.is_in_quadtree = false;
	}
}

GameObject* ModuleScene::CreateGameObject(GameObject* parent)
{
	GameObject* game_object = game_objects.Obtain();
	game_object->Init();
	game_object->SetParent(parent);
	game_objects_id_map[game_object->GetID()] = game_object;

	return game_object;
}

GameObject* ModuleScene::DuplicateGameObject(GameObject* game_object)
{
	// TODO: Duplicate Game Objects
	return game_object;
}

void ModuleScene::DestroyGameObject(GameObject* game_object)
{
	if (game_object == nullptr) return;

	// We need a copy because we are invalidating the iterator by removing GameObjects
	std::vector<GameObject*> children = game_object->GetChildren();
	for (GameObject* child : children)
	{
		DestroyGameObject(child);
	}

	if (game_object->is_in_quadtree)
	{
		quadtree.Remove(game_object);
	}

	game_objects_id_map.erase(game_object->GetID());
	game_object->id = 0;
	for (Component* component : game_object->components)
	{
		delete component;
	}
	game_object->components.clear();
	game_object->Enable();
	game_object->SetParent(nullptr);
	game_objects.Release(game_object);
}

GameObject* ModuleScene::GetGameObject(UID id) const
{
	if (game_objects_id_map.count(id) == 0) return nullptr;

	return game_objects_id_map.at(id);
}
