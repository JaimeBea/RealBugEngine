#pragma once
#include <queue>
#include <Components/Component.h>

class GameObject;
class ComponentSelectable;
class IPointerEnterHandler;

class ComponentEventSystem : public Component {
public:
	REGISTER_COMPONENT(ComponentEventSystem, ComponentType::EVENT_SYSTEM, false);
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
	void EnteredPointerOnSelectable(ComponentSelectable* newH); //Interface implementation
	void ExitedPointerOnSelectable(ComponentSelectable* newH);	//Interface implementation
	ComponentSelectable* GetCurrentSelected() const;			//Returns currently selected ComponentSelectable
	ComponentSelectable* GetCurrentlyHovered() const;			//Returns last Selectable that was hovered over with mouse

private:
	ComponentSelectable* currentSelected = nullptr;		  //Currently selected SelectableComponent*
	std::vector<ComponentSelectable*> hoveredSelectables; //vector of SelectableComponents* it updates (adding/removing) with mouse events

public:
	static std::vector<ComponentSelectable*> m_Selectables; //Vector of all selectable components
	ComponentSelectable* firstSelected = nullptr;			//Reference to the "first selected selectableComponent", this is not used directly but Unity implements it so that users can access it
};
