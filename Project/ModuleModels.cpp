#include "ModuleModels.h"

#include "Globals.h"

bool ModuleModels::Init()
{
    models.Reserve(10);

    return true;
}

bool ModuleModels::CleanUp()
{
    for (Model& current_model : models)
    {
        current_model.Release();
    }
    models.Clear();

    return true;
}

unsigned ModuleModels::LoadModel(const char* file_name)
{
    Model current_model = Model();

    if (!current_model.Load(file_name))
    {
        return 0;
    }

    return models.Offer(std::move(current_model));
}

void ModuleModels::ReleaseModel(unsigned current_model)
{
    models[current_model].Release();
    models.Remove(current_model);
}
