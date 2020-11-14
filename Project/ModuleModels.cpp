#include "ModuleModels.h"

bool ModuleModels::Init()
{
    models.Reserve(10);

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
    model.Load(file_name);
    return models.Offer(std::move(model));
}

void ModuleModels::ReleaseModel(unsigned model)
{
    models.Remove(model);
}
