#pragma once
#include "Resources/GameObject.h"
struct Event {
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

	enum EventType {
		GameObject_Destroyed,
		File_Dropped,
		Pressed_Play,
		Pressed_Pause,
		Pressed_Resume,
		Pressed_Step,
		Pressed_Stop,
	} type;

	static const char* TypeToString(EventType v) {
		switch (v) {
		case GameObject_Destroyed:
			return "GameObject destroyed";
		case File_Dropped:
			return "File dropped";
		case Pressed_Play:
			return "Pressed play";
		case Pressed_Pause:
			return "Pressed pause";
		case Pressed_Resume:
			return "Pressed resume";
		case Pressed_Step:
			return "Pressed step";
		case Pressed_Stop:
			return "Pressed stop";
		default:
			return "Unkown event type";
		}
	}

	Event(EventType aType)
		: type(aType) {
	}
	~Event() {
	}
};
