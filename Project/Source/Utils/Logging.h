#pragma once

#include <string>

#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

#define LOG(format, ...) Log(__FILENAME__, __LINE__, format, __VA_ARGS__);
#define FPS_LOG_SIZE 100

void Log(const char file[], int line, const char* format, ...);
void UpdateLogString();

void LogDeltaMS(float deltaMs);

extern std::string* logString;
extern int fpsLogIndex;
extern float fpsLog[];
extern float msLog[];
