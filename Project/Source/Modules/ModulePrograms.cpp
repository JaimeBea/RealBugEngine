#include "ModulePrograms.h"

#include "Globals.h"
#include "Application.h"
#include "Utils/Logging.h"
#include "Modules/ModuleFiles.h"

#include "GL/glew.h"

#include "Utils/Leaks.h"

static unsigned CreateShader(unsigned type, const char* filePath) {
	LOG("Creating shader from file: \"%s\"...", filePath);

	Buffer<char> sourceBuffer = App->files->Load(filePath);
	char* source = sourceBuffer.Data();

	unsigned shaderId = glCreateShader(type);
	glShaderSource(shaderId, 1, &source, 0);

	glCompileShader(shaderId);

	int res = GL_FALSE;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &res);
	if (res == GL_FALSE) {
		int len = 0;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &len);
		if (len > 0) {
			int written = 0;
			Buffer<char> info = Buffer<char>(len);
			glGetShaderInfoLog(shaderId, len, &written, info.Data());
			LOG("Log Info: %s", info.Data());
		}
	}

	LOG("Shader created successfuly.");
	return shaderId;
}

static unsigned CreateProgram(const char* vertexShaderFilePath, const char* fragmentShaderFilePath) {
	LOG("Creating program...");

	// Compile the shaders and delete them at the end
	LOG("Compiling shaders...");
	unsigned vertexShader = CreateShader(GL_VERTEX_SHADER, vertexShaderFilePath);
	DEFER {
		glDeleteShader(vertexShader);
	};
	unsigned fragmentShader = CreateShader(GL_FRAGMENT_SHADER, fragmentShaderFilePath);
	DEFER {
		glDeleteShader(fragmentShader);
	};

	// Link the program
	LOG("Linking program...");
	unsigned programId = glCreateProgram();
	glAttachShader(programId, vertexShader);
	glAttachShader(programId, fragmentShader);
	glLinkProgram(programId);
	int res = GL_FALSE;
	glGetProgramiv(programId, GL_LINK_STATUS, &res);
	if (res == GL_FALSE) {
		int len = 0;
		glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &len);
		if (len > 0) {
			int written = 0;
			Buffer<char> info = Buffer<char>(len);
			glGetProgramInfoLog(programId, len, &written, info.Data());
			LOG("Program Log Info: %s", info.Data());
		}

		LOG("Error linking program.");
	} else {
		LOG("Program linked.");
	}

	return programId;
}

bool ModulePrograms::Start() {
	defaultProgram = CreateProgram("Shaders/default_vertex.glsl", "Shaders/default_fragment.glsl");
	phongPbrProgram = CreateProgram("Shaders/phong_pbr_vertex.glsl", "Shaders/phong_pbr_fragment.glsl");
	skyboxProgram = CreateProgram("Shaders/skybox_vertex.glsl", "Shaders/skybox_fragment.glsl");

	return true;
}

bool ModulePrograms::CleanUp() {
	glDeleteProgram(defaultProgram);
	glDeleteProgram(phongPbrProgram);
	glDeleteProgram(skyboxProgram);
	return true;
}
