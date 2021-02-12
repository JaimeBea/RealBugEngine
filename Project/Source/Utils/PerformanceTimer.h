#pragma once

class PerformanceTimer {
public:
	void Start();
	unsigned long long Stop();
	unsigned long long Read();

public:
	bool running = false;
	unsigned long long startCount = 0;
	unsigned long long deltaTime = 0;
};
