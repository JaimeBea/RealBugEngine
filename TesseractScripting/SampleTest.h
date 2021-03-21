#pragma once

#ifdef TESSERACTSCRIPTING_EXPORTS
#define TESSERACTSCRIPTING_API __declspec(dllexport)
#else
#define TESSERACTSCRIPTING_API __declspec(dllimport)
#endif // TESSERACTSCRIPTING_EXPORTS


extern "C" TESSERACTSCRIPTING_API int Add(const int a, const int b);

extern "C" TESSERACTSCRIPTING_API int Mul(const int a, const int b);

extern "C" TESSERACTSCRIPTING_API void CreateSolution(const char* name);