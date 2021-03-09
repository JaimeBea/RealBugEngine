#pragma once
#include <queue>
#include <Modules/Module.h>
#include "UI/EventSystem/Event.h"

class ModuleUserInterface;
class ModuleScene;
class GameObject;
class Selectable;

class ModuleEventSystem : public Module {
private:
	std::queue<Event> eventQueue;
	void ProcessEvent(Event& e);
	std::unordered_map<Event::EventType, std::vector<Module*>> observerMap;
	static Selectable* currentSelected;

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
	void AddEvent(const Event& newEvent);
	bool CleanUp() override;

public:
	std::vector<Selectable*> m_Selectables;
};
