#pragma once

#include "Debug.h"
#include "ShortString.h"

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

enum screen_mode
{
	SM_WINDOWED,
	SM_BORDERLESS,
	SM_FULLSCREEN,
	SM_FULLSCREEN_DESKTOP
};

// Constants -----------
#define DEGTORAD 0.01745329251f

// Configuration -----------
#define GLSL_VERSION "#version 460"

extern ShortString<20> APP_NAME;
extern ShortString<20> ORGANIZATION_NAME;
extern int MAX_FPS;
extern bool LIMIT_FRAMERATE;
extern bool VSYNC;
extern int SCREEN_MODE;
extern bool RESIZABLE;
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern float BRIGHTNESS;
