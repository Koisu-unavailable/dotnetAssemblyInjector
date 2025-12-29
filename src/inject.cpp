#include <windows.h>
#include <Psapi.h>
#include <stdio.h>
#include <string>
#define pNameBufSize 100

void printLastError();



int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("usage: injector <path to dll> <pid>");
        return 0;
    }
    PCSTR pathToDll = argv[1];
    DWORD pid = atoi(argv[2]);
    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!processHandle)
    {
        printf("Couldn't open process with pid %lu, maybe the pid is invalid or you don't have permission?\n", pid);
        return 1;
    }

    wchar_t processName[pNameBufSize];
    int get_file_name_result;
    get_file_name_result = GetModuleFileNameEx(processHandle, NULL, processName, pNameBufSize);

    if (get_file_name_result == 0)
    {
        printLastError();
        return 1;
    }

    printf("Injecting %s into pid: %lu (", pathToDll, pid);
    wprintf(processName);
    printf(")\n");

    LPVOID mem = VirtualAllocEx(processHandle, NULL, strlen(pathToDll) + 1, (MEM_COMMIT | MEM_RESERVE), PAGE_READWRITE);

    if (mem == NULL)
    {
        printLastError();
        return 1;
    }

    BOOL result = WriteProcessMemory(processHandle, mem, pathToDll, strlen(pathToDll) + 1, NULL);

    if (result == FALSE)
    {
        printLastError();
        return 1;
    }

    HMODULE addressOfKernel32 = GetModuleHandleW(L"kernel32.dll");

    if (addressOfKernel32 == NULL)
    {
        printLastError();
        return 1;
    }

    FARPROC load_library_address = GetProcAddress(addressOfKernel32, "LoadLibraryA");

    if (load_library_address == NULL)
    {
        printLastError();
        return 1;
    }

    HANDLE hThread = CreateRemoteThread(
        processHandle, NULL, 0,
        (LPTHREAD_START_ROUTINE)load_library_address,
        mem, 0, NULL);

    if (hThread == NULL)
    {
        printLastError();
        return 1;
    }
    size_t func_size = 10000;
    auto funcMem = VirtualAllocEx(processHandle, NULL, func_size, (MEM_COMMIT | MEM_RESERVE), PAGE_EXECUTE_READWRITE);
    void* callFuncCode = malloc(func_size);
    ReadProcessMemory(GetCurrentProcess(), &callFunc, callFuncCode, );
    
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(processHandle);
    return 0;
}

void callFunc()
{
    HMODULE addressOfKernel32 = GetModuleHandleW(L"kernel32.dll");
    HMODULE loadedDllHandle = GetModuleHandleA("dotnetAssemblyInjector.dll");
    FARPROC GetProcAddress_address = GetProcAddress(addressOfKernel32, "GetProcAddress");
    FARPROC Func_address = GetProcAddress(loadedDllHandle, "inject"); // todo: MAKE VARIABLE
    Func_address();
}

void printLastError()
{
    LPSTR pointer_to_message = NULL;
    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        GetLastError(),
        LANG_USER_DEFAULT,
        (LPSTR)&pointer_to_message,
        0,
        NULL);
    printf("%s", pointer_to_message);
}