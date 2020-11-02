#include "ModuleProgram.h"
#include "GL/glew.h"

static char* ReadFile(const char* file_name)
{
	char* data = nullptr;
	FILE* file = fopen(file_name, "rb");
	if (file)
	{
		fseek(file, 0, SEEK_END);
		long size = ftell(file);
		rewind(file);

		data = (char*)malloc(size + 1);
		fread(data, 1, size, file);
		data[size] = '\0';
		fclose(file);
	}
	else
	{
		LOG("Error opening file %s (%s).\n", file_name, std::strerror(errno));
	}
	return data;
}

static unsigned CreateShader(unsigned type, const char* file_name)
{
	char* source = ReadFile(file_name);
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
			char* info = (char*)malloc(len);
			glGetShaderInfoLog(shader_id, len, &written, info);
			LOG("Log Info: %s", info);
			free(info);
		}
	}
	free(source);
	return shader_id;
}

ModuleProgram::ModuleProgram()
{
	programs.reserve(1);
}

ModuleProgram::~ModuleProgram()
{
	CleanUp();
}

unsigned ModuleProgram::CreateProgram(const char* vertex_shader_file_name, const char* fragment_shader_file_name)
{
	// Compile the shaders
	unsigned vertex_shader = CreateShader(GL_VERTEX_SHADER, vertex_shader_file_name);
	unsigned fragment_shader = CreateShader(GL_FRAGMENT_SHADER, fragment_shader_file_name);

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
			char* info = (char*)malloc(len);
			glGetProgramInfoLog(program_id, len, &written, info);
			LOG("Program Log Info: %s", info);
			free(info);
		}
	}

	// Release the individual shaders
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	programs.push_back(program_id);
	return program_id;
}

bool ModuleProgram::CleanUp()
{
	for (unsigned program : programs)
	{
		glDeleteProgram(program);
	}
	programs.clear();
	return true;
}
