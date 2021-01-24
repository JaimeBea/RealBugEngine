#pragma once

class Panel {
public:
	Panel(const char* name, bool enabled);

	virtual void Update();
	virtual void HelpMarker(const char* desc);

public:
	const char* name = "";
	bool enabled = true;
};
