#include "ModuleEventSystem.h"
#include "UI/EventSystem/Event.h"
#include "Modules/ModuleUserInterface.h"
#include "Resources/GameObject.h"
#include "UI/Selectables/Selectable.h"
#include "Utils/Logging.h"
#include "Utils/Leaks.h"

ModuleEventSystem::ModuleEventSystem()
	: currentSelected(nullptr)
	, firstSelected(nullptr) {
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
		(*it)->RecieveEvent(e);
	}
}

bool ModuleEventSystem::Init() {
	SetSelected(firstSelected);
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
	return true;
}

void ModuleEventSystem::SetSelected(Selectable* newSelected) {
	if (currentSelected != nullptr) {
		currentSelected->OnDeselect();
	}
	currentSelected = newSelected;
	if (newSelected != nullptr) {
		newSelected->OnSelect();
	}
}
