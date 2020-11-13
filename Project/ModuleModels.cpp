#include "ModuleModels.h"

bool ModuleModels::Init()
{
    models.reserve(10);

    return true;
}

bool ModuleModels::CleanUp()
{
    for (Model& model : models)
    {
        model.Release();
    }

    return true;
}

unsigned ModuleModels::LoadModel(const char* file_name)
{
    Model model;
    model.Load(file_name);
    unsigned id = models.size();
    models.push_back(model);
    return id;
}

void ModuleModels::ReleaseModel(unsigned model)
{
    models[model].Release();
    models.erase(models.begin() + model);
}
