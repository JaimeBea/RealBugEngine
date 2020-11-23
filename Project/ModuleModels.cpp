#include "ModuleModels.h"

#include "Globals.h"
#include "Logging.h"
#include <string>

static void AssimpLogCallback(const char* message, char* user)
{
    std::string message_str = message;
    std::string final_message_str = message_str.substr(0, message_str.find_last_of('\n'));
    LOG(final_message_str.c_str());
}

bool ModuleModels::Init()
{
    models.Reserve(10);

#ifdef _DEBUG
    log_stream.callback = AssimpLogCallback;
    aiAttachLogStream(&log_stream);
#endif

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
