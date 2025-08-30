#include "includes/nethost.h"
#include "includes/coreclr_delegates.h"
#include "includes/hostfxr.h"
#include <windows.h>
#include <iostream>
#include <cstdlib>
#include <cwchar>
#include <string.h>
#include <windows.h>
using string_t = std::basic_string<char_t>;
#define STR(s) L##s

namespace
{
    // Globals to hold hostfxr exports
    hostfxr_initialize_for_dotnet_command_line_fn init_for_cmd_line_fptr;
    hostfxr_initialize_for_runtime_config_fn init_fptr;
    hostfxr_get_runtime_delegate_fn get_delegate_fptr;
    hostfxr_run_app_fn run_app_fptr;
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
    size_t bufferSize = sizeof(buffer) / sizeof(char_t); // how many characters it can store
    int rc = get_hostfxr_path(buffer, &bufferSize, nullptr);

    if (rc != 0)
    { // error
        return false;
    }
    char *ansiBuffer = new char[wcslen(buffer) + 1];
    wcstombs(ansiBuffer, buffer, wcslen(buffer) + 1);

    void *lib = LoadLibraryW(buffer);

    delete[] ansiBuffer;
    // function pointers to function in the dll
    init_fptr = (hostfxr_initialize_for_runtime_config_fn)get_export(lib, "hostfxr_initialize_for_runtime_config");
    get_delegate_fptr = (hostfxr_get_runtime_delegate_fn)get_export(lib, "hostfxr_get_runtime_delegate");
    close_fptr = (hostfxr_close_fn)get_export(lib, "hostfxr_close");
    return (init_fptr && get_delegate_fptr && close_fptr);
}
load_assembly_and_get_function_pointer_fn get_dotnet_load_assembly(const char_t *configPath)
{
    ::load_hostfxr();
    void *load_assembly_and_get_function_pointer = nullptr;
    hostfxr_handle ctx = nullptr;
    int rc = init_fptr(configPath, nullptr, &ctx);
    if (rc == 1)
    {
        printf("YOO!");
    }
    if (rc != 1 || ctx == nullptr)
    {
        std::cout << "Error occured" << "\n";
        close_fptr(ctx);
        return nullptr;
    }

    // Get the function pointer that loads an assembly and gets the main function
    rc = get_delegate_fptr(
        ctx,
        hdt_load_assembly_and_get_function_pointer,
        &load_assembly_and_get_function_pointer);
    if (rc != 0){
        printf("F!");
    }
    close_fptr(ctx);
    return (load_assembly_and_get_function_pointer_fn)load_assembly_and_get_function_pointer;
}
enum InjectErrors
{
    Ok,
    CouldntMalloc,
    CouldntFormat
};
int inject()
{
    load_assembly_and_get_function_pointer_fn load_assembly_and_get_function_pointer = nullptr;
    string_t path = STR("./customDotnetHostTutorial.runtimeconfig.json");
    load_assembly_and_get_function_pointer = get_dotnet_load_assembly(path.c_str());
    
    
    typedef void (CORECLR_DELEGATE_CALLTYPE* DoPatchingFn)();
    DoPatchingFn DoPatching;
    int rc = load_assembly_and_get_function_pointer(
        STR("./HarmonyPatch.dll"),
        STR("TestPatch.MyPatcher, HarmonyPatch"),
        STR("DoPatching"),
        // STR("TestPatch.MyPatcher+DoPatchingDelegate, HarmonyPatch") /*delegate_type_name*/,
        UNMANAGEDCALLERSONLY_METHOD,
        nullptr,
        (void**)&DoPatching);
    if (rc != 0 || DoPatching == nullptr){
        MessageBoxA(NULL, "An error occured", "Oops", MB_ICONEXCLAMATION);
    }
    DoPatching();
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reasonForCall, LPVOID lpReserved)
{
    switch (reasonForCall)
    {
    case DLL_PROCESS_ATTACH:
        inject();
        MessageBoxA((HWND)hModule, "Success", "Successfully Injected the DLL", MB_ICONEXCLAMATION);
        break;
    case DLL_THREAD_ATTACH:
        MessageBoxA(NULL, "Tehee attached but THREAD!", "WARNING", MB_ICONEXCLAMATION);
        break;
    case DLL_PROCESS_DETACH:
        MessageBoxA(NULL, "Aw, detached", "WARNING", MB_ICONEXCLAMATION);
        break;
    case DLL_THREAD_DETACH:
        MessageBoxA(NULL, "Aw detached, but THREAD", "WARNING", MB_ICONEXCLAMATION);
        break;
    default:
        break;
    }
    return TRUE;
}