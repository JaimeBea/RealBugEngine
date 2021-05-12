#include "ScreenResolutionSetter.h"

#include "GameplaySystems.h"
#include "Components/UI/ComponentText.h"
#include "GameObject.h"


int ScreenResolutionSetter::currentResolution = 2;
int ScreenResolutionSetter::preSelectedResolution = 2;
bool ScreenResolutionSetter::confirmationWasRequested = 2;

EXPOSE_MEMBERS(ScreenResolutionSetter) {

	MEMBER(MemberType::GAME_OBJECT_UID, textObjectID),
		MEMBER(MemberType::BOOL, increasing)
};

GENERATE_BODY_IMPL(ScreenResolutionSetter);

void ScreenResolutionSetter::Start() {
	GameObject* textObj = GameplaySystems::GetGameObject(textObjectID);

	if (textObj) {
		text = textObj->GetComponent<ComponentText>();
	}

	UpdateText();
	preSelectedResolution = currentResolution = 2;
}

void ScreenResolutionSetter::Update() {
	if (confirmationWasRequested) {
		if (currentResolution != preSelectedResolution) {
			currentResolution = preSelectedResolution;

			switch (currentResolution) {
			case RESOLUTION::m_1920x1080:
				Screen::SetResolution(1920, 1080);
				break;
			case RESOLUTION::m_1080x720:
				Screen::SetResolution(1080, 720);
				break;
			case RESOLUTION::m_720x480:
				Screen::SetResolution(720, 480);
				break;
			}

		}
		confirmationWasRequested = false;
	}
}

void ScreenResolutionSetter::OnButtonClick() {
	IncreaseResolution(increasing ? 1 : -1);
}

void ScreenResolutionSetter::IncreaseResolution(int multiplier) {

	std::string message = "Multiplier was " + std::to_string(multiplier) + ", current res was " + std::to_string((currentResolution));
	Debug::Log(message.c_str());

	preSelectedResolution = preSelectedResolution + multiplier;


	if (preSelectedResolution < 0) {
		preSelectedResolution = RESOLUTION::m_720x480;
	} else if (preSelectedResolution >= RESOLUTION::MAX) {
		preSelectedResolution = RESOLUTION::MAX - 1;
	}


	UpdateText();
}

void ScreenResolutionSetter::UpdateText() {
	if (!text) return;

	text->SetText(std::to_string(GetPreSelectedWidth()) + "x" + std::to_string(GetPreSelectedHeight()));
}

int ScreenResolutionSetter::GetPreSelectedWidth() {
	switch (preSelectedResolution) {
	case RESOLUTION::m_1920x1080:
		return 1920;
	case RESOLUTION::m_1080x720:
		return 1080;
	case RESOLUTION::m_720x480:
		return 720;
	}
	return 0;
}

int ScreenResolutionSetter::GetPreSelectedHeight() {
	switch (preSelectedResolution) {
	case RESOLUTION::m_1920x1080:
		return 1080;
	case RESOLUTION::m_1080x720:
		return 720;
	case RESOLUTION::m_720x480:
		return 480;
	}
	return 0;
}