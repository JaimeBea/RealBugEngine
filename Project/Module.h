#pragma once

enum class UpdateStatus;

class Module
{
public:
	Module();

	virtual bool PreInit();
	virtual bool Init();
	virtual bool PostInit();
	virtual UpdateStatus PreUpdate();
	virtual UpdateStatus Update();
	virtual UpdateStatus PostUpdate();
	virtual bool CleanUp();
};
