#define SECURITY_WIN32

#include <stdio.h>
#include <windows.h>
#include <stdarg.h>
#include "detours.h"

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "Secur32.lib")
#pragma comment(lib, "detours.lib")

void debugPrintf(const char *fmt, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    printf("%s\n", buffer);
    OutputDebugStringA(buffer);
}

/*
NtCreateUserProcess
source: https://ntdoc.m417z.com/ntcreateuserprocess

*/ 
typedef void* PVOID_GENERIC;
typedef struct _OBJECT_ATTRIBUTES OBJECT_ATTRIBUTES;
typedef OBJECT_ATTRIBUTES* POBJECT_ATTRIBUTES;
typedef const OBJECT_ATTRIBUTES* PCOBJECT_ATTRIBUTES;


long (WINAPI * pNtCreateUserProcess)(
    PHANDLE ProcessHandle,
    PHANDLE ThreadHandle,
    ACCESS_MASK ProcessDesiredAccess,
    ACCESS_MASK ThreadDesiredAccess,
    PCOBJECT_ATTRIBUTES ProcessObjectAttributes,
    PCOBJECT_ATTRIBUTES ThreadObjectAttributes,
    ULONG ProcessFlags,
    ULONG ThreadFlags,
    PVOID_GENERIC ProcessParameters,
    PVOID_GENERIC CreateInfo,
    PVOID_GENERIC AttributeList
);

long HookedNtCreateUserProcess(
    PHANDLE ProcessHandle,
    PHANDLE ThreadHandle,
    ACCESS_MASK ProcessDesiredAccess,
    ACCESS_MASK ThreadDesiredAccess,
    PCOBJECT_ATTRIBUTES ProcessObjectAttributes,
    PCOBJECT_ATTRIBUTES ThreadObjectAttributes,
    ULONG ProcessFlags,
    ULONG ThreadFlags,
    PVOID_GENERIC ProcessParameters,
    PVOID_GENERIC CreateInfo,
    PVOID_GENERIC AttributeList
) {
    long returnValue;
    debugPrintf("[!] NtCreateUserProcess\n");
    returnValue = pNtCreateUserProcess(ProcessHandle, ThreadHandle, ProcessDesiredAccess, ThreadDesiredAccess, ProcessObjectAttributes, ThreadObjectAttributes, ProcessFlags, ThreadFlags, ProcessParameters, CreateInfo, AttributeList);
    return returnValue;
}

int AttachHooks(){
    DetourAttach((PVOID*)&pNtCreateUserProcess, HookedNtCreateUserProcess);
    return 0;
}

int DetachHooks(){
    DetourDetach((PVOID*)&pNtCreateUserProcess, HookedNtCreateUserProcess);
    return 0;
}

int AddHook(void) {

    // Dynamic NTDLL resolution
    // Nt* functions are part of windows native api, but the name is not available as a resolved symbol to the linker 
    // In order to get the address of the original ntdll's functions, a runtime dynamic resolution is needed
    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    if (hNtdll != NULL) {
        pNtCreateUserProcess = (void*)GetProcAddress(hNtdll, "NtCreateUserProcess");
    }

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    AttachHooks();
    DetourTransactionCommit();

    return 1;
}

int RemoveHook(void) {
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetachHooks();
    DetourTransactionCommit();

    return 1;
}

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved) {
    switch (dwReason) {
        case DLL_PROCESS_ATTACH:
            AddHook();
            break;
             
        case DLL_THREAD_ATTACH:
            break;
             
        case DLL_THREAD_DETACH:
            break;
             
        case DLL_PROCESS_DETACH:
            RemoveHook();
            break;
    }
         
    return TRUE;
}