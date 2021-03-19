#include "ModuleEventSystem.h"
#include "Resources/GameObject.h"
#include "Utils/Logging.h"
#include "Utils/Logging.h"
#include "Utils/Leaks.h"

ModuleEventSystem::ModuleEventSystem() {
}

ModuleEventSystem ::~ModuleEventSystem() {
}

void ModuleEventSystem::AddEvent(const Event& newEvent) {
	eventQueue.push(newEvent);
}

Event ModuleEventSystem::PopEvent() {
	Event tmp = eventQueue.front();
	eventQueue.pop();
	return tmp;
}

void ModuleEventSystem::ProcessEvents() {
	while (!eventQueue.empty()) {
		ProcessEvent(PopEvent());
	}
}

void ModuleEventSystem::ProcessEvent(Event& e) {
	for (std::vector<Module*>::iterator it = observerMap[e.type].begin(); it != observerMap[e.type].end(); ++it) {
		(*it)->ReceiveEvent(e);
	}
}

bool ModuleEventSystem::Init() {
	return true;
}

UpdateStatus ModuleEventSystem::PreUpdate() {
	return UpdateStatus::CONTINUE;
}

UpdateStatus ModuleEventSystem::Update() {
	ProcessEvents();
	return UpdateStatus::CONTINUE;
}

UpdateStatus ModuleEventSystem::PostUpdate() {
	return UpdateStatus::CONTINUE;
}

void ModuleEventSystem::AddObserverToEvent(Event::EventType type, Module* moduleToAdd) {
	observerMap[type].push_back(moduleToAdd);
}

void ModuleEventSystem::RemoveObserverFromEvent(Event::EventType type, Module* moduleToRemove) {
	for (std::vector<Module*>::iterator it = observerMap[type].begin(); it != observerMap[type].end(); ++it) {
		if (*it == moduleToRemove) {
			observerMap[type].erase(it);
			return;
		}
	}
}

bool ModuleEventSystem::CleanUp() {
	observerMap.clear();
	return true;
}
