#pragma once
#include <queue>
#include <Components/Component.h>

class GameObject;
class ComponentSelectable;

class ComponentEventSystem : public Component {
private:
	ComponentSelectable* currentSelected;

public:
	static ComponentEventSystem* currentEvSys;
	std::vector<ComponentSelectable*> m_Selectables;
	ComponentSelectable* firstSelected;

public:
	REGISTER_COMPONENT(ComponentEventSystem, ComponentType::EVENT_SYSTEM, false);

	//ComponentEventSystem(GameObject* owner, UID componentID_, bool active);
	~ComponentEventSystem();

	void Init() override;
	void Update() override;
	void OnTransformUpdate() override;
	void OnEditorUpdate() override;
	void Save(JsonValue jComponent) const override;
	void Load(JsonValue jComponent) override;
	void Enable() override;
	void Disable() override;
	void DuplicateComponent(GameObject& owner) override;

	void SetSelected(ComponentSelectable* newSelected);
};
