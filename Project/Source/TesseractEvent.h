#pragma once

#include <memory>

class GameObject;
class Component;
class Resource;
struct AssetFolder;

/* Creating a new event type:
*    1. Add a new EventType for the new event
*    2. Add a struct containing the necessary information for said event inside the union below
*    3. (If allocating) Make sure you release all allocated resources in ModuleEvents.cpp's CleanUpEvent()
*/

enum class TesseractEventType {
	UNKNOWN,
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
	MOUSE_RELEASED
};

struct TesseractEvent {
public:
	TesseractEvent(TesseractEventType type);

public:
	TesseractEventType type = TesseractEventType::UNKNOWN;

	union {
		struct DestroyGameObject {
			GameObject* gameObject = nullptr;
		} destroyGameObject;

		struct AddComponent {
			Component* component = nullptr;
		} addComponent;

		struct AddResource {
			Resource* resource = nullptr;
		} addResource;

		struct UpdateFolders {
			AssetFolder* folder = nullptr;
		} updateFolders;

		struct MouseUpdate {
			float mouseX = 0.0f;
			float mouseY = 0.0f;
		} mouseUpdate;

		struct MouseClicked {
			float mouseX = 0.0f;
			float mouseY = 0.0f;
		} mouseClicked;
	};
};
