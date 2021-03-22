#pragma once
#include <queue>
#include <Components/Component.h>

class GameObject;
class Selectable;

class ComponentEventSystem : public Component {
private:
	Selectable* currentSelected;

public:
	static ComponentEventSystem* currentEvSys;
	std::vector<Selectable*> m_Selectables;
	Selectable* firstSelected;

public:
	ComponentEventSystem(GameObject& owner, bool active);
	~ComponentEventSystem();

	void Init() override;
	void Update() override;
	void OnTransformUpdate() override;
	void OnEditorUpdate() override;
	void Save(JsonValue jComponent) const override;
	void Load(JsonValue jComponent) override;
	void Enable() override;
	void Disable() override;

	void SetSelected(Selectable* newSelected);
};
