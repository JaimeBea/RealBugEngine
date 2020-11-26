#pragma once

class MSTimer
{
public:
	void Start();
	unsigned int Stop();
	unsigned int Read();

public:
	bool running = false;
	unsigned int start_time = 0;
	unsigned int delta_time = 0;
};

