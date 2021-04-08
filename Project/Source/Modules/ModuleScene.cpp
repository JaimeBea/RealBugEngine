#include "ModuleScene.h"

#include "Globals.h"
#include "Application.h"
#include "Utils/Logging.h"
#include "Utils/FileDialog.h"
#include "FileSystem/SceneImporter.h"
#include "FileSystem/TextureImporter.h"
#include "FileSystem/JsonValue.h"
#include "Resources/ResourceTexture.h"
#include "Resources/ResourceSkybox.h"
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
#include "Modules/ModuleEvents.h"
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

#include <Windows.h>
#include <array>

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
	App->events->AddObserverToEvent(EventType::GAMEOBJECT_DESTROYED, this);
	App->files->CreateFolder(LIBRARY_PATH);
	App->files->CreateFolder(TEXTURES_PATH);
	App->files->CreateFolder(SCENES_PATH);

	CreateEmptyScene();

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

	// Create Skybox
	ComponentSkyBox* skybox = gameCamera->CreateComponent<ComponentSkyBox>();
	gameCamera->InitComponents();
}

void ModuleScene::DestroyGameObjectDeferred(GameObject* gameObject) {
	if (gameObject == nullptr) return;

	const std::vector<GameObject*>& children = gameObject->GetChildren();
	for (GameObject* child : children) {
		DestroyGameObjectDeferred(child);
	}
	Event ev(EventType::GAMEOBJECT_DESTROYED);
	ev.destroyGameObject.ptr = gameObject;
	App->events->AddEvent(ev);
}

void ModuleScene::ReceiveEvent(const Event& e) {
	switch (e.type) {
	case EventType::GAMEOBJECT_DESTROYED:
		scene->DestroyGameObject(e.destroyGameObject.ptr);
		break;
	}
}