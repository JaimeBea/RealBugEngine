#pragma once

#include "Panel.h"

class GameObject;

class PanelHierarchy : public Panel
{
public:
	PanelHierarchy();

	void Update() override;
	GameObject* selected_object = nullptr;

private:
	int window_width = 0;
	int window_height = 0;


};


