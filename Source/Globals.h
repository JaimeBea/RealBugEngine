#pragma once

#include "Debug.h"

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Constants -----------
#define DEGTORAD 0.01745329251f

// Configuration -----------
#define GLSL_VERSION "#version 460"
