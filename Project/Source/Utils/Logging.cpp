#include "Logging.h"

#include <sstream>
#include <windows.h>
#include <stdio.h>
#include <mutex>

#include "Leaks.h"

std::mutex logMessageQueueMutex;
std::queue<std::string> logMessageQueue;

void Log(const char file[], int line, const char* format, ...) {
	static char tmpString[4096];
	static char tmpString2[4096];
	static va_list ap;

	// Construct the string from variable arguments
	logMessageQueueMutex.lock();
	va_start(ap, format);
	vsprintf_s(tmpString, 4096, format, ap);
	va_end(ap);
	sprintf_s(tmpString2, 4096, "%s(%d) : %s\n", file, line, tmpString);
	OutputDebugString(tmpString2);
	logMessageQueue.push(tmpString2);
	logMessageQueueMutex.unlock();
}

void UpdateLogString() {
	logMessageQueueMutex.lock();
	while (!logMessageQueue.empty()) {
		logString->append(logMessageQueue.front());
		logMessageQueue.pop();
	}
	logMessageQueueMutex.unlock();
}

void LogDeltaMS(float deltaMs) {
	float fps = 1000.0f / deltaMs;
	fpsLogIndex -= 1;
	if (fpsLogIndex < 0) {
		fpsLogIndex = FPS_LOG_SIZE - 1;
	}
	fpsLog[fpsLogIndex] = fps;
	msLog[fpsLogIndex] = deltaMs;
}

std::string* logString = nullptr;
int fpsLogIndex = FPS_LOG_SIZE - 1;
float fpsLog[FPS_LOG_SIZE] = {0};
float msLog[FPS_LOG_SIZE] = {0};
