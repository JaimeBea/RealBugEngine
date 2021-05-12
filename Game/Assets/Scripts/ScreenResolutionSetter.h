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
	bool increasing;
	UID textObjectID;
	static int currentResolution;
	static int preSelectedResolution;
	static bool confirmationWasRequested;

private:
	void IncreaseResolution(int multiplier);
	void UpdateText();
	int GetPreSelectedWidth();
	int GetPreSelectedHeight();
private:
	enum RESOLUTION {
		m_720x480 = 0,
		m_1080x720,
		m_1920x1080,
		MAX
	};
	ComponentText* text;
};

