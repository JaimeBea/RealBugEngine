#include "ModuleModels.h"

#include "Globals.h"

bool ModuleModels::Init()
{
    models.Reserve(10);

    return true;
}

bool ModuleModels::Start()
{
    current_model = LoadModel("Assets/BakerHouse/BakerHouse.fbx");

    return true;
}

bool ModuleModels::CleanUp()
{
    for (Model& model : models)
    {
        model.Release();
    }
    models.Clear();

    return true;
}

unsigned ModuleModels::LoadModel(const char* file_name)
{
    Model model = Model();

    if (!model.Load(file_name))
    {
        return 0;
    }

    return models.Offer(std::move(model));
}

void ModuleModels::ReleaseModel(unsigned current_model)
{
    models[current_model].Release();
    models.Remove(current_model);
}
