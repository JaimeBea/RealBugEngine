#pragma once

enum class UpdateStatus;

class Module
{
public:
	virtual bool Init();
	virtual bool Start();
	virtual UpdateStatus PreUpdate();
	virtual UpdateStatus Update();
	virtual UpdateStatus PostUpdate();
	virtual bool CleanUp();
};
