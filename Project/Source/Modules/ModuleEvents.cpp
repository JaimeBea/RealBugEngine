#include "ModuleEvents.h"

#include "Utils/Logging.h"

#include "Utils/Leaks.h"

static void CleanUpEvent(TesseractEvent& e) {
	switch (e.type) {
	case TesseractEventType::ADD_COMPONENT:
		RELEASE(e.addComponent.component);
		break;
	case TesseractEventType::ADD_RESOURCE:
		RELEASE(e.addResource.resource);
		break;
	case TesseractEventType::UPDATE_FOLDERS:
		RELEASE(e.updateFolders.folder);
		break;
	}
}

bool ModuleEvents::Init() {
	observerArray = new std::vector<Module*>[(int) TesseractEventType::COUNT];
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

bool ModuleEvents::CleanUp() {
	delete[] observerArray;
	while (!eventQueue.empty()) {
		TesseractEvent e(TesseractEventType::UNKNOWN);
		eventQueue.try_pop(e);
		CleanUpEvent(e);
	}
	return true;
}

void ModuleEvents::AddObserverToEvent(TesseractEventType type, Module* moduleToAdd) {
	observerArray[(int) type].push_back(moduleToAdd);
}

void ModuleEvents::RemoveObserverFromEvent(TesseractEventType type, Module* moduleToRemove) {
	for (std::vector<Module*>::iterator it = observerArray[(int) type].begin(); it != observerArray[(int) type].end(); ++it) {
		if (*it == moduleToRemove) {
			observerArray[(int) type].erase(it);
			return;
		}
	}
}

void ModuleEvents::AddEvent(const TesseractEvent& newEvent) {
	eventQueue.push(newEvent);
}

void ModuleEvents::ProcessEvents() {
	while (!eventQueue.empty()) {
		TesseractEvent e(TesseractEventType::UNKNOWN);
		eventQueue.try_pop(e);
		ProcessEvent(e);
		CleanUpEvent(e);
	}
}

void ModuleEvents::ProcessEvent(TesseractEvent& e) {
	for (Module* m : observerArray[(int) e.type]) {
		m->ReceiveEvent(e);
	}
}
