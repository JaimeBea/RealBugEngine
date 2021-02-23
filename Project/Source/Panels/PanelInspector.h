#pragma once

#include "Panel.h"

class Component;

class PanelInspector : public Panel {
public:
	PanelInspector();

	void Update() override;

	Component* GetCompToDelete();
	void SetCompToDelete(Component* comp);

private:
	Component* compToDelete = nullptr; // Setted in PanelInspector, when using the button "Delete" on the Component Options menu. If not nullptr, the PostUpdate() will delete the component it points to.
};
