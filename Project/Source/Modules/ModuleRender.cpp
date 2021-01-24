#include "ModuleRender.h"

#include "Globals.h"
#include "Application.h"
#include "Utils/Logging.h"
#include "Components/ComponentMesh.h"
#include "Components/ComponentBoundingBox.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentMaterial.h"
#include "Modules/ModuleInput.h"
#include "Modules/ModuleWindow.h"
#include "Modules/ModuleCamera.h"
#include "Modules/ModuleDebugDraw.h"
#include "Modules/ModuleResources.h"
#include "Modules/ModuleScene.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModulePrograms.h"

#include "Geometry/AABB.h"
#include "Geometry/AABB2D.h"
#include "Geometry/OBB.h"
#include "debugdraw.h"
#include "GL/glew.h"
#include "SDL.h"
#include "Brofiler.h"

#include "Utils/Leaks.h"

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

	// Draw Skybox as a first element
	DrawSkyBox();

	// Draw the scene
	//PerformanceTimer timer;
	//timer.Start();
	App->camera->CalculateFrustumPlanes();
	for (GameObject& game_object : App->scene->game_objects)
	{
		game_object.flag = false;
		if (game_object.is_in_quadtree) continue;

		ComponentBoundingBox* bounding_box = game_object.GetComponent<ComponentBoundingBox>();
		if (bounding_box == nullptr) continue;

		const AABB& game_object_aabb = bounding_box->GetWorldAABB();
		const OBB& game_object_obb = bounding_box->GetWorldOBB();
		if (CheckIfInsideFrustum(game_object_aabb, game_object_obb))
		{
			DrawGameObject(&game_object);
		}
	}
	if (App->scene->quadtree.IsOperative())
	{
		DrawSceneRecursive(App->scene->quadtree.root, App->scene->quadtree.bounds);
	}
	//LOG("Scene draw: %llu mis", timer.Stop());

	// Draw Guizmos
	GameObject* selected_object = App->editor->selected_object;
	if (selected_object) selected_object->DrawGizmos();

	// Draw quadtree
	if (draw_quadtree)
	{
		DrawQuadtreeRecursive(App->scene->quadtree.root, App->scene->quadtree.bounds);
	}

	// Draw debug draw
	App->debug_draw->Draw(App->camera->GetViewMatrix(), App->camera->GetProjectionMatrix(), viewport_width, viewport_height);

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

void ModuleRender::DrawQuadtreeRecursive(const Quadtree<GameObject>::Node& node, const AABB2D& aabb)
{
	if (node.IsBranch())
	{
		vec2d center = aabb.minPoint + (aabb.maxPoint - aabb.minPoint) * 0.5f;

		const Quadtree<GameObject>::Node& top_left = node.child_nodes->nodes[0];
		AABB2D top_left_aabb = {{aabb.minPoint.x, center.y}, {center.x, aabb.maxPoint.y}};
		DrawQuadtreeRecursive(top_left, top_left_aabb);

		const Quadtree<GameObject>::Node& top_right = node.child_nodes->nodes[1];
		AABB2D top_right_aabb = {{center.x, center.y}, {aabb.maxPoint.x, aabb.maxPoint.y}};
		DrawQuadtreeRecursive(top_right, top_right_aabb);

		const Quadtree<GameObject>::Node& bottom_left = node.child_nodes->nodes[2];
		AABB2D bottom_left_aabb = {{aabb.minPoint.x, aabb.minPoint.y}, {center.x, center.y}};
		DrawQuadtreeRecursive(bottom_left, bottom_left_aabb);

		const Quadtree<GameObject>::Node& bottom_right = node.child_nodes->nodes[3];
		AABB2D bottom_right_aabb = {{center.x, aabb.minPoint.y}, {aabb.maxPoint.x, center.y}};
		DrawQuadtreeRecursive(bottom_right, bottom_right_aabb);
	}
	else
	{
		float3 points[8] = {
			{aabb.minPoint.x, 0, aabb.minPoint.y},
			{aabb.maxPoint.x, 0, aabb.minPoint.y},
			{aabb.maxPoint.x, 0, aabb.maxPoint.y},
			{aabb.minPoint.x, 0, aabb.maxPoint.y},
			{aabb.minPoint.x, 30, aabb.minPoint.y},
			{aabb.maxPoint.x, 30, aabb.minPoint.y},
			{aabb.maxPoint.x, 30, aabb.maxPoint.y},
			{aabb.minPoint.x, 30, aabb.maxPoint.y},
		};
		dd::box(points, dd::colors::White);
	}
}

