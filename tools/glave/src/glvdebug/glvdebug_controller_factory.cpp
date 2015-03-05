#include "glvdebug_controller_factory.h"
#include "glv_platform.h"

glvdebug_controller_factory::glvdebug_controller_factory()
{
}

glvdebug_controller_factory::~glvdebug_controller_factory()
{
}

glvdebug_controller* glvdebug_controller_factory::Load(const char* filename)
{
    char* exeDir = glv_platform_get_current_executable_directory();
#if defined(PLATFORM_WINDOWS)
    char* controllerPath = glv_copy_and_append(exeDir,"\\", filename);
#else
    char* controllerPath = glv_copy_and_append(exeDir,"/", filename);
#endif

    void* pLibrary = glv_platform_open_library(controllerPath);
    if (pLibrary == NULL)
    {
        glvdebug_output_error(QString("Failed to load controller '%1'").arg(controllerPath).toStdString().c_str());
#if defined(PLATFORM_LINUX)
        char* error = dlerror();
        glvdebug_output_error(error);
#endif
        glv_free(controllerPath);
        return NULL;
    }

    glv_free(exeDir);

    glvdebug_controller* pController = GLV_NEW(glvdebug_controller);
    if (pController != NULL)
    {
        pController->pLibrary = pLibrary;
        pController->InterpretTracePacket = (funcptr_glvdebug_controller_interpret_trace_packet)glv_platform_get_library_entrypoint(pLibrary, "glvdebug_controller_interpret_trace_packet");
        pController->LoadTraceFile = (funcptr_glvdebug_controller_load_trace_file)glv_platform_get_library_entrypoint(pLibrary, "glvdebug_controller_load_trace_file");
        pController->UnloadTraceFile = (funcptr_glvdebug_controller_unload_trace_file)glv_platform_get_library_entrypoint(pLibrary, "glvdebug_controller_unload_trace_file");
        pController->GetSettings = (funcptr_glvdebug_controller_get_settings)glv_platform_get_library_entrypoint(pLibrary, "glvdebug_controller_get_settings");
        pController->UpdateFromSettings = (funcptr_glvdebug_controller_update_from_settings)glv_platform_get_library_entrypoint(pLibrary, "glvdebug_controller_update_from_settings");

        assert(pController->InterpretTracePacket != NULL);
        assert(pController->LoadTraceFile != NULL);
        assert(pController->UnloadTraceFile != NULL);
        assert(pController->GetSettings != NULL);
        assert(pController->UpdateFromSettings != NULL);
    }

    return pController;
}

void glvdebug_controller_factory::Unload(glvdebug_controller** ppController)
{
    assert(ppController != NULL);
    assert(*ppController != NULL);
    glv_platform_close_library((*ppController)->pLibrary);
    GLV_DELETE(*ppController);
    *ppController = NULL;
}
