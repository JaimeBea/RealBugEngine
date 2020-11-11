#pragma once

enum class UpdateStatus
{
	CONTINUE,
	STOP,
	ERROR
};

// Constants -----------
#define DEGTORAD 0.01745329251f

// Configuration -----------
#define GLSL_VERSION "#version 460"

// Delete helpers
#define RELEASE( x ) { if( x != nullptr ) { delete x; x = nullptr; } }
#define RELEASE_ARRAY( x ) { if( x != nullptr ) { delete[] x; x = nullptr; } }
