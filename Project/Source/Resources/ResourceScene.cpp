#include "ResourceScene.h"

#include "Application.h"
#include "Modules/ModuleScene.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleFiles.h"
#include "Utils/Logging.h"

#include "rapidjson/error/en.h"

#define JSON_TAG_ROOT "Root"
#define JSON_TAG_QUADTREE_BOUNDS "QuadtreeBounds"
#define JSON_TAG_QUADTREE_MAX_DEPTH "QuadtreeMaxDepth"
#define JSON_TAG_QUADTREE_ELEMENTS_PER_NODE "QuadtreeElementsPerNode"

void ResourceScene::BuildScene() {
	// Clear scene
	Scene* scene = App->scene->scene;
	scene->ClearScene();
	App->editor->selectedGameObject = nullptr;

	// Timer to measure loading a scene
	MSTimer timer;
	timer.Start();
	std::string filePath = GetResourceFilePath();
	LOG("Building scene from path: \"%s\".", filePath.c_str());

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
	GameObject* root = scene->gameObjects.Obtain();
	scene->root = root;
	root->scene = scene;
	root->Load(jRoot);
	scene->gameObjectsIdMap[root->GetID()] = root;
	root->InitComponents();

	// Quadtree generation
	JsonValue jQuadtreeBounds = jScene[JSON_TAG_QUADTREE_BOUNDS];
	scene->quadtreeBounds = {{jQuadtreeBounds[0], jQuadtreeBounds[1]}, {jQuadtreeBounds[2], jQuadtreeBounds[3]}};
	scene->quadtreeMaxDepth = jScene[JSON_TAG_QUADTREE_MAX_DEPTH];
	scene->quadtreeElementsPerNode = jScene[JSON_TAG_QUADTREE_ELEMENTS_PER_NODE];
	scene->RebuildQuadtree();

	unsigned timeMs = timer.Stop();
	LOG("Scene built in %ums.", timeMs);
}
