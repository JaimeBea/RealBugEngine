#include "CanvasImage.h"

#include "Utils/Logging.h"
#include "GameObject.h"
#include "GameplaySystems.h"

GENERATE_BODY_IMPL(CanvasImage);

void CanvasImage::Start() {
	gameObject = GameplaySystems::GetGameObject("Background");
}

void CanvasImage::Update() {
}