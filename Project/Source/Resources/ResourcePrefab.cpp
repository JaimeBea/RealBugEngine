#include "ResourcePrefab.h"

#include "Application.h"
#include "Modules/ModuleScene.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleFiles.h"
#include "Utils/Logging.h"

#include "rapidjson/error/en.h"

#define JSON_TAG_ROOT "Root"
#define JSON_TAG_NAME "Name"
#define JSON_TAG_PARENT_INDEX "ParentIndex"

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
	Scene* scene = parent->scene;
	JsonValue jRoot = jScene[JSON_TAG_ROOT];
	GameObject* gameObject = scene->gameObjects.Obtain();
	gameObject->scene = scene;
	gameObject->LoadPrototype(jRoot);
	gameObject->id = GenerateUID();
	scene->gameObjectsIdMap[gameObject->GetID()] = gameObject;
	gameObject->SetParent(parent);
	gameObject->InitComponents();

	unsigned timeMs = timer.Stop();
	LOG("Scene built in %ums.", timeMs);
}
