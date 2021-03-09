#include "ModuleEventSystem.h"
#include "UI/EventSystem/Event.h"
#include "Modules/ModuleUserInterface.h"
#include "Resources/GameObject.h"
#include "UI/Selectables/Selectable.h"
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
		(*it)->RecieveEvent(e);
	}
}

bool ModuleEventSystem::Init() {
	//	//GameObject Destroyed Event
	std::vector<Module*> gameObjectDestroyedMapVector;
	gameObjectDestroyedMapVector.push_back(App->scene);
	gameObjectDestroyedMapVector.push_back((Module*) App->renderer);
	observerMap[Event::EventType::GameObject_Destroyed] = gameObjectDestroyedMapVector;

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

bool ModuleEventSystem::CleanUp() {
	return true;
}