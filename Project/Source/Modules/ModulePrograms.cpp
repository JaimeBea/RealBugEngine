#include "ModulePrograms.h"

#include "Globals.h"
#include "Application.h"
#include "Utils/Logging.h"
#include "Modules/ModuleFiles.h"

#include "GL/glew.h"

#include "Utils/Leaks.h"

static unsigned CreateShader(unsigned type, const char* file_name) {
	LOG("Creating shader from file: \"%s\"...", file_name);

	Buffer<char> source_buffer = App->files->Load(file_name);
	char* source = source_buffer.Data();

	unsigned shader_id = glCreateShader(type);
	glShaderSource(shader_id, 1, &source, 0);

	glCompileShader(shader_id);

	int res = GL_FALSE;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &res);
	if (res == GL_FALSE) {
		int len = 0;
		glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &len);
		if (len > 0) {
			int written = 0;
			Buffer<char> info = Buffer<char>(len);
			glGetShaderInfoLog(shader_id, len, &written, info.Data());
			LOG("Log Info: %s", info.Data());
		}
	}

	LOG("Shader created successfuly.");
	return shader_id;
}

static unsigned CreateProgram(const char* vertex_shader_file_name, const char* fragment_shader_file_name) {
	LOG("Creating program...");

	// Compile the shaders and delete them at the end
	LOG("Compiling shaders...");
	unsigned vertex_shader = CreateShader(GL_VERTEX_SHADER, vertex_shader_file_name);
	DEFER {
		glDeleteShader(vertex_shader);
	};
	unsigned fragment_shader = CreateShader(GL_FRAGMENT_SHADER, fragment_shader_file_name);
	DEFER {
		glDeleteShader(fragment_shader);
	};

	// Link the program
	LOG("Linking program...");
	unsigned program_id = glCreateProgram();
	glAttachShader(program_id, vertex_shader);
	glAttachShader(program_id, fragment_shader);
	glLinkProgram(program_id);
	int res = GL_FALSE;
	glGetProgramiv(program_id, GL_LINK_STATUS, &res);
	if (res == GL_FALSE) {
		int len = 0;
		glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &len);
		if (len > 0) {
			int written = 0;
			Buffer<char> info = Buffer<char>(len);
			glGetProgramInfoLog(program_id, len, &written, info.Data());
			LOG("Program Log Info: %s", info.Data());
		}

		LOG("Error linking program.");
	} else {
		LOG("Program linked.");
	}

	return program_id;
}

bool ModulePrograms::Start() {
	default_program = CreateProgram("Shaders/default_vertex.glsl", "Shaders/default_fragment.glsl");
	phong_pbr_program = CreateProgram("Shaders/phong_pbr_vertex.glsl", "Shaders/phong_pbr_fragment.glsl");
	skybox_program = CreateProgram("Shaders/skybox_vertex.glsl", "Shaders/skybox_fragment.glsl");

	return true;
}

bool ModulePrograms::CleanUp() {
	glDeleteProgram(default_program);
	glDeleteProgram(phong_pbr_program);
	glDeleteProgram(skybox_program);
	return true;
}
