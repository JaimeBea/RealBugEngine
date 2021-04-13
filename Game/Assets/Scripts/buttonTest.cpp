#include "buttonTest.h"

#include "Utils/Logging.h"
#include "GameObject.h"
#include "GameplaySystems.h"

GENERATE_BODY_IMPL(buttonTest);

void buttonTest::Start() {
	Button = GameplaySystems::GetGameObject("StartButton");
}

void buttonTest::Update() {
	ComponentTransform2D* transform2D = Button->GetComponent<ComponentTransform2D>();
	float aspectRatioImg = transform2D->GetSize().x / transform2D->GetSize().y;
	float newHeight = Screen::GetScreenHeight() * 0.2f;
	float newWidth = newHeight * aspectRatioImg;
	transform2D->SetSize(float2(newWidth, newHeight));
}

void buttonTest::OnButtonClick() {
	SceneManager::SceneLoad("Assets/Scenes/TestScene.scene");
	LOG("Click");
}