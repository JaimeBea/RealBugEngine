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
#include "Components/ComponentLight.h"
#include "Components/ComponentMeshRenderer.h"
#include "Components/ComponentBoundingBox.h"
#include "Components/ComponentCamera.h"
#include "Modules/ModuleInput.h"
#include "Modules/ModulePrograms.h"
#include "Modules/ModuleCamera.h"
#include "Modules/ModuleResources.h"
#include "Modules/ModuleFiles.h"
#include "Modules/ModuleRender.h"
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
#include "UI/EventSystem/Event.h"

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

	App->eventSystem->AddObserverToEvent(Event::EventType::GameObject_Destroyed, this);

	return true;
}

bool ModuleScene::Start() {
	App->files->CreateFolder("Library");
	App->files->CreateFolder(TEXTURES_PATH);
	App->files->CreateFolder(MESHES_PATH);
	App->files->CreateFolder(SCENES_PATH);

	CreateEmptyScene();

	SceneImporter::LoadScene("survival_shooter");

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

	return true;
}

UpdateStatus ModuleScene::Update() {
	BROFILER_CATEGORY("ModuleScene - Update", Profiler::Color::Green)

	// Load scene/fbx if one gets dropped
	const char* droppedFilePath = App->input->GetDroppedFilePath();
	if (droppedFilePath != nullptr) {
		std::string droppedFileExtension = App->files->GetFileExtension(droppedFilePath);
		std::string droppedFileName = App->files->GetFileName(droppedFilePath);
		if (droppedFileExtension == SCENE_EXTENSION) {
			SceneImporter::LoadScene(droppedFileName.c_str());

			LOG("Scene loaded");
		} else if (droppedFileExtension == ".fbx") {
			SceneImporter::ImportScene(droppedFilePath, root);

			LOG("Scene imported");
		} else if (droppedFileExtension == ".png" || droppedFileExtension == ".tif" || droppedFileExtension == ".dds") {
			Texture* texture = TextureImporter::ImportTexture(droppedFilePath);
			TextureImporter::LoadTexture(texture);

			LOG("Texture imported");
		}

		App->input->ReleaseDroppedFilePath();
	}

	// Update GameObjects
	for (GameObject& gameObject : gameObjects) {
		gameObject.Update();
	}

	return UpdateStatus::CONTINUE;
}

bool ModuleScene::CleanUp() {
	glDeleteVertexArrays(1, &skyboxVao);
	glDeleteBuffers(1, &skyboxVbo);

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
	sceneTransform->SetPosition(float3(0, 0, 0));
	sceneTransform->SetRotation(Quat::identity);
	sceneTransform->SetScale(float3(1, 1, 1));
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
	gameCamera->InitComponents();
}

void ModuleScene::ClearScene() {
	DestroyGameObject(root);
	root = nullptr;
	quadtree.Clear();

	assert(gameObjects.Count() == 0); // There should be no GameObjects outside the scene hierarchy
	gameObjects.ReleaseAll(); // This looks redundant, but it resets the free list so that GameObject order is mantained when saving/loading
}

void ModuleScene::RebuildQuadtree() {
	quadtree.Initialize(quadtreeBounds, quadtreeMaxDepth, quadtreeElementsPerNode);
	for (GameObject& gameObject : gameObjects) {
		ComponentBoundingBox* boundingBox = gameObject.GetComponent<ComponentBoundingBox>();
		if (boundingBox == nullptr) continue;

		boundingBox->CalculateWorldBoundingBox();
		const AABB& worldAABB = boundingBox->GetWorldAABB();
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

GameObject* ModuleScene::DuplicateGameObject(GameObject* gameObject) {
	// TODO: Duplicate Game Objects
	return gameObject;
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
	gameObject->id = 0;
	for (Component* component : gameObject->components) {
		delete component;
	}
	gameObject->components.clear();
	gameObject->Enable();
	gameObject->SetParent(nullptr);
	gameObjects.Release(gameObject);

	Event ev = Event(Event::EventType::GameObject_Destroyed);
	ev.objPtr.ptr = gameObject;
	App->BroadCastEvent(ev);
}

GameObject* ModuleScene::GetGameObject(UID id) const {
	if (gameObjectsIdMap.count(id) == 0) return nullptr;

	return gameObjectsIdMap.at(id);
}
