#pragma once

#include "Panel.h"
#include "Utils/UID.h"

class GameObject;

class PanelHierarchy : public Panel {
public:
	PanelHierarchy();

	void Update() override;

private:
	void UpdateHierarchyNode(GameObject* gameObject);
	GameObject* CreateEmptyGameObject(GameObject* gameObject);
	GameObject* CreateUICanvas(GameObject* gameObject);
	GameObject* CreateUIImage(GameObject* gameObject);
	GameObject* CreateUIText(GameObject* gameObject);
	GameObject* CreateUIButton(GameObject* gameObject);
	GameObject* CreateEventSystem(GameObject* gameObject);

private:
	int windowWidth = 0;
	int windowHeight = 0;

};
