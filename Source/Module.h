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

	virtual update_status PreUpdate()
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status Update()
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status PostUpdate()
	{
		return UPDATE_CONTINUE;
	}

	virtual bool CleanUp() 
	{ 
		return true; 
	}
};
