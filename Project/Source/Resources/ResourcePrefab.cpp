#include "ResourcePrefab.h"

#include "Application.h"
#include "GameObject.h"
#include "Modules/ModuleScene.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleFiles.h"
#include "Utils/Logging.h"
#include "Utils/MSTimer.h"

#include "rapidjson/error/en.h"

#define JSON_TAG_ROOT "Root"
#define JSON_TAG_NAME "Name"
#define JSON_TAG_PARENT_INDEX "ParentIndex"

void ResourcePrefab::BuildPrefab(GameObject* parent) {
	// Timer to measure bulding a prefab
	MSTimer timer;
	timer.Start();
	std::string filePath = GetResourceFilePath();
	LOG("Building prefab from path: \"%s\".", filePath.c_str());

	// Read from file
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
	UID gameObjectId = GenerateUID();
	GameObject* gameObject = scene->gameObjects.Obtain(gameObjectId);
	gameObject->scene = scene;
	gameObject->LoadPrefab(jRoot, GetId());
	gameObject->id = gameObjectId;
	gameObject->SetParent(parent);

	unsigned timeMs = timer.Stop();
	LOG("Prefab built in %ums.", timeMs);
}

void ResourcePrefab::ReloadPrefab(GameObject* gameObject) {
	// Timer to measure loading a prefab
	MSTimer timer;
	timer.Start();
	std::string filePath = GetResourceFilePath();
	LOG("Building prefab from path: \"%s\".", filePath.c_str());

	// Read from file
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
	JsonValue jRoot = jScene[JSON_TAG_ROOT];
	gameObject->LoadPrefab(jRoot, GetId());

	unsigned timeMs = timer.Stop();
	LOG("Prefab loaded in %ums.", timeMs);
}

void ResourcePrefab::SavePrefab(GameObject* gameObject) {
	// Timer to measure loading a prefab
	MSTimer timer;
	timer.Start();
	std::string filePath = GetResourceFilePath();
	LOG("Building prefab from path: \"%s\".", filePath.c_str());

	// Read from file
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
	JsonValue jRoot = jScene[JSON_TAG_ROOT];
	gameObject->SavePrefab(jRoot, GetId());

	unsigned timeMs = timer.Stop();
	LOG("Prefab loaded in %ums.", timeMs);
}
