#include <windows.h>

int main(void)
{
    
    while (1)
    {
        STARTUPINFOA si = {0};
        PROCESS_INFORMATION pi = {0};
    
        si.cb = sizeof(si);
    
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
    }

    return 0;
}
