#include "ModuleScene.h"

#include "Globals.h"
#include "Application.h"
#include "Logging.h"
#include "ModuleInput.h"
#include "ModulePrograms.h"
#include "ModuleCamera.h"
#include "ModuleResources.h"
#include "ModuleFiles.h"
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
	// Loading test TODO: remove
	//Load("Test");

	// TODO: Remove after test
	SceneImporter::ImportScene("Assets/Street_Environment/Street_environment_V01.fbx");

	//SceneImporter::ImportScene("Assets/BakerHouse.fbx");

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

	// Create Directional Light
	GameObject* game_object = CreateGameObject(root);
	game_object->name = "Directional Light";

	ComponentTransform* transform = game_object->CreateComponent<ComponentTransform>();
	transform->SetPosition(float3(0, 0, 0));
	transform->SetRotation(Quat::identity);
	transform->SetScale(float3(1, 1, 1));
	transform->CalculateGlobalMatrix();
	ComponentLight* light = game_object->CreateComponent<ComponentLight>();

	// Create Game Camera
	game_object = CreateGameObject(root);
	game_object->name = "Game Camera";
	scene_cameras.push_back(game_object);

	transform = game_object->CreateComponent<ComponentTransform>();
	transform->SetPosition(float3(2, 3, -5));
	transform->SetRotation(Quat::identity);
	transform->SetScale(float3(1, 1, 1));
	transform->CalculateGlobalMatrix();
	ComponentCamera* camera = game_object->CreateComponent<ComponentCamera>();
	camera->Init();

	// Saving test TODO: remove
	//Save("Test");

	return true;
}

	return true;
}

UpdateStatus ModuleScene::Update()
{
	// Load scene if one gets dropped
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

void ModuleScene::ClearScene()
{
	DestroyGameObject(root);
	root = nullptr;

	assert(game_objects.Count() == 0);

	root = CreateGameObject(nullptr);
	root->name = "Scene";
	ComponentTransform* transform = root->CreateComponent<ComponentTransform>();
	transform->SetPosition(float3(0, 0, 0));
	transform->SetRotation(Quat::identity);
	transform->SetScale(float3(1, 1, 1));
	transform->CalculateGlobalMatrix();
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

	for (GameObject* child : game_object->GetChildren())
	{
		DestroyGameObject(child);
	}

	game_object->CleanUp();
	game_objects_id_map.erase(game_object->GetID());
	game_objects.Release(game_object);
}

GameObject* ModuleScene::GetGameObject(UID id) const
{
	if (game_objects_id_map.count(id) == 0) return nullptr;

	return game_objects_id_map.at(id);
}