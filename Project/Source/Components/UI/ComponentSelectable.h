#pragma once
#include <Math/float2.h>
#include <vector>
#include <Components/Component.h>
#include "UI/Interfaces/IPointerEnterHandler.h"
#include "UI/Interfaces/IPointerExitHandler.h"
#include "UI/Interfaces/IMoveHandler.h"
class ComponentEventSystem;
class ComponentSelectable : public Component
	, IPointerEnterHandler
	, IPointerExitHandler {
public:
	REGISTER_COMPONENT(ComponentSelectable, ComponentType::SELECTABLE, false);

	//Selectable(GameObject* owner, UID componentID_, bool active = true);
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
		COLOR_CHANGE,
		ANIMATION,
		NONE
	};

protected:
	bool interactable = true;
	bool highlighted = false;
	bool selected = false;
	bool hovered = false;
	int selectableIndex = -1;
	NavigationType m_NavigationType = NavigationType::AUTOMATIC;
	TransitionType m_Transition = TransitionType::NONE;

};
