#include "ModuleEvents.h"

#include "Utils/Logging.h"

#include "Utils/Leaks.h"

void ModuleEvents::AddEvent(const Event& newEvent) {
	eventQueue.push(newEvent);
}

Event ModuleEvents::PopEvent() {
	Event tmp(EventType::UNKNOWN);
	eventQueue.try_pop(tmp);
	return tmp;
}

void ModuleEvents::ProcessEvents() {
	while (!eventQueue.empty()) {
		ProcessEvent(PopEvent());
	}
}

void ModuleEvents::ProcessEvent(Event& e) {
	for (Module* m : observerMap[e.type]) {
		m->ReceiveEvent(e);
	}
}

bool ModuleEvents::Init() {
	return true;
}

UpdateStatus ModuleEvents::PreUpdate() {
	return UpdateStatus::CONTINUE;
}

UpdateStatus ModuleEvents::Update() {
	ProcessEvents();
	return UpdateStatus::CONTINUE;
}

UpdateStatus ModuleEvents::PostUpdate() {
	return UpdateStatus::CONTINUE;
}

void ModuleEvents::AddObserverToEvent(EventType type, Module* moduleToAdd) {
	observerMap[type].push_back(moduleToAdd);
}

void ModuleEvents::RemoveObserverFromEvent(EventType type, Module* moduleToRemove) {
	for (std::vector<Module*>::iterator it = observerMap[type].begin(); it != observerMap[type].end(); ++it) {
		if (*it == moduleToRemove) {
			observerMap[type].erase(it);
			return;
		}
	}
}

bool ModuleEvents::CleanUp() {
	observerMap.clear();
	while (!eventQueue.empty()) {
		Event anEvent(EventType::UNKNOWN);
		eventQueue.try_pop(anEvent);

		switch (anEvent.type) {
		case EventType::ADD_RESOURCE:
			RELEASE(anEvent.addResource.resource);
			break;
		case EventType::UPDATE_FOLDERS:
			RELEASE(anEvent.updateFolders.folder);
			break;
		}
	}
	return true;
}
