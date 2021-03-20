#include "ResourceScene.h"

#include "Application.h"
#include "Modules/ModuleScene.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleFiles.h"
#include "Utils/Logging.h"

#include "rapidjson/error/en.h"

#define JSON_TAG_GAMEOBJECTS "GameObjects"
#define JSON_TAG_ROOT_ID "RootId"
#define JSON_TAG_QUADTREE_BOUNDS "QuadtreeBounds"
#define JSON_TAG_QUADTREE_MAX_DEPTH "QuadtreeMaxDepth"
#define JSON_TAG_QUADTREE_ELEMENTS_PER_NODE "QuadtreeElementsPerNode"
#define JSON_TAG_ID "Id"
#define JSON_TAG_NAME "Name"
#define JSON_TAG_PARENT_ID "ParentId"

void ResourceScene::BuildScene() {
	// Clear scene
	Scene* scene = App->scene->scene;
	scene->ClearScene();
	App->editor->selectedGameObject = nullptr;

	// Timer to measure loading a scene
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

		GameObject* gameObject = scene->gameObjects.Obtain();
		gameObject->scene = scene;
		gameObject->Load(jGameObject);
		gameObjects[i] = gameObject;
	}

	// Post-load
	scene->root = scene->GetGameObject(jScene[JSON_TAG_ROOT_ID]);
	for (unsigned i = 0; i < numGameObjects; ++i) {
		JsonValue jGameObject = jGameObjects[i];
		GameObject* gameObject = gameObjects[i];

		UID id = gameObject->GetID();
		std::string name = jGameObject[JSON_TAG_NAME];
		UID parentId = jGameObject[JSON_TAG_PARENT_ID];
		gameObject->id = id;
		gameObject->name = name;
		scene->gameObjectsIdMap[id] = gameObject;
		gameObject->SetParent(scene->GetGameObject(parentId));
	}

	// Init components
	for (unsigned i = 0; i < numGameObjects; ++i) {
		GameObject* gameObject = gameObjects[i];

		gameObject->InitComponents();
	}

	// Quadtree generation
	JsonValue jQuadtreeBounds = jScene[JSON_TAG_QUADTREE_BOUNDS];
	scene->quadtreeBounds = {{jQuadtreeBounds[0], jQuadtreeBounds[1]}, {jQuadtreeBounds[2], jQuadtreeBounds[3]}};
	scene->quadtreeMaxDepth = jScene[JSON_TAG_QUADTREE_MAX_DEPTH];
	scene->quadtreeElementsPerNode = jScene[JSON_TAG_QUADTREE_ELEMENTS_PER_NODE];
	scene->RebuildQuadtree();

	unsigned timeMs = timer.Stop();
	LOG("Scene built in %ums.", timeMs);
}
