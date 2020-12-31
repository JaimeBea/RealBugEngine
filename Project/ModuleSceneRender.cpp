#include "ModuleSceneRender.h"

#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleScene.h"
#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "ComponentMesh.h"
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

void ModuleSceneRender::DrawSkyBox()
{
	if (skybox_active)
	{
		App->scene->DrawSkyBox();
	}
}
