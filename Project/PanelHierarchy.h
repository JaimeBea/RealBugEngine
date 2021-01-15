#pragma once

#include "Panel.h"
#include "UID.h"

class GameObject;

class PanelHierarchy : public Panel
{
public:
	PanelHierarchy();

	void Update() override;
	void UpdateHierarchyNode(GameObject* game_object);
	void SetSelectedObject(GameObject* game_object);

public:
	GameObject* selected_object = nullptr;

private:
	int window_width = 0;
	int window_height = 0;
};
