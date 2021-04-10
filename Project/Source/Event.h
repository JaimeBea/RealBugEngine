#pragma once

#include "GameObject.h"

class Resource;
struct AssetFolder;

enum class EventType {
	UNKNOWN,
	GAMEOBJECT_DESTROYED,
	PRESSED_PLAY,
	PRESSED_PAUSE,
	PRESSED_RESUME,
	PRESSED_STEP,
	PRESSED_STOP,
	ADD_RESOURCE,
	UPDATE_FOLDERS,
	MOUSE_UPDATE,
	MOUSE_CLICKED
};

//STEPS TO CREATE A NEW EVENT:
//1 - CREATE ENUM CLASS EVENTYPE
//2 - ESTABLISH SWITCH EXCEPTION FOR TYPETOSTRING
//3 - GENERATE A STRUCT CALLED EVENT+NAMEOFTHEEVENT CONTAINING THE NECESARY INFORMATION FOR SAID EVENT
//4 - ADD SAID STRUCT TO THE UNION
//5 - (OPTIONAL) IF THIS PARTICULAR EVENT USES A POINTER, MAKE SURE SAID POINTER IS RELEASED ON EVENTSMODULE CLEANUP (THERE'S A SWITCH)

struct Event {
public:
	struct EventDestroyGameObject {
		GameObject* ptr;
	};

	struct EventAddResource {
		Resource* resource;
	};

	struct EventUpdateFolders {
		AssetFolder* folder;
	};

	struct EventMouseUpdate {
		float mouseX;
		float mouseY;
	};

	struct EventMouseClicked {
		float mouseX;
		float mouseY;
	};

	static const char* TypeToString(EventType v) {
		switch (v) {
		case EventType::GAMEOBJECT_DESTROYED:
			return "GameObject destroyed";
		case EventType::PRESSED_PLAY:
			return "Pressed play";
		case EventType::PRESSED_PAUSE:
			return "Pressed pause";
		case EventType::PRESSED_RESUME:
			return "Pressed resume";
		case EventType::PRESSED_STEP:
			return "Pressed step";
		case EventType::PRESSED_STOP:
			return "Pressed stop";
		case EventType::MOUSE_UPDATE:
			return "Mouse moved";
		case EventType::MOUSE_CLICKED:
			return "Mouse clicked";
		case EventType::ADD_RESOURCE:
			return "Add Resource";
		case EventType::UPDATE_FOLDERS:
			return "Update folders";
		default:
			return "Unkown event type";
		}
	}

	Event(EventType type_)
		: type(type_) {
	}

	//Event()
	//	: type(EventType::UNKNOWN) {
	//}

	~Event() {
	}

public:
	EventType type;

	union {
		EventDestroyGameObject destroyGameObject;
		EventAddResource addResource;
		EventUpdateFolders updateFolders;
		EventMouseUpdate mouseUpdate;
		EventMouseClicked mouseClicked;
	};
};
