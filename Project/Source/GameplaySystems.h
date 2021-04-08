#pragma once

#if defined(TESSERACT_ENGINE_API)
/* do nothing. */
#elif defined(_MSC_VER)
#define TESSERACT_ENGINE_API __declspec(dllexport)
#endif

class GameObject;

namespace GameplaySystems {

	TESSERACT_ENGINE_API float GetDeltaTime();
	TESSERACT_ENGINE_API GameObject* GetGameObject(const char* name);
};
