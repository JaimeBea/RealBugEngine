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

	//GameObject Destroyed Event listeners
	std::vector<Module*> gameObjectDestroyedMapVector;
	gameObjectDestroyedMapVector.push_back(App->scene);
	gameObjectDestroyedMapVector.push_back((Module*) App->renderer);
	observerMap[Event::EventType::GameObject_Destroyed] = gameObjectDestroyedMapVector;

	std::vector<Module*> pressedPlayMapVector;
	pressedPlayMapVector.push_back((Module*) App->time);
	observerMap[Event::EventType::Pressed_Play] = pressedPlayMapVector;

	std::vector<Module*> pressedStopMapVector;
	pressedStopMapVector.push_back((Module*) App->time);
	observerMap[Event::EventType::Pressed_Stop] = pressedStopMapVector;

	std::vector<Module*> pressedPauseMapVector;
	pressedPauseMapVector.push_back((Module*) App->time);
	observerMap[Event::EventType::Pressed_Pause] = pressedPauseMapVector;

	std::vector<Module*> pressedStepMapVector;
	pressedStepMapVector.push_back((Module*) App->time);
	observerMap[Event::EventType::Pressed_Step] = pressedStepMapVector;

	std::vector<Module*> pressedResumeMapVector;
	pressedResumeMapVector.push_back((Module*) App->time);
	observerMap[Event::EventType::Pressed_Resume] = pressedResumeMapVector;

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

void ModuleEventSystem::SetSelected(Selectable* newSelected) {
	if (currentSelected != nullptr) {
		currentSelected->OnDeselect();
	}
	currentSelected = newSelected;
	if (newSelected != nullptr) {
		newSelected->OnSelect();
	}
}
