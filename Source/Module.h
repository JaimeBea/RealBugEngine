#pragma once

#include "Globals.h"

class Module
{
public:
	Module() {}

	virtual bool PreInit()
	{
		return true;
	}

	virtual bool Init() 
	{
		return true; 
	}

	virtual bool PostInit()
	{
		return true;
	}

	virtual UpdateStatus PreUpdate()
	{
		return UpdateStatus::CONTINUE;
	}

	virtual UpdateStatus Update()
	{
		return UpdateStatus::CONTINUE;
	}

	virtual UpdateStatus PostUpdate()
	{
		return UpdateStatus::CONTINUE;
	}

	virtual bool CleanUp() 
	{ 
		return true; 
	}
};
