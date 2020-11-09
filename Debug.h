#pragma once

#include <vector>

#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);
#define FPS_LOG_SIZE 100

void log(const char file[], int line, const char* format, ...);

void log_delta_ms(unsigned delta_ms);

extern std::vector<char*> log_lines;

extern int fps_log_index;
extern float fps_log[];
extern float ms_log[];
