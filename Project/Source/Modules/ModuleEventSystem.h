#pragma once

#include "Event.h"
#include "Modules/Module.h"

#include <queue>
#include <vector>
#include <unordered_map>

class ModuleEventSystem : public Module {
private:
	std::queue<Event> eventQueue;
	void ProcessEvent(Event& e);
	std::unordered_map<Event::EventType, std::vector<Module*>> observerMap;

private:
	Event PopEvent();
	void ProcessEvents();

public:
	ModuleEventSystem();
	~ModuleEventSystem();

	bool Init() override;
	UpdateStatus PreUpdate() override;
	UpdateStatus Update() override;
	UpdateStatus PostUpdate() override;

	void AddObserverToEvent(Event::EventType type, Module* moduleToAdd); //All events should be added on the Init method for security pruposes,
																		 //exceptions are those Modules that MUST handle some events on last instance,
																		 //such as ModuleScene, which must handle GameObjectDestroyed the last so that
																		 //there are no nullptrs

	void RemoveObserverFromEvent(Event::EventType type, Module* moduletoRemove);
	void AddEvent(const Event& newEvent);
	bool CleanUp() override;
};