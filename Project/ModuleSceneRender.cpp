#include "ModuleSceneRender.h"

#include "Application.h"
#include "ModuleResources.h"
#include "ModulePrograms.h"
#include "ModuleScene.h"
#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "ComponentMesh.h"
#include "ComponentBoundingBox.h"
#include "ComponentTransform.h"
#include "ComponentMaterial.h"

#include "GL/glew.h"
#include "Brofiler.h"

#include "Leaks.h"

UpdateStatus ModuleSceneRender::Update()
{
	BROFILER_CATEGORY("ModuleSceneRender - Update", Profiler::Color::DarkBlue)

	// Draw Skybox as a first element
	DrawSkyBox();

	// Load model or texture if one gets dropped
	const char* dropped_file_name = App->input->GetDroppedFileName();
	if (dropped_file_name != nullptr)
	{
		bool loaded_scene = App->scene->Load(dropped_file_name);
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

void ModuleSceneRender::DrawGameObject(GameObject* game_object)
{
	ComponentTransform* transform = game_object->GetComponent<ComponentTransform>();
	ComponentBoundingBox* bounding_box = game_object->GetComponent<ComponentBoundingBox>();
	std::vector<ComponentMesh*> meshes = game_object->GetComponents<ComponentMesh>();
	std::vector<ComponentMaterial*> materials = game_object->GetComponents<ComponentMaterial>();

	transform->CalculateGlobalMatrix();
	if (bounding_box != nullptr) bounding_box->CalculateWorldBoundingBox();
	for (ComponentMesh* mesh : meshes)
	{
		mesh->Draw(materials, transform->GetGlobalMatrix());
	}

	for (GameObject* child : game_object->GetChildren())
	{
		DrawGameObject(child);
	}
}

void ModuleSceneRender::DrawSkyBox()
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
