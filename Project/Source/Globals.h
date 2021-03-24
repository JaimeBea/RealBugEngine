#pragma once

// Enums -----------
enum class UpdateStatus {
	CONTINUE,
	STOP,
	ERROR
};

// Constants -----------
#define DEGTORAD 0.01745329251f
#define RADTODEG 1.0f / DEGTORAD

// Files -----------
#define ASSETS_PATH "Assets"
#define LIBRARY_PATH "Library"
#define TEXTURES_PATH "Assets/Textures"
#define SHADERS_PATH "Assets/Shaders"
#define SCENES_PATH "Assets/Scenes"
#define TEXTURE_EXTENSION ".dds"
#define MATERIAL_EXTENSION ".mat"
#define SCENE_EXTENSION ".scene"
#define META_EXTENSION ".meta"

#define PHONG_SHADER_FILE "phong.glsl"

// Configuration -----------
#define GLSL_VERSION "#version 460"

// Threads
#define TIME_BETWEEN_RESOURCE_UPDATES_MS 300

// Delete helpers -----------
#define RELEASE(x)          \
	{                       \
		if (x != nullptr) { \
			delete x;       \
			x = nullptr;    \
		}                   \
	}
#define RELEASE_ARRAY(x)    \
	{                       \
		if (x != nullptr) { \
			delete[] x;     \
			x = nullptr;    \
		}                   \
	}

// Defer -----------
// Defer calls the given code at the end of the scope. Useful for freeing resources.
// Important: End of scope is not end of function. Be careful when using it inside loops.
// Usage: DEFER{ statements; };
#ifndef DEFER
struct defer_dummy {};

template<class F>
struct deferrer {
	F f;
	~deferrer() {
		f();
	}
};

template<class F>
deferrer<F> operator*(defer_dummy, F f) {
	return {f};
}

#define DEFER__(LINE) zz_defer##LINE
#define DEFER_(LINE) DEFER__(LINE)
#define DEFER auto DEFER_(__LINE__) = defer_dummy {}* [&]()
#endif // DEFER
