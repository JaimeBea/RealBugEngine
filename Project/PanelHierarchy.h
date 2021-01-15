#pragma once

#include "Panel.h"
#include "UID.h"

class GameObject;

class PanelHierarchy : public Panel
{
public:
	PanelHierarchy();

	void Update() override;

public:
	GameObject* selected_object = nullptr;

private:
	void UpdateHierarchyNode(GameObject* game_object);

private:
	int window_width = 0;
	int window_height = 0;
};
