#pragma once

class MSTimer {
public:
	void Start();
	unsigned int Stop();
	unsigned int Read();

public:
	bool running = false;
	unsigned int startTime = 0;
	unsigned int deltaTime = 0;
};