void ModuleRender::DrawSceneRecursive(const Quadtree<GameObject>::Node& node, const AABB2D& aabb)
{
	AABB aabb_3d = AABB({aabb.minPoint.x, -1000000.0f, aabb.minPoint.y}, {aabb.maxPoint.x, 1000000.0f, aabb.maxPoint.y});
	if (CheckIfInsideFrustum(aabb_3d, OBB(aabb_3d)))
	{
		if (node.IsBranch())
		{
			vec2d center = aabb.minPoint + (aabb.maxPoint - aabb.minPoint) * 0.5f;

			const Quadtree<GameObject>::Node& top_left = node.child_nodes->nodes[0];
			AABB2D top_left_aabb = {{aabb.minPoint.x, center.y}, {center.x, aabb.maxPoint.y}};
			DrawSceneRecursive(top_left, top_left_aabb);

			const Quadtree<GameObject>::Node& top_right = node.child_nodes->nodes[1];
			AABB2D top_right_aabb = {{center.x, center.y}, {aabb.maxPoint.x, aabb.maxPoint.y}};
			DrawSceneRecursive(top_right, top_right_aabb);

			const Quadtree<GameObject>::Node& bottom_left = node.child_nodes->nodes[2];
			AABB2D bottom_left_aabb = {{aabb.minPoint.x, aabb.minPoint.y}, {center.x, center.y}};
			DrawSceneRecursive(bottom_left, bottom_left_aabb);

			const Quadtree<GameObject>::Node& bottom_right = node.child_nodes->nodes[3];
			AABB2D bottom_right_aabb = {{center.x, aabb.minPoint.y}, {aabb.maxPoint.x, center.y}};
			DrawSceneRecursive(bottom_right, bottom_right_aabb);
		}
		else
		{
			const Quadtree<GameObject>::Element* element = node.first_element;
			while (element != nullptr)
			{
				GameObject* game_object = element->object;
				if (!game_object->flag)
				{
					ComponentBoundingBox* bounding_box = game_object->GetComponent<ComponentBoundingBox>();
					const AABB& game_object_aabb = bounding_box->GetWorldAABB();
					const OBB& game_object_obb = bounding_box->GetWorldOBB();
					if (CheckIfInsideFrustum(game_object_aabb, game_object_obb))
					{
						DrawGameObject(game_object);
					}

					game_object->flag = true;
				}
				element = element->next;
			}
		}
	}
}

bool ModuleRender::CheckIfInsideFrustum(const AABB& aabb, const OBB& obb)
{
	float3 points[8];
	obb.GetCornerPoints(points);

	const FrustumPlanes& frustum_planes = App->camera->GetFrustumPlanes();
	for (const Plane& plane : frustum_planes.planes)
	{
		// check box outside/inside of frustum
		int out = 0;
		for (int i = 0; i < 8; i++)
		{
			out += (plane.SignedDistance(points[i]) > 0 ? 1 : 0);
		}
		if (out == 8) return false;
	}

	// check frustum outside/inside box
	int out;
	out = 0;
	for (int i = 0; i < 8; i++) out += ((frustum_planes.points[i].x > aabb.MaxX()) ? 1 : 0);
	if (out == 8) return false;
	out = 0;
	for (int i = 0; i < 8; i++) out += ((frustum_planes.points[i].x < aabb.MinX()) ? 1 : 0);
	if (out == 8) return false;
	out = 0;
	for (int i = 0; i < 8; i++) out += ((frustum_planes.points[i].y > aabb.MaxY()) ? 1 : 0);
	if (out == 8) return false;
	out = 0;
	for (int i = 0; i < 8; i++) out += ((frustum_planes.points[i].y < aabb.MinY()) ? 1 : 0);
	if (out == 8) return false;
	out = 0;
	for (int i = 0; i < 8; i++) out += ((frustum_planes.points[i].z > aabb.MaxZ()) ? 1 : 0);
	if (out == 8) return false;
	out = 0;
	for (int i = 0; i < 8; i++) out += ((frustum_planes.points[i].z < aabb.MinZ()) ? 1 : 0);
	if (out == 8) return false;

	return true;
}

void ModuleRender::DrawGameObject(GameObject* game_object)
{
	ComponentTransform* transform = game_object->GetComponent<ComponentTransform>();
	std::vector<ComponentMesh*> meshes = game_object->GetComponents<ComponentMesh>();
	std::vector<ComponentMaterial*> materials = game_object->GetComponents<ComponentMaterial>();
	ComponentBoundingBox* bounding_box = game_object->GetComponent<ComponentBoundingBox>();

	if (bounding_box && draw_all_bounding_boxes)
	{
		bounding_box->DrawBoundingBox();
	}

	for (ComponentMesh* mesh : meshes)
	{
		mesh->Draw(materials, transform->GetGlobalMatrix());
	}
}

void ModuleRender::DrawSkyBox()
{
	if (skybox_active)
	{
		glDepthFunc(GL_LEQUAL);

		unsigned program = App->programs->skybox_program;
		glUseProgram(program);
		glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, App->camera->GetViewMatrix().ptr());
		glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, App->camera->GetProjectionMatrix().ptr());
		glUniform1i(glGetUniformLocation(program, "cubemap"), 0);

		glBindVertexArray(App->scene->skybox_vao);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, App->scene->skybox_cube_map->gl_texture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		glDepthFunc(GL_LESS);
	}
}
