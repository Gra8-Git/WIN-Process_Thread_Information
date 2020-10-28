#ifndef UNICODE
#define UNICODE
#define UNICODE_WAS_UNDEFINED
#endif

#include <Windows.h>
#include <iostream>
#include <sddl.h>
#include <tchar.h>
#ifdef UNICODE_WAS_UNDEFINED
#undef UNICODE
#endif

#pragma commant(lib, "advapi32.lib");
//Visual Code C++ minGW  for proper work need to be one more function
BOOL EnableWindowsPrivilege(BOOL State, HANDLE hprocess)
{
    HANDLE hToken;
    TOKEN_PRIVILEGES tokenp;
    DWORD dwSize;
    ZeroMemory(&tokenp, sizeof(tokenp));
    tokenp.PrivilegeCount = 1;
    if (!OpenProcessToken(hprocess, TOKEN_ALL_ACCESS, &hToken))
    {
        return FALSE;
    }
    if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tokenp.Privileges[0].Luid))
    {
        CloseHandle(hToken);
    }
    if (State)
    {
        tokenp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    }
    else
    {
        tokenp.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;
    }
    if (!AdjustTokenPrivileges(hToken, FALSE, &tokenp, 0, NULL, &dwSize))
    {
        CloseHandle(hToken);
    }
    return CloseHandle(hToken);
}

int main(void)
{
    HANDLE pToken = NULL;
    HANDLE hprocess;

    //process id to enable external debug mode
    int PID = 7777;
    hprocess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
    
    if (!EnableWindowsPrivilege(TRUE, hprocess))
    {
        printf("Could not enable Debug!\n");
        return 1;
    }
    else
    {
        printf("SE_DEBUG_NAME: privilages enabled\n");
    }
    return 0;
}
