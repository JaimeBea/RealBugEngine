#include "Module.h"

#include "Globals.h"
#include "Utils/Logging.h"
#include "Utils/Leaks.h"
#include "UI/EventSystem/Event.h"
Module::~Module() {}

bool Module::Init() {
	return true;
}

bool Module::Start() {
	return true;
}

UpdateStatus Module::PreUpdate() {
	return UpdateStatus::CONTINUE;
}

UpdateStatus Module::Update() {
	return UpdateStatus::CONTINUE;
}

UpdateStatus Module::PostUpdate() {
	return UpdateStatus::CONTINUE;
}

bool Module::CleanUp() {
	return true;
}

void Module::RecieveEvent(const Event& e) {
	switch (e.type) {
	case Event::EventType::GameObject_Destroyed:
		//To do iterate interested modules and send them the event
		LOG("GameObject %s was destroyed", e.objPtr.ptr->name.c_str());
		break;
	case Event::EventType::Pressed_Resume:
		LOG("Do sth with resume");
		break;
	}

	LOG("Received event of type %s", Event::TypeToString(e.type));
}
