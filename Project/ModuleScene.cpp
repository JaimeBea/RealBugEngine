#include "ModuleScene.h"

#include "Globals.h"
#include "Application.h"
#include "Logging.h"
#include "ModuleInput.h"
#include "ModulePrograms.h"
#include "ModuleCamera.h"
#include "ModuleResources.h"
#include "ModuleFiles.h"
#include "ModuleEditor.h"
#include "PanelHierarchy.h"
#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentLight.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentBoundingBox.h"
#include "ComponentCamera.h"
#include "Texture.h"
#include "CubeMap.h"
#include "JsonValue.h"
#include "SceneImporter.h"
#include "TextureImporter.h"

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
	ClearScene();

	return true;
}

bool ModuleScene::Start()
{
	CreateEmptyScene();

	// Loading test TODO: remove
	//Load("Test");

	// TODO: Remove after test
	//SceneImporter::ImportScene("Assets/Street_Environment/Street_environment_V01.fbx", App->scene->root);

	SceneImporter::ImportScene("Assets/BakerHouse.fbx", App->scene->root);

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

	const char* files[6] = {
		"Assets/Skybox/right.jpg",
		"Assets/Skybox/left.jpg",
		"Assets/Skybox/top.jpg",
		"Assets/Skybox/bottom.jpg",
		"Assets/Skybox/front.jpg",
		"Assets/Skybox/back.jpg"};

	skybox_cube_map = TextureImporter::ImportCubeMap(files);
	TextureImporter::LoadCubeMap(skybox_cube_map);

	// Saving test TODO: remove
	//Save("Test");

	return true;
}

UpdateStatus ModuleScene::Update()
{
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
			SceneImporter::ImportScene(dropped_file_path, App->editor->panel_hierarchy.selected_object);

			LOG("Scene imported");
		}

		App->input->ReleaseDroppedFilePath();
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
	scene_transform->CalculateGlobalMatrix();

	// Create Directional Light
	GameObject* light = CreateGameObject(root);
	light->name = "Directional Light";
	ComponentTransform* light_transform = light->CreateComponent<ComponentTransform>();
	light_transform->SetPosition(float3(0, 0, 0));
	light_transform->SetRotation(Quat::identity);
	light_transform->SetScale(float3(1, 1, 1));
	light_transform->CalculateGlobalMatrix();
	ComponentLight* light_light = light->CreateComponent<ComponentLight>();

	// Create Game Camera
	GameObject* game_camera = CreateGameObject(root);
	game_camera->name = "Game Camera";
	ComponentTransform* game_camera_transform = game_camera->CreateComponent<ComponentTransform>();
	game_camera_transform->SetPosition(float3(2, 3, -5));
	game_camera_transform->SetRotation(Quat::identity);
	game_camera_transform->SetScale(float3(1, 1, 1));
	game_camera_transform->CalculateGlobalMatrix();
	ComponentCamera* game_camera_camera = game_camera->CreateComponent<ComponentCamera>();
	game_camera_camera->Init();
}

void ModuleScene::ClearScene()
{
	DestroyGameObject(root);
	root = nullptr;

	assert(game_objects.Count() == 0);
}

GameObject* ModuleScene::CreateGameObject(GameObject* parent)
{
	GameObject* game_object = game_objects.Obtain();
	game_objects_id_map[game_object->GetID()] = game_object;
	game_object->SetParent(parent);
	game_object->Init();

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

	// We need a copy because we are invalidating the iterator by removing GameObjects
	std::vector<GameObject*> children = game_object->GetChildren();
	for (GameObject* child : children)
	{
		DestroyGameObject(child);
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