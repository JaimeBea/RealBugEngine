#pragma once

#ifdef TESSERACTSCRIPTING_EXPORTS
#define TESSERACTSCRIPTING_API __declspec(dllexport)
#else
#define TESSERACTSCRIPTING_API __declspec(dllimport)
#endif // TESSERACTSCRIPTING_EXPORTS

class Script {
public:
	TESSERACTSCRIPTING_API virtual void Start() {};
	TESSERACTSCRIPTING_API virtual void PreUpdate() {};
	TESSERACTSCRIPTING_API virtual void Update() {};
	TESSERACTSCRIPTING_API virtual void PostUpdate() {};

};