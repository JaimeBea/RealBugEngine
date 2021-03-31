#pragma once
#include <Math/float2.h>
#include <vector>
#include <Components/Component.h>
#include "UI/Interfaces/IPointerEnterHandler.h"
#include "UI/Interfaces/IPointerExitHandler.h"
class ComponentEventSystem;
class ComponentSelectable : public Component
	, IPointerEnterHandler
	, IPointerExitHandler {
public:
	//REGISTER_COMPONENT(ComponentSelectable, ComponentType::SELECTABLE, false);
	ComponentSelectable(ComponentType type_, GameObject* owner, UID componentID_, bool active = true);
	~ComponentSelectable();
	bool GetInteractable();
	void SetInteractable(bool b);
	ComponentSelectable* FindSelectableOnDir(float2 dir);

	virtual void OnSelect();
	virtual void OnDeselect();
	ComponentSelectable* onAxisUp;
	ComponentSelectable* onAxisDown;
	ComponentSelectable* onAxisLeft;
	ComponentSelectable* onAxisRight;

	void Init() override;
	void Update() override;
	void OnEditorUpdate() override;
	void Enable() override;
	void Disable() override;
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

protected:
	bool interactable = true; //Can this ComponentSelectable be interacted
	bool highlighted = false; //Is this ComponentSelectable highlighted
	bool selected = false;	  //Is this ComponentSelectable selected
	bool hovered = false;	  //Is this ComponentSelectable hovered by mouse
	int selectableIndex = -1;
	NavigationType m_NavigationType = NavigationType::AUTOMATIC; //Navigation can be user-explicit, automatic based on 2D axis, or non-existing
	TransitionType m_Transition = TransitionType::NONE;			 //Transition for selected/hovered can be managed in different ways
};
