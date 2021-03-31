#pragma once
#include "ComponentSelectable.h"
#include "UI/Interfaces/IMouseClickHandler.h"
class GameObject;
class ComponentButton : public ComponentSelectable //Class that allows user interaction by clicking/pressing
	, public IMouseClickHandler {
public:
	REGISTER_COMPONENT_SELECTABLE(ComponentButton, ComponentType::BUTTON, false);
	void Init() override;

	void OnClicked() override;
	void Update() override;
	void DuplicateComponent(GameObject& obj) override;
	void OnPointerEnter() override;
	void OnPointerExit() override;

	bool GetClicked() const;
	void SetClicked(bool b);

private:
	bool clicked = false;
};
