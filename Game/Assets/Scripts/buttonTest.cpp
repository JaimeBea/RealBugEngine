#include "buttonTest.h"

#include "Utils/Logging.h"
#include "GameObject.h"
#include "GameplaySystems.h"

GENERATE_BODY_IMPL(buttonTest);

void buttonTest::Start() {
	Button = GameplaySystems::GetGameObject("StartButton");
	LOG("Button test workind");
}

void buttonTest::Update() {
}

void buttonTest::OnButtonClick() {
	SceneManager::SceneLoad("Assets/Scenes/TestScene.scene");
}