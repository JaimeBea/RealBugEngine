#pragma once
#include <Math/float2.h>
#include <vector>
#include <Components/Component.h>
#include "UI/Interfaces/IPointerEnterHandler.h"
#include "UI/Interfaces/IPointerExitHandler.h"

#include "Math/float4.h"

class ComponentEventSystem;

#define JSON_TAG_COLOR_HOVERED "ColorHover"
#define JSON_TAG_COLOR_DISABLED "ColorDisable"
#define JSON_TAG_COLOR_SELECTED "ColorDisable"

#define JSON_TAG_INTERACTABLE "Interactable"
#define JSON_TAG_ON_AXIS_DOWN "OnAxisDown"
#define JSON_TAG_ON_AXIS_UP "OnAxisUp"
#define JSON_TAG_ON_AXIS_RIGHT "OnAxisRight"
#define JSON_TAG_ON_AXIS_LEFT "OnAxisLeft"
#define JSON_TAG_TRANSITION_TYPE "TransitionType"
#define JSON_TAG_NAVIGATION_TYPE "NavigationType"

class ComponentSelectable : public Component
	, IPointerEnterHandler
	, IPointerExitHandler {
public:
	REGISTER_COMPONENT(ComponentSelectable, ComponentType::SELECTABLE, false);

	~ComponentSelectable();
	bool IsInteractable() const;
	void SetInteractable(bool b);
	ComponentSelectable* FindSelectableOnDir(float2 dir);

	virtual void OnSelect();
	virtual void OnDeselect();

	void Init() override;
	void Update() override;
	void OnEditorUpdate() override;
	void Enable() override;
	void Disable() override;
	bool IsHovered() const;
	bool IsSelected() const;
	void Save(JsonValue jsonVal) const override;
	void Load(JsonValue jsonVal) override;

	// Heredado vía IPointerEnterHandler
	virtual void OnPointerEnter() override;
	virtual void OnPointerExit() override;

	void DuplicateComponent(GameObject& owner);

	void Highlight(bool b);

	Component* GetSelectableComponent();
	void SetSelectableType(ComponentType type_);
	const float4 GetDisabledColor() const; // Returns colorDisabled
	const float4 GetHoverColor() const;	   // Returns colorHovered
	const float4 GetSelectedColor() const; // Returns colorSelected
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

	UID onAxisUp = 0;
	UID onAxisDown = 0;
	UID onAxisLeft = 0;
	UID onAxisRight = 0;

	ComponentType selectableType = ComponentType::UNKNOWN;

protected:
	bool interactable = true;								   //Can this ComponentSelectable be interacted
	bool highlighted = false;								   //Is this ComponentSelectable highlighted
	bool selected = false;									   //Is this ComponentSelectable selected
	bool hovered = false;									   //Is this ComponentSelectable hovered by mouse
	NavigationType navigationType = NavigationType::AUTOMATIC; //Navigation can be user-explicit, automatic based on 2D axis, or non-existing
	TransitionType transitionType = TransitionType::NONE;	   //Transition for selected/hovered can be managed in different ways
	float4 colorDisabled = float4(0.73f, 0.73f, 0.73f, 1.f);   // The color when the button is disabled
	float4 colorHovered = float4(0.84f, 0.84f, 0.84f, 1.f);	   // The color when the button is hovered
	float4 colorSelected = float4(0.5f, 0.5f, 0.5f, 1.f);	   // The color when the button is hovered
};
