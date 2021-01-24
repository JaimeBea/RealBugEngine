#pragma once

class PerformanceTimer {
public:
	void Start();
	unsigned long long Stop();
	unsigned long long Read();

public:
	bool running = false;
	unsigned long long start_count = 0;
	unsigned long long delta_time = 0;
};
