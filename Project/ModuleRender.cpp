#include "ModuleRender.h"

#include "Globals.h"
#include "Application.h"
#include "Logging.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "ModuleDebugDraw.h"
#include "ModuleResources.h"
#include "ModuleScene.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "ComponentMaterial.h"

#include "GL/glew.h"
#include "SDL.h"
#include "Brofiler.h"

#include "Leaks.h"

static void __stdcall OurOpenGLErrorFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	const char *tmp_source = "", *tmp_type = "", *tmp_severity = "";
	switch (source)
	{
	case GL_DEBUG_SOURCE_API:
		tmp_source = "API";
		break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		tmp_source = "Window System";
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		tmp_source = "Shader Compiler";
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		tmp_source = "Third Party";
		break;
	case GL_DEBUG_SOURCE_APPLICATION:
		tmp_source = "Application";
		break;
	case GL_DEBUG_SOURCE_OTHER:
		tmp_source = "Other";
		break;
	};
	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:
		tmp_type = "Error";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		tmp_type = "Deprecated Behaviour";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		tmp_type = "Undefined Behaviour";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		tmp_type = "Portability";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		tmp_type = "Performance";
		break;
	case GL_DEBUG_TYPE_MARKER:
		tmp_type = "Marker";
		break;
	case GL_DEBUG_TYPE_PUSH_GROUP:
		tmp_type = "Push Group";
		break;
	case GL_DEBUG_TYPE_POP_GROUP:
		tmp_type = "Pop Group";
		break;
	case GL_DEBUG_TYPE_OTHER:
		tmp_type = "Other";
		break;
	};
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:
		tmp_severity = "high";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		tmp_severity = "medium";
		break;
	case GL_DEBUG_SEVERITY_LOW:
		tmp_severity = "low";
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		tmp_severity = "notification";
		break;
	};

	if (severity != GL_DEBUG_SEVERITY_HIGH)
	{
		return;
	}

	LOG("<Source:%s> <Type:%s> <Severity:%s> <ID:%d> <Message:%s>", tmp_source, tmp_type, tmp_severity, id, message);
}

bool ModuleRender::Init()
{
	LOG("Creating Renderer context");

	context = SDL_GL_CreateContext(App->window->window);

	GLenum err = glewInit();
	LOG("Using Glew %s", glewGetString(GLEW_VERSION));

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

#ifdef _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(&OurOpenGLErrorFunction, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true);
#endif

	glGenFramebuffers(1, &framebuffer);
	glGenRenderbuffers(1, &depth_renderbuffer);
	glGenTextures(1, &render_texture);

	ViewportResized(200, 200);

	return true;
}

UpdateStatus ModuleRender::PreUpdate()
{
	BROFILER_CATEGORY("ModuleRender - PreUpdate", Profiler::Color::Green)

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glViewport(0, 0, viewport_width, viewport_height);

	glClearColor(clear_color.x, clear_color.y, clear_color.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return UpdateStatus::CONTINUE;
}

UpdateStatus ModuleRender::Update()
{
	BROFILER_CATEGORY("ModuleRender - Update", Profiler::Color::Green)

	App->debug_draw->Draw(App->camera->GetViewMatrix(), App->camera->GetProjectionMatrix(), viewport_width, viewport_height);

	// Load model or texture if one gets dropped
	const char* dropped_file_name = App->input->GetDroppedFileName();
	if (dropped_file_name != nullptr)
	{
		bool loaded_scene = App->scene->Import(dropped_file_name);
		if (!loaded_scene)
		{
			/* TODO: Load textures
			unsigned loaded_texture = App->textures->LoadTexture(dropped_file_name);
			if (loaded_texture)
			{
				for (unsigned material : current_model->materials)
				{
					App->textures->ReleaseTexture(material);
				}
				current_model->materials.clear();
				current_model->materials.push_back(loaded_texture);
				for (Mesh& mesh : current_model->meshes)
				{
					mesh.material_index = 0;
				}
			}
			*/
		}
	}

	// Draw the scene
	GameObject* root = App->scene->root;
	if (root != nullptr)
	{
		ComponentTransform* transform = root->GetComponent<ComponentTransform>();
		transform->InvalidateHierarchy();

		DrawGameObject(App->scene->root);
	}

	return UpdateStatus::CONTINUE;
}

UpdateStatus ModuleRender::PostUpdate()
{
	BROFILER_CATEGORY("ModuleRender - PostUpdate", Profiler::Color::Green)

	SDL_GL_SwapWindow(App->window->window);

	return UpdateStatus::CONTINUE;
}

bool ModuleRender::CleanUp()
{
	glDeleteTextures(1, &render_texture);
	glDeleteRenderbuffers(1, &depth_renderbuffer);
	glDeleteFramebuffers(1, &framebuffer);

	return true;
}

void ModuleRender::ViewportResized(int width, int height)
{
	viewport_width = width;
	viewport_height = height;

	// Framebuffer calculations
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glBindTexture(GL_TEXTURE_2D, render_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, render_texture, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, depth_renderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_renderbuffer);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG("ERROR: Framebuffer is not complete!");
	}
}

void ModuleRender::SetVSync(bool vsync)
{
	SDL_GL_SetSwapInterval(vsync);
}

void ModuleRender::DrawGameObject(GameObject* game_object)
{
	ComponentTransform* transform = game_object->GetComponent<ComponentTransform>();
	std::vector<ComponentMesh*> meshes = game_object->GetComponents<ComponentMesh>();
	std::vector<ComponentMaterial*> materials = game_object->GetComponents<ComponentMaterial>();

	transform->CalculateGlobalMatrix();
	for (ComponentMesh* mesh : meshes)
	{
		mesh->Draw(materials, transform->GetGlobalMatrix());
	}

	for (GameObject* child : game_object->GetChildren())
	{
		DrawGameObject(child);
	}
}
