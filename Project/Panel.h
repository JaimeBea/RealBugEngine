#pragma once

class Panel
{
public:
	Panel(const char* name, unsigned width, unsigned height, bool enabled = true);

	virtual void Start();
	virtual void Update();

public:
	const char* name = "";
	unsigned height = 0;
	unsigned width = 0;
	bool enabled = true;
};

