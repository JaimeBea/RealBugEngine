#pragma once
#include "Module.h"
#include "Resources/GameObject.h"

class ModuleUI : public Module {
public:
	bool Init() override;
	bool Start() override;
	UpdateStatus Update() override;
	bool CleanUp() override;

	void StartUI();
	void EndUI();
	GameObject* GetCanvas() const;

private:

	GameObject* canvas;
};
