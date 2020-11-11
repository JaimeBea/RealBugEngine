#include "Module.h"

#include "Globals.h"

bool Module::PreInit()
{
	return true;
}

bool Module::Init()
{
	return true;
}

bool Module::PostInit()
{
	return true;
}

UpdateStatus Module::PreUpdate()
{
	return UpdateStatus::CONTINUE;
}

UpdateStatus Module::Update()
{
	return UpdateStatus::CONTINUE;
}

UpdateStatus Module::PostUpdate()
{
	return UpdateStatus::CONTINUE;
}

bool Module::CleanUp()
{
	return true;
}
