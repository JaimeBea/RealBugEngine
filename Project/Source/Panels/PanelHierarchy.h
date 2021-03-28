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
	void CreateUICanvas(GameObject* gameObject);
	void CreateUIImage(GameObject* gameObject);
	void CreateUIText(GameObject* gameObject);
	void CreateUIButton(GameObject* gameObject);

private:
	int windowWidth = 0;
	int windowHeight = 0;

};
