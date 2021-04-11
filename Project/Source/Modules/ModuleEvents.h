#pragma once

#include "Event.h"
#include "Modules/Module.h"

#include <concurrent_queue.h>
#include <vector>
#include <unordered_map>

class ModuleEvents : public Module {
public:
	bool Init() override;
	UpdateStatus PreUpdate() override;
	UpdateStatus Update() override;
	UpdateStatus PostUpdate() override;

	void AddObserverToEvent(EventType type, Module* moduleToAdd); //All events should be added on the Init method for security pruposes,
																  //exceptions are those Modules that MUST handle some events on last instance,
																  //such as ModuleScene, which must handle GameObjectDestroyed the last so that
																  //there are no nullptrs

	void RemoveObserverFromEvent(EventType type, Module* moduletoRemove);
	void AddEvent(const Event& newEvent);
	bool CleanUp() override;

private:
	Event PopEvent();
	void ProcessEvents();
	void ProcessEvent(Event& e);

private:
	concurrency::concurrent_queue<Event> eventQueue;
	std::unordered_map<EventType, std::vector<Module*>> observerMap;
};