#pragma once

#include "Modules/Module.h"
#include "Scene.h"
#include "GameObject.h"
#include "Utils/Logging.h"
#include "Utils/UID.h"
#include "Utils/Pool.h"
#include "Utils/Quadtree.h"
#include "Utils/VectorMap.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentMeshRenderer.h"
#include "Components/ComponentBoundingBox.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentLight.h"

#include <unordered_map>
#include <string>
#include <Components/ComponentSkyBox.h>

class CubeMap;
struct aiScene;
struct aiNode;

class ModuleScene : public Module {
public:
	// ------- Core Functions ------ //
	bool Init() override;
	bool Start() override;
	UpdateStatus Update() override;
	bool CleanUp() override;

	void CreateEmptyScene(); // Crates a new scene with a default game camera and directional light.

	void DestroyGameObjectDeferred(GameObject* gameObject); //Event dependant destruction, Gameobjects are destroyed upon the receival of an event, so that info is not null
	void ReceiveEvent(const Event& e) override;

public:
	std::string fileName = "";							   // REVIEW. This can be removed? Is it even used for anything?
	Scene* scene = nullptr;
};
