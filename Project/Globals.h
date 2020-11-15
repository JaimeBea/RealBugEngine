#pragma once

// Enums -----------
enum class UpdateStatus
{
	CONTINUE,
	STOP,
	ERROR
};

enum class WindowMode
{
	WINDOWED,
	BORDERLESS,
	FULLSCREEN,
	FULLSCREEN_DESKTOP
};

enum class TextureFilter
{
	NEAREST,
	LINEAR,
	NEAREST_MIPMAP_NEAREST,
	LINEAR_MIPMAP_NEAREST,
	NEAREST_MIPMAP_LINEAR,
	LINEAR_MIPMAP_LINEAR
};

enum class TextureWrap
{
	REPEAT,
	CLAMP_TO_EDGE,
	CLAMP_TO_BORDER,
	MIRROR_REPEAT,
	MIRROR_CLAMP_TO_EDGE
};

// Constants -----------
#define DEGTORAD 0.01745329251f

// Configuration -----------
#define GLSL_VERSION "#version 460"

// Delete helpers
#define RELEASE( x ) { if( x != nullptr ) { delete x; x = nullptr; } }
#define RELEASE_ARRAY( x ) { if( x != nullptr ) { delete[] x; x = nullptr; } }

// Defer calls the given code at the end of the scope. Useful for freeing resources.
// Important: End of scope is not end of function. Be careful when using it inside loops.
// Usage: DEFER{ statements; };
#ifndef DEFER
struct defer_dummy {};
template <class F> struct deferrer { F f; ~deferrer() { f(); } };
template <class F> deferrer<F> operator*(defer_dummy, F f) { return { f }; }
#define DEFER__(LINE) zz_defer##LINE
#define DEFER_(LINE) DEFER__(LINE)
#define DEFER auto DEFER_(__LINE__) = defer_dummy{} *[&]()
#endif