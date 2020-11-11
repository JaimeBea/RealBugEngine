#include "ModuleProgram.h"

#include "Globals.h"
#include "Logging.h"

#include "GL/glew.h"
#include <string.h>

static char* LoadShader(const char* file_name)
{
	FILE* file = fopen(file_name, "rb");
	if (!file)
	{
		LOG("Error opening file %s (%s).\n", file_name, strerror(errno));
		return nullptr;
	}
	DEFER{ fclose(file); };

	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	rewind(file);

	char* data = new char[size + 1];
	fread(data, 1, size, file);
	data[size] = '\0';

	return data;
}

static unsigned CreateShader(unsigned type, const char* file_name)
{
	char* source = LoadShader(file_name);
	DEFER{ RELEASE_ARRAY(source); };

	unsigned shader_id = glCreateShader(type);
	glShaderSource(shader_id, 1, &source, 0);

	glCompileShader(shader_id);

	int res = GL_FALSE;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &res);
	if (res == GL_FALSE)
	{
		int len = 0;
		glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &len);
		if (len > 0)
		{
			int written = 0;
			char* info = new char[len];
			DEFER{ RELEASE_ARRAY(info); };

			glGetShaderInfoLog(shader_id, len, &written, info);

			LOG("Log Info: %s", info);
		}
	}
	return shader_id;
}

static unsigned CreateProgram(const char* vertex_shader_file_name, const char* fragment_shader_file_name)
{
	// Compile the shaders and delete them at the end
	unsigned vertex_shader = CreateShader(GL_VERTEX_SHADER, vertex_shader_file_name);
	DEFER{ glDeleteShader(vertex_shader); };
	unsigned fragment_shader = CreateShader(GL_FRAGMENT_SHADER, fragment_shader_file_name);
	DEFER{ glDeleteShader(fragment_shader); };

	// Link the program
	unsigned program_id = glCreateProgram();
	glAttachShader(program_id, vertex_shader);
	glAttachShader(program_id, fragment_shader);
	glLinkProgram(program_id);
	int res = GL_FALSE;
	glGetProgramiv(program_id, GL_LINK_STATUS, &res);
	if (res == GL_FALSE)
	{
		int len = 0;
		glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &len);
		if (len > 0)
		{
			int written = 0;
			char* info = new char[len];
			DEFER{ RELEASE_ARRAY(info); };

			glGetProgramInfoLog(program_id, len, &written, info);

			LOG("Program Log Info: %s", info);
		}
	}

	return program_id;
}

ModuleProgram::ModuleProgram() {}

ModuleProgram::~ModuleProgram() {}

bool ModuleProgram::Init()
{
	program = CreateProgram("vertex.vert", "fragment.frag");

	return true;
}

bool ModuleProgram::CleanUp()
{
	glDeleteProgram(program);

	return true;
}
