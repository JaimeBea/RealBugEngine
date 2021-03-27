#pragma once

#include "GameObject.h"

struct Event {
public:
	enum class EventType {
		GAMEOBJECT_DESTROYED,
		PRESSED_PLAY,
		PRESSED_PAUSE,
		PRESSED_RESUME,
		PRESSED_STEP,
		PRESSED_STOP,
		FILE_DROPPED

	} type;

	static const char* TypeToString(EventType v) {
		switch (v) {
		case EventType::GAMEOBJECT_DESTROYED:
			return "GameObject destroyed";
		case EventType::FILE_DROPPED:
			return "File dropped";
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
		default:
			return "Unkown event type";
		}
	}

	Event(EventType aType)
		: type(aType) {
		point2d.x = point2d.y = 0;
		objPtr.ptr = nullptr;
	}

	Event(EventType aType, float2 aFloat2)
		: type(aType) {
		point2d.x = aFloat2.x;
		point2d.y = aFloat2.y;
	}

	Event(EventType aType, GameObject* obj)
		: type(aType) {
		objPtr.ptr = obj;
	}

	~Event() {
	}

public:
	union {
		struct {
			GameObject* ptr;
		} objPtr;
		struct
		{
			int x, y;
		} point2d;
	};
};