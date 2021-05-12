#pragma once

#include "Scripting/Script.h"

class ComponentText;



class ScreenResolutionSetter : public Script {
	GENERATE_BODY(ScreenResolutionSetter);

public:

	void Start() override;
	void Update() override;
	void OnButtonClick() override;

public:
	bool increasing = false;
	UID textObjectID = 0;

private:
	void IncreaseResolution(int multiplier);
	void UpdateText();
	int GetPreSelectedWidth();
	int GetPreSelectedHeight();
private:

	ComponentText* text = nullptr;
};

