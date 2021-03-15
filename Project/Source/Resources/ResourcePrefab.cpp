#include "ResourcePrefab.h"

#include "Application.h"
#include "Modules/ModuleScene.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleFiles.h"
#include "Utils/Logging.h"

#include "rapidjson/error/en.h"

#define JSON_TAG_GAMEOBJECTS "GameObjects"
#define JSON_TAG_ROOT_ID "RootId"
#define JSON_TAG_NAME "Name"
#define JSON_TAG_PARENT_INDEX "ParentIndex"

ResourcePrefab::ResourcePrefab(UID id, const char* assetFilePath, const char* resourceFilePath)
	: Resource(id, assetFilePath, resourceFilePath) {}

void ResourcePrefab::BuildPrefab(GameObject* parent) {
	// Timer to measure loading a prefab
	MSTimer timer;
	timer.Start();

	// Read from file
	std::string filePath = GetResourceFilePath();
	Buffer<char> buffer = App->files->Load(filePath.c_str());

	if (buffer.Size() == 0) return;

	// Parse document from file
	rapidjson::Document document;
	document.ParseInsitu<rapidjson::kParseNanAndInfFlag>(buffer.Data());
	if (document.HasParseError()) {
		LOG("Error parsing JSON: %s (offset: %u)", rapidjson::GetParseError_En(document.GetParseError()), document.GetErrorOffset());
		return;
	}
	JsonValue jScene(document, document);

	// Load GameObjects
	JsonValue jGameObjects = jScene[JSON_TAG_GAMEOBJECTS];
	unsigned numGameObjects = jGameObjects.Size();
	Buffer<GameObject*> gameObjects(numGameObjects);
	for (unsigned i = 0; i < numGameObjects; ++i) {
		JsonValue jGameObject = jGameObjects[i];

		GameObject* gameObject = App->scene->gameObjects.Obtain();
		gameObject->Init();
		gameObject->LoadPrototype(jGameObject);
		gameObjects[i] = gameObject;
	}

	// Post-load
	App->scene->root = App->scene->GetGameObject(jScene[JSON_TAG_ROOT_ID]);
	for (unsigned i = 0; i < numGameObjects; ++i) {
		JsonValue jGameObject = jGameObjects[i];
		GameObject* gameObject = gameObjects[i];

		std::string name = jGameObject[JSON_TAG_NAME];
		int parentIndex = jGameObject[JSON_TAG_PARENT_INDEX];
		UID id = GenerateUID();
		gameObject->id = id;
		gameObject->name = name;
		App->scene->gameObjectsIdMap[id] = gameObject;
		if (parentIndex >= 0) {
			gameObject->SetParent(gameObjects[parentIndex]);
		} else {
			gameObject->SetParent(parent);
		}
	}

	// Init components
	for (unsigned i = 0; i < numGameObjects; ++i) {
		GameObject* gameObject = gameObjects[i];

		gameObject->InitComponents();
	}

	unsigned timeMs = timer.Stop();
	LOG("Scene built in %ums.", timeMs);
}
