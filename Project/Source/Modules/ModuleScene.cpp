#include "ModuleScene.h"

#include "Globals.h"
#include "Application.h"
#include "Utils/Logging.h"
#include "Utils/FileDialog.h"
#include "FileSystem/SceneImporter.h"
#include "FileSystem/TextureImporter.h"
#include "FileSystem/JsonValue.h"
#include "Resources/ResourceTexture.h"
#include "Components/Component.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentLight.h"
#include "Components/ComponentMeshRenderer.h"
#include "Components/ComponentBoundingBox.h"
#include "Components/ComponentCamera.h"
#include "Modules/ModuleInput.h"
#include "Modules/ModulePrograms.h"
#include "Modules/ModuleCamera.h"
#include "Modules/ModuleResources.h"
#include "Modules/ModuleFiles.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleEventSystem.h"
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

static aiLogStream logStream = {nullptr, nullptr};

static void AssimpLogCallback(const char* message, char* user) {
	std::string messageStr = message;
	std::string finalMessageStr = messageStr.substr(0, messageStr.find_last_of('\n'));
	LOG(finalMessageStr.c_str());
}

bool ModuleScene::Init() {
	scene = new Scene(10000);

#ifdef _DEBUG
	logStream.callback = AssimpLogCallback;
	aiAttachLogStream(&logStream);
#endif

	return true;
}

bool ModuleScene::Start() {
	App->eventSystem->AddObserverToEvent(Event::EventType::GAMEOBJECT_DESTROYED, this);
	App->files->CreateFolder(LIBRARY_PATH);
	App->files->CreateFolder(TEXTURES_PATH);
	App->files->CreateFolder(SCENES_PATH);

	CreateEmptyScene();

	// TODO: (Scene resource) Load default scene
	// SceneImporter::LoadScene("survival_shooter");

	// Load skybox
	// clang-format off
	float skyboxVertices[] = {
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
	// TODO: (Texture resource) Make skybox work
	/*
	glGenVertexArrays(1, &skyboxVao);
	glGenBuffers(1, &skyboxVbo);
	glBindVertexArray(skyboxVao);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
	glBindVertexArray(0);

	skyboxCubeMap = App->resources->ObtainCubeMap();
	skyboxCubeMap->fileNames[0] = "right";
	skyboxCubeMap->fileNames[1] = "left";
	skyboxCubeMap->fileNames[2] = "top";
	skyboxCubeMap->fileNames[3] = "bottom";
	skyboxCubeMap->fileNames[4] = "front";
	skyboxCubeMap->fileNames[5] = "back";
	TextureImporter::LoadCubeMap(skyboxCubeMap);
	*/

	return true;
}

UpdateStatus ModuleScene::Update() {
	BROFILER_CATEGORY("ModuleScene - Update", Profiler::Color::Green)

	// Update GameObjects
	scene->root->Update();

	return UpdateStatus::CONTINUE;
}

bool ModuleScene::CleanUp() {
	// TODO: (Texture resource) make skybox work
	/*
	glDeleteVertexArrays(1, &skyboxVao);
	glDeleteBuffers(1, &skyboxVbo);
	*/

	scene->ClearScene();
	RELEASE(scene);

#ifdef _DEBUG
	aiDetachAllLogStreams();
#endif

	return true;
}

void ModuleScene::CreateEmptyScene() {
	scene->ClearScene();

	// Create Scene root node
	GameObject* root = scene->CreateGameObject(nullptr, GenerateUID(), "Scene");
	scene->root = root;
	ComponentTransform* sceneTransform = root->CreateComponent<ComponentTransform>();
	root->InitComponents();

	// Create Directional Light
	GameObject* dirLight = scene->CreateGameObject(root, GenerateUID(), "Directional Light");
	ComponentTransform* dirLightTransform = dirLight->CreateComponent<ComponentTransform>();
	dirLightTransform->SetPosition(float3(0, 300, 0));
	dirLightTransform->SetRotation(Quat::FromEulerXYZ(pi / 2, 0.0f, 0.0));
	dirLightTransform->SetScale(float3(1, 1, 1));
	ComponentLight* dirLightLight = dirLight->CreateComponent<ComponentLight>();
	dirLight->InitComponents();

	// Create Game Camera
	GameObject* gameCamera = scene->CreateGameObject(root, GenerateUID(), "Game Camera");
	ComponentTransform* gameCameraTransform = gameCamera->CreateComponent<ComponentTransform>();
	gameCameraTransform->SetPosition(float3(2, 3, -5));
	gameCameraTransform->SetRotation(Quat::identity);
	gameCameraTransform->SetScale(float3(1, 1, 1));
	ComponentCamera* gameCameraCamera = gameCamera->CreateComponent<ComponentCamera>();
	gameCamera->InitComponents();
}

void ModuleScene::DestroyGameObjectDeferred(GameObject* gameObject) {
	if (gameObject == nullptr) return;

	const std::vector<GameObject*>& children = gameObject->GetChildren();
	for (GameObject* child : children) {
		DestroyGameObjectDeferred(child);
	}

	App->BroadCastEvent(Event(Event::EventType::GAMEOBJECT_DESTROYED, gameObject));
}

void ModuleScene::ReceiveEvent(const Event& e) {
	switch (e.type) {
	case Event::EventType::GAMEOBJECT_DESTROYED:
		scene->DestroyGameObject(e.objPtr.ptr);
		break;
	}
}