#pragma once
#include "Module.h"
#include "Resources/GameObject.h"

class ModuleUI : public Module {
public:
	bool Init() override;
	bool Start() override;
	void Render();
	bool CleanUp() override;

	void StartUI();
	void EndUI();
	GameObject* GetCanvas() const;

public:

	GameObject* canvas = nullptr;
};
