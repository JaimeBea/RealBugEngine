#pragma once

#include "Debug.h"

enum class UpdateStatus
{
	CONTINUE = 1,
	STOP,
	ERROR
};

// Constants -----------
#define DEGTORAD 0.01745329251f

// Configuration -----------
#define GLSL_VERSION "#version 460"
