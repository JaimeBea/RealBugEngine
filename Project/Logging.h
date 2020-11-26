#pragma once

#include <string>

#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

#define LOG(format, ...) log(__FILENAME__, __LINE__, format, __VA_ARGS__);
#define FPS_LOG_SIZE 100

void log(const char file[], int line, const char* format, ...);
void log_delta_ms(float delta_ms);

extern std::string log_string;
extern int fps_log_index;
extern float fps_log[];
extern float ms_log[];
