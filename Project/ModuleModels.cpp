#include "ModuleModels.h"

#include "Globals.h"
#include "Logging.h"

#include <string>

static void AssimpLogCallback(const char* message, char* user)
{
    std::string message_str = message;
    std::string final_message_str = message_str.substr(0, message_str.find_last_of('\n'));
    //LOG(final_message_str.c_str());
}

bool ModuleModels::Init()
{
    models.Allocate(10);

#ifdef _DEBUG
    log_stream.callback = AssimpLogCallback;
    aiAttachLogStream(&log_stream);
#endif

    return true;
}

bool ModuleModels::CleanUp()
{
    for (Model& model : models)
    {
        model.Release();
    }
    models.Clear();

#ifdef _DEBUG
    aiDetachAllLogStreams();
#endif

    return true;
}

Model* ModuleModels::LoadModel(const char* file_name)
{
    Model* model = models.Obtain();
    if (!model->Load(file_name))
    {
        models.Release(model);
        return nullptr;
    }
    return model;
}

void ModuleModels::ReleaseModel(Model* model)
{
    model->Release();
    models.Release(model);
}
