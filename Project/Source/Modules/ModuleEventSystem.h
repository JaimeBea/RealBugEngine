#pragma once
#include <queue>
#include <Modules/Module.h>
#include < vector>
#include "Resources/Event.h"


class ModuleUserInterface;
class ModuleScene;
class ModuleTime;
class GameObject;

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
	void AddObserverToEvent(Event::EventType type, Module* moduleToAdd);
	void RemoveObserverFromEvent(Event::EventType type, Module* moduletoRemove);
	void AddEvent(const Event& newEvent);
	bool CleanUp() override;
};
