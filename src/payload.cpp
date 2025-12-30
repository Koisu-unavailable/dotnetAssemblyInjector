#include "../includes/nethost.h"
#include "../includes/coreclr_delegates.h"
#include "../includes/hostfxr.h"
#include "log.h"
#include "readConfig.hpp"
#include "error.h"
#include "util.hpp"
#include <windows.h>
#include <iostream>
#include <cstdlib>
#include <cwchar>
#include <string.h>
#include <windows.h>
#include <format>
#include <print>

using string_t = std::basic_string<char_t>;
#define STR(x) std::format("{}", x)
namespace
{
    // Globals to hold hostfxr exports
    hostfxr_initialize_for_runtime_config_fn init_fptr;
    hostfxr_get_runtime_delegate_fn get_delegate_fptr;
    hostfxr_close_fn close_fptr;
}
void *get_export(void *h, const char *name)
{
    FARPROC f = ::GetProcAddress((HMODULE)h, name);
    return (void *)f;
}
// Use nethost library to get location of hostfxr and get it's exports
bool load_hostfxr()
{
    // Preallocate a large buffer for path, so no buiffer overflows YAY!!!
    char_t buffer[MAX_PATH];
    size_t bufferLen = sizeof(buffer) / sizeof(char_t); // how many characters it can store
    int rc = get_hostfxr_path(buffer, &bufferLen, nullptr);

    if (rc != 0)
    { 
        return false;
    }
    void *lib = LoadLibraryW(buffer);

    // function pointers to function in the dll
    init_fptr = (hostfxr_initialize_for_runtime_config_fn)get_export(lib, "hostfxr_initialize_for_runtime_config");
    get_delegate_fptr = (hostfxr_get_runtime_delegate_fn)get_export(lib, "hostfxr_get_runtime_delegate");
    close_fptr = (hostfxr_close_fn)get_export(lib, "hostfxr_close");
    return (init_fptr && get_delegate_fptr && close_fptr);
}
load_assembly_and_get_function_pointer_fn get_dotnet_load_assembly(const char_t *configPath)
{
    if(!::load_hostfxr()) handleError("Could not load hostfxr");
    void *load_assembly_and_get_function_pointer = nullptr;
    hostfxr_handle ctx = nullptr;
    int rc = init_fptr(configPath, nullptr, &ctx);
    if (rc != 1 || ctx == nullptr)
    {
        handleError("Catastrophic error occured"); // TODO: Make better
        close_fptr(ctx);
        return nullptr;
    }

    // Get the function pointer that loads an assembly and gets the main function
    rc = get_delegate_fptr(
        ctx,
        hdt_load_assembly_and_get_function_pointer,
        &load_assembly_and_get_function_pointer);
    if (rc != 0)
    {
        handleError("Error occured getting delegate function pointer.");
    }
    close_fptr(ctx);
    return (load_assembly_and_get_function_pointer_fn)load_assembly_and_get_function_pointer;
}
int inject()
{

    const std::string logFp = "payload.log"; // TODO: add to config
    Logger logger(logFp);
    logger.LogMessage("Beginning Injection");
    const std::string configPath = "config.cfg"; // TODO: at some point, make this be able to be varied
    logger.LogMessage(std::format("Loading config: {}", configPath));
    Config_t config = parseConfig(const_cast<char *>(configPath.c_str()));
    logger.LogMessage(std::format("Loaded config: \n {}", pprintConfig(&config)));
    load_assembly_and_get_function_pointer_fn load_assembly_and_get_function_pointer = nullptr;
    string_t path = s2ws(config.configPath);
    load_assembly_and_get_function_pointer = get_dotnet_load_assembly(path.c_str());
    typedef void(CORECLR_DELEGATE_CALLTYPE * DoPatchingFn)();
    DoPatchingFn DoPatching;
    auto typeName = s2ws(std::format("{}, {}", config.patchMethodClass, config.patchMethodAssembly));

    logger.LogMessage(ws2s(typeName).c_str());
    auto patchDllPath = s2ws(config.patchDllPath);
    logger.LogMessage(ws2s(std::format(L"patch Dll Path: {}", patchDllPath)));
    auto patchMethodName = s2ws(config.patchMethodName);
    logger.LogMessage(ws2s(std::format(L"patchMethodName: {}", patchMethodName)));
    int rc = load_assembly_and_get_function_pointer(
        patchDllPath.c_str(),
        typeName.c_str(),
        patchMethodName.c_str(),
        UNMANAGEDCALLERSONLY_METHOD,
        nullptr,
        (void **)&DoPatching);
    if (rc != 0 || DoPatching == nullptr)
    {
        char message[1000];
        FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            rc,
            LANG_USER_DEFAULT,
            (LPSTR)&message,
            0,
            nullptr);
        handleError(std::format("An error occured, load_assembly_and_get_function_pointer_fn returned: {}", rc).c_str());
        handleError(std::format("Message: {}", message).c_str());
        return 1;
    }
    logger.LogMessage(std::format("Calling {}", patchMethodName);
    DoPatching();
    return 0;
}

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL, // handle to DLL module
    DWORD fdwReason,    // reason for calling function
    LPVOID lpvReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        inject();
    }
}
