#include "ModuleScene.h"

#include "Globals.h"
#include "Application.h"
#include "Utils/Logging.h"
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
#include "Utils/FileDialog.h"
static aiLogStream logStream = {nullptr, nullptr};

static void AssimpLogCallback(const char* message, char* user) {
	std::string messageStr = message;
	std::string finalMessageStr = messageStr.substr(0, messageStr.find_last_of('\n'));
	LOG(finalMessageStr.c_str());
}

bool ModuleScene::Init() {
	gameObjects.Allocate(10000);

#ifdef _DEBUG
	logStream.callback = AssimpLogCallback;
	aiAttachLogStream(&logStream);
#endif

	return true;
}

bool ModuleScene::Start() {
	App->files->CreateFolder("Library");
	App->files->CreateFolder(TEXTURES_PATH);
	App->files->CreateFolder(MESHES_PATH);
	App->files->CreateFolder(SCENES_PATH);

	CreateEmptyScene();

	return true;
}

UpdateStatus ModuleScene::Update() {
	BROFILER_CATEGORY("ModuleScene - Update", Profiler::Color::Green)

	// Update GameObjects
	root->Update();

	return UpdateStatus::CONTINUE;
}

bool ModuleScene::CleanUp() {
	// TODO: (Texture resource) make skybox work
	/*
	glDeleteVertexArrays(1, &skyboxVao);
	glDeleteBuffers(1, &skyboxVbo);
	*/

	ClearScene();

#ifdef _DEBUG
	aiDetachAllLogStreams();
#endif

	return true;
}

void ModuleScene::CreateEmptyScene() {
	ClearScene();

	// Create Scene root node
	root = CreateGameObject(nullptr);
	root->name = "Scene";
	ComponentTransform* sceneTransform = root->CreateComponent<ComponentTransform>();
	/*sceneTransform->SetPosition(float3(0, 0, 0));
	sceneTransform->SetRotation(Quat::identity);
	sceneTransform->SetScale(float3(1, 1, 1));*/
	root->InitComponents();

	// Create Directional Light
	GameObject* dirLight = CreateGameObject(root);
	dirLight->name = "Directional Light";
	ComponentTransform* dirLightTransform = dirLight->CreateComponent<ComponentTransform>();
	dirLightTransform->SetPosition(float3(0, 300, 0));
	dirLightTransform->SetRotation(Quat::FromEulerXYZ(pi / 2, 0.0f, 0.0));
	dirLightTransform->SetScale(float3(1, 1, 1));
	ComponentLight* dirLightLight = dirLight->CreateComponent<ComponentLight>();
	dirLight->InitComponents();

	// Create Game Camera
	GameObject* gameCamera = CreateGameObject(root);
	gameCamera->name = "Game Camera";
	ComponentTransform* gameCameraTransform = gameCamera->CreateComponent<ComponentTransform>();
	gameCameraTransform->SetPosition(float3(2, 3, -5));
	gameCameraTransform->SetRotation(Quat::identity);
	gameCameraTransform->SetScale(float3(1, 1, 1));
	ComponentCamera* gameCameraCamera = gameCamera->CreateComponent<ComponentCamera>();
	//gameCamera->InitComponents();

	// Create Skybox
	/*ComponentSkyBox* skybox = gameCamera->CreateComponent<ComponentSkyBox>();*/
	gameCamera->InitComponents();
}

void ModuleScene::ClearScene() {
	DestroyGameObject(root);
	root = nullptr;
	quadtree.Clear();

	assert(gameObjects.Count() == 0); // There should be no GameObjects outside the scene hierarchy
	gameObjects.ReleaseAll();		  // This looks redundant, but it resets the free list so that GameObject order is mantained when saving/loading
}

void ModuleScene::RebuildQuadtree() {
	quadtree.Initialize(quadtreeBounds, quadtreeMaxDepth, quadtreeElementsPerNode);
	for (ComponentBoundingBox& boundingBox : boundingBoxComponents) {
		GameObject& gameObject = boundingBox.GetOwner();
		boundingBox.CalculateWorldBoundingBox();
		const AABB& worldAABB = boundingBox.GetWorldAABB();
		quadtree.Add(&gameObject, AABB2D(worldAABB.minPoint.xz(), worldAABB.maxPoint.xz()));
		gameObject.isInQuadtree = true;
	}
	quadtree.Optimize();
}

void ModuleScene::ClearQuadtree() {
	quadtree.Clear();
	for (GameObject& gameObject : gameObjects) {
		gameObject.isInQuadtree = false;
	}
}

GameObject* ModuleScene::CreateGameObject(GameObject* parent) {
	GameObject* gameObject = gameObjects.Obtain();
	gameObject->Init();
	gameObject->SetParent(parent);
	gameObjectsIdMap[gameObject->GetID()] = gameObject;

	return gameObject;
}

GameObject* ModuleScene::DuplicateGameObject(GameObject* gameObject, GameObject* parent) {
	GameObject* newGO = CreateGameObject(parent); // ID and parent are set here
	// Copy Game Object members
	newGO->name = gameObject->name + " (copy)";

	// Copy the components
	for (Component* component : gameObject->GetComponents()) {
		component->DuplicateComponent(*newGO);
	}
	newGO->InitComponents();
	// Duplicate recursively its children
	for (GameObject* child : gameObject->GetChildren()) {
		DuplicateGameObject(child, newGO);
	}
	return newGO;
}

void ModuleScene::DestroyGameObject(GameObject* gameObject) {
	if (gameObject == nullptr) return;

	// We need a copy because we are invalidating the iterator by removing GameObjects
	std::vector<GameObject*> children = gameObject->GetChildren();
	for (GameObject* child : children) {
		DestroyGameObject(child);
	}

	if (gameObject->isInQuadtree) {
		quadtree.Remove(gameObject);
	}

	gameObjectsIdMap.erase(gameObject->GetID());
	gameObject->RemoveComponents();
	gameObject->SetParent(nullptr);
	gameObjects.Release(gameObject);
}

GameObject* ModuleScene::GetGameObject(UID id) const {
	if (gameObjectsIdMap.count(id) == 0) return nullptr;

	return gameObjectsIdMap.at(id);
}