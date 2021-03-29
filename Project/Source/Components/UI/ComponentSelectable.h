#pragma once
#include <Math/float2.h>
#include <vector>
#include <Components/Component.h>
#include "UI/Interfaces/IPointerEnterHandler.h"
#include "UI/Interfaces/IPointerExitHandler.h"
#include "UI/Interfaces/IMoveHandler.h"
class ComponentEventSystem;

class ComponentSelectable : IPointerEnterHandler
	, IPointerExitHandler {
public:
	//REGISTER_COMPONENT(ComponentSelectable, ComponentType::SELECTABLE, false);

	//Selectable(GameObject* owner, UID componentID_, bool active = true);
	~ComponentSelectable();
	bool GetInteractable();
	void SetInteractable(bool b);
	ComponentSelectable* FindSelectableOnDir(float2 dir);
	GameObject* GetSelectableOwner();

	virtual void OnSelect();
	virtual void OnDeselect();
	ComponentSelectable* onAxisUp;
	ComponentSelectable* onAxisDown;
	ComponentSelectable* onAxisLeft;
	ComponentSelectable* onAxisRight;
	void SelectableInit(GameObject* obj);
	//void Init() override;
	//void Update() override;
	//void OnEditorUpdate() override;
	void Enable();
	void Disable();
	bool IsHovered() const;
	// Heredado vía IPointerEnterHandler
	virtual void OnPointerEnter() override;
	virtual void OnPointerExit() override;
	void Highlight(bool b);

public:
	enum class NavigationType {
		NONE,
		AUTOMATIC,
		MANUAL
	};

	//TO DO
	enum class TransitionType {
		NONE,
		COLOR_CHANGE,
		ANIMATION,

	};

private:
	GameObject* selectableOwner;

protected:
	bool interactable = true;
	bool highlighted = false;
	bool selected = false;
	bool hovered = false;
	int selectableIndex = -1;
	NavigationType m_NavigationType = NavigationType::AUTOMATIC;
	TransitionType m_Transition = TransitionType::NONE;
};
