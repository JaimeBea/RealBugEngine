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
	unsigned shaderId = glCreateShader(type);
	if (shaderId == 0) {
		return 0;
	}
	std::string v = GLSL_VERSION "\n";
	std::string defineVertexShader = "#define VERTEX  \n";
	std::string defineFragmentShader = "#define FRAGMENT\n";

	std::string shaderDefine = (type == GL_VERTEX_SHADER) ? defineVertexShader : defineFragmentShader;

	GLchar const* shaderStrings[3] = {v.c_str(), shaderDefine.c_str(), sourceBuffer.Data()};
	GLint shaderStringLengths[3] = {v.size(), shaderDefine.size(), sourceBuffer.Size()};

	glShaderSource(shaderId, 3, shaderStrings, shaderStringLengths);
	glCompileShader(shaderId);

	int res = GL_FALSE;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &res);

	if (res == GL_FALSE) {
		int len = 0;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &len);

		if (len > 1) {
			int written = 0;
			Buffer<char> info = Buffer<char>(len);
			glGetShaderInfoLog(shaderId, len, &written, info.Data());
			LOG("Log Info: %s", info.Data());
		}
		return 0;
	}

	LOG("Shader created successfuly.");
	return shaderId;
}

unsigned ModulePrograms::CreateProgram(const char* ShaderFilePath) {
	LOG("Creating program...");

	// Compile the shaders and delete them at the end
	LOG("Compiling shaders...");
	unsigned vertexShader = CreateShader(GL_VERTEX_SHADER, ShaderFilePath);
	DEFER {
		glDeleteShader(vertexShader);
	};
	unsigned fragmentShader = CreateShader(GL_FRAGMENT_SHADER, ShaderFilePath);
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
	//ASAP TO DO, ESTABLISH UIPROGRAM FROM A FILE, THIS FILE MUST BE CONTAINED IN ASSETS FOLDER WITH THE OTHER SHADERS

	//defaultProgram = CreateProgram("Shaders/default_vertex.glsl", "Shaders/default_fragment.glsl");
	//phongPbrProgram = CreateProgram("Shaders/phong_pbr_vertex.glsl", "Shaders/phong_pbr_fragment.glsl");
	//skyboxProgram = CreateProgram("Shaders/skybox_vertex.glsl", "Shaders/skybox_fragment.glsl");
	//uiProgram = CreateProgram("Shaders/ui_vertex.glsl", "Shaders/ui_fragment.glsl");

	//uiProgram = App->
	
	return true;
}

void ModulePrograms::DeleteProgram(unsigned int IdProgram) {
	glDeleteProgram(IdProgram);
}

bool ModulePrograms::CleanUp() {
	return true;
}