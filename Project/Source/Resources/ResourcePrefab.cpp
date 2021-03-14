#include "ResourcePrefab.h"

#include "Application.h"
#include "Modules/ModuleScene.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleFiles.h"
#include "Utils/Logging.h"

#include "rapidjson/error/en.h"

#define JSON_TAG_GAMEOBJECTS "GameObjects"
#define JSON_TAG_ROOT_ID "RootId"

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
	unsigned jGameObjectsSize = jGameObjects.Size();
	Buffer<UID> ids(jGameObjectsSize);
	for (unsigned i = 0; i < jGameObjectsSize; ++i) {
		JsonValue jGameObject = jGameObjects[i];

		GameObject* gameObject = App->scene->gameObjects.Obtain();
		gameObject->Load(jGameObject);

		UID id = gameObject->GetID();
		App->scene->gameObjectsIdMap[id] = gameObject;
		ids[i] = id;
	}

	// Post-load
	App->scene->root = App->scene->GetGameObject(jScene[JSON_TAG_ROOT_ID]);
	for (unsigned i = 0; i < jGameObjectsSize; ++i) {
		JsonValue jGameObject = jGameObjects[i];

		UID id = ids[i];
		GameObject* gameObject = App->scene->GetGameObject(id);
		gameObject->PostLoad(jGameObject);
	}

	// Init components
	for (unsigned i = 0; i < jGameObjectsSize; ++i) {
		JsonValue jGameObject = jGameObjects[i];

		UID id = ids[i];
		GameObject* gameObject = App->scene->GetGameObject(id);
		gameObject->InitComponents();
	}

	unsigned timeMs = timer.Stop();
	LOG("Scene built in %ums.", timeMs);
}
