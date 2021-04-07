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

	void SetSelected(UID newSelectableComponentID);
	void EnteredPointerOnSelectable(ComponentSelectable* newHoveredComponent);	//Interface implementation
	void ExitedPointerOnSelectable(ComponentSelectable* newUnHoveredComponent); //Interface implementation
	ComponentSelectable* GetCurrentSelected() const;							//Returns currently selected ComponentSelectable
	ComponentSelectable* GetCurrentlyHovered() const;							//Returns last Selectable that was hovered over with mouse

private:
	UID currentSelected = 0; //Currently selected SelectableComponent*

	//ESTO VA A DAR POR CULO 100%
	std::vector<UID> hoveredSelectables; //vector of SelectableComponents* it updates (adding/removing) with mouse events

public:
	UID firstSelected = 0; //Reference to the "first selected selectableComponent", this is not used directly but Unity implements it so that users can access it
};
