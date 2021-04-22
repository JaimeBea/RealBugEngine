#pragma once
#include <memory>
#include <string>
#include <variant>

#include "Math/float2.h"

class GameObject;
class Component;
class Resource;

struct AssetFolder;

#define EventVariant std::variant<DestroyGameObjectStruct, AddComponentStruct, AddResourceStruct, UpdateFoldersStruct, MouseUpdateStruct, MouseClickedStruct, ChangeSceneStruct>

/* Creating a new event type:
*    1. Add a new EventType for the new event
*    2. Add a struct containing the necessary information for said event inside the union below
*    3. (If allocating) Make sure you release all allocated resources in ModuleEvents.cpp's CleanUpEvent()
*/

enum class TesseractEventType {
	UNKNOWN = 0,
	GAMEOBJECT_DESTROYED,
	ADD_COMPONENT,
	PRESSED_PLAY,
	PRESSED_PAUSE,
	PRESSED_RESUME,
	PRESSED_STEP,
	PRESSED_STOP,
	ADD_RESOURCE,
	UPDATE_FOLDERS,
	MOUSE_UPDATE,
	MOUSE_CLICKED,
	MOUSE_RELEASED,
	CHANGE_SCENE,
	RESOURCES_LOADED,
	COUNT = 15
};

struct AddResourceStruct {
	Resource* resource = nullptr;
};

struct DestroyGameObjectStruct {
	GameObject* gameObject = nullptr;
};

struct AddComponentStruct {
	Component* component = nullptr;
};

struct UpdateFoldersStruct {
	AssetFolder* folder = nullptr;
};

struct MouseUpdateStruct {
	//float mouseX = 0.0f;
	//float mouseY = 0.0f;

	float2 mousePos = float2::zero;
};

struct MouseClickedStruct {
	//float mouseX = 0.0f;
	//float mouseY = 0.0f;

	float2 mousePos = float2::zero;
};

struct ChangeSceneStruct {
	const char* scenePath = nullptr;
};

struct TesseractEvent {
public:
	TesseractEvent(TesseractEventType type);

public:
	TesseractEventType type = TesseractEventType::UNKNOWN;

	EventVariant variant;

};
