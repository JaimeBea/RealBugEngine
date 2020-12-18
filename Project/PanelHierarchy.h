#pragma once

#include "Panel.h"

class GameObject;

class PanelHierarchy : public Panel
{
public:
	PanelHierarchy();

	void Update() override;
	void UpdateHierarchyNode(GameObject* game_object);

public:
	GameObject* selected_object = nullptr;
	unsigned long long selected_id = -1;

private:
	int window_width = 0;
	int window_height = 0;


};


