#ifndef UNICODE
#define UNICODE
#define UNICODE_WAS_UNDEFINED
#endif
#include "main.h"
#include <Windows.h>
#include <iostream>
#include <sddl.h>
#include <tchar.h>
#include <string.h>
#ifdef UNICODE_WAS_UNDEFINED
#undef UNICODE
#endif

typedef PVOID Privvalue;
typedef ULONG priv_ULONG;
typedef PULONG P_priv_ULONG;

#define NT_QUERY_INFORMATION_PROCESS_NAME "NtQueryInformationProcess"
typedef int (*FN_NtQueryInformationProcess)(HANDLE, int, PVOID, priv_ULONG, P_priv_ULONG);

#pragma commant(lib, "advapi32.lib");

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
    return TRUE;
}

BOOL IsProcessUAC(HANDLE hprocess)
{
    BOOL fIsElevated = FALSE;
    HANDLE hToken = NULL;
    TOKEN_ELEVATION elevation;
    DWORD dwSize;

    if (!OpenProcessToken(hprocess, TOKEN_QUERY, &hToken))
    {
        std::cout << "\n Failed to get Process Token : " << GetLastError() << "\n";
        goto Cleanup; // if Failed, we treat as False
    }

    if (!GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &dwSize))
    {
        std::cout << "\n Failed to get Token Information : " << GetLastError() << "\n";
        goto Cleanup; // if Failed, we treat as False
    }

    fIsElevated = elevation.TokenIsElevated;

Cleanup:
    if (hToken)
    {
        CloseHandle(hToken);
        hToken = NULL;
    }
    return fIsElevated;
}

_PROCESS_BASIC_INFORMATION64 readPibmemory64(HANDLE hprocess)
{
    int pid, ret;
    priv_ULONG read_length;
    HINSTANCE ntdll;
    FN_NtQueryInformationProcess NtQueryInformationProcess = NULL;
    _PROCESS_BASIC_INFORMATION64 pbi64{0};
    SIZE_T *dwBytesRead = NULL;

    ntdll = LoadLibrary(L"ntdll.dll"); //load liberary and get hInstance to get function
    if (ntdll == NULL)
    {
        std::cout << "Unable to load ntdll\n";
    }

    NtQueryInformationProcess = (FN_NtQueryInformationProcess)GetProcAddress(ntdll, NT_QUERY_INFORMATION_PROCESS_NAME); //request function
    if (NtQueryInformationProcess == NULL)
    {
        std::cout << "Unable to get address of " << NT_QUERY_INFORMATION_PROCESS_NAME << "\n";
    }

    ZeroMemory(&pbi64, sizeof(pbi64));
    ret = NtQueryInformationProcess(hprocess, ProcessBasicInformation, &pbi64, sizeof(pbi64), &read_length); //get pbi  from the remote process
    if (ret != 0)
    {
        std::cout << "Error at : " << NT_QUERY_INFORMATION_PROCESS_NAME << " ret : " << ret;
    }
    return pbi64;
}

_PROCESS_BASIC_INFORMATION readPibmemory32(HANDLE hprocess)
{
    int pid, ret;
    priv_ULONG read_length;
    HINSTANCE ntdll;
    FN_NtQueryInformationProcess NtQueryInformationProcess = NULL;
    _PROCESS_BASIC_INFORMATION pbi{0};
    SIZE_T *dwBytesRead = NULL;

    ntdll = LoadLibrary(L"ntdll.dll"); //load liberary and get hInstance to get function
    if (ntdll == NULL)
    {
        std::cout << "Unable to load ntdll\n";
    }

    NtQueryInformationProcess = (FN_NtQueryInformationProcess)GetProcAddress(ntdll, NT_QUERY_INFORMATION_PROCESS_NAME); //request function
    if (NtQueryInformationProcess == NULL)
    {
        std::cout << "Unable to get address of " << NT_QUERY_INFORMATION_PROCESS_NAME << "\n";
    }
    ZeroMemory(&pbi, sizeof(pbi));
    ret = NtQueryInformationProcess(hprocess, ProcessBasicInformation, &pbi, sizeof(pbi), &read_length); //get pbi  from the remote process
    if (ret != 0)
    {
        std::cout << "Error at : " << NT_QUERY_INFORMATION_PROCESS_NAME << " ret : " << ret;
    }
    return pbi;
}

PEB readPEBmemory(HANDLE hprocess, _PROCESS_BASIC_INFORMATION64 pbi64)
{
    PEB peb;
    UINT64 dwSize = 0;
    if (!ReadProcessMemory(hprocess, pbi64.PebBaseAddress, &peb, sizeof(PEB), &dwSize))
    {
        std::cout << "Could not read the address of PEB :" << GetLastError() << "\n";
    }
    return peb;
}

PVOID readRTLUserProcessParam(HANDLE hprocess, PVOID peb)
{
    PVOID rtlUserProcParamsAddress;

    /* get the address of ProcessParameters */
    if (!ReadProcessMemory(hprocess, &(((_PEB *)peb)->ProcessParameters), &rtlUserProcParamsAddress, sizeof(PVOID), NULL))
    {
        std::cout << "Could not read the address of ProcessParameters :" << GetLastError() << "\n";
    }
    return rtlUserProcParamsAddress;
}

UNICODE_STRING readRTLcommand(HANDLE hprocess, PVOID rtlUserProcParamsAddress)
{
    UNICODE_STRING commandLine = {};

    SIZE_T *readBytes = new SIZE_T;
    /*We use PVOID to save address inside varible and create point to read memory  */
    if (!ReadProcessMemory(hprocess, &(((_RTL_USER_PROCESS_PARAMETERS *)rtlUserProcParamsAddress)->CommandLine), &commandLine, sizeof(commandLine), NULL))
    {
        std::cout << "Could not read ->  rtlUserProcParamsAddress.CommandLine :" << GetLastError() << "\n";
    }
    return commandLine;
}

WCHAR *readRTLcommandLine(HANDLE hprocess, UNICODE_STRING commandLine)
{
    WCHAR *commandLineContents;

    commandLineContents = (WCHAR *)malloc(commandLine.Length);
    /* read the command line */
    if (!ReadProcessMemory(hprocess, commandLine.Buffer, commandLineContents, commandLine.Length, NULL))
    {
        std::cout << "Could not read  commandLine.Buffer string" << GetLastError() << "\n";
    }

    return commandLineContents;
}

int main(void)
{
    HANDLE pToken = NULL;
    HANDLE hprocess;
    PVOID rtlUserProcParamsAddress;
    PVOID BeingDebugged;
    UNICODE_STRING commandLine;
    WCHAR *commandLineContents;

    PEB peb;
    //process id to enable external debug mode
    int PID = 14440;
    hprocess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);

    if (IsProcessUAC(hprocess) == TRUE)
    {
        std::cout << "\t\tProcess Elevated\n";
    }
    else
    {
        std::cout << "\t\tProcess not elevated\n";
    }
    if (!EnableWindowsPrivilege(TRUE, hprocess))
    {
        std::cout << "\t\tNot  Debugged!\n\n";
        return 1;
    }
    else
    {
        std::cout << "\t\tSE_DEBUG_NAME: privilages enabled\n\n";
    }
#ifdef _WIN64
    _PROCESS_BASIC_INFORMATION64 pbi64 = readPibmemory64(hprocess);
    peb = readPEBmemory(hprocess, pbi64);
    rtlUserProcParamsAddress = readRTLUserProcessParam(hprocess, pbi64.PebBaseAddress);
#else
    _PROCESS_BASIC_INFORMATION pbi = readPibmemory32(hprocess);
    peb = readPEBmemory(hprocess, pbi);
    rtlUserProcParamsAddress = readRTLUserProcessParam(hprocess, pbi.PebBaseAddress);
#endif

    commandLine = readRTLcommand(hprocess, rtlUserProcParamsAddress);
    commandLineContents = readRTLcommandLine(hprocess, commandLine);

    std::wcout << "Command Line: " << commandLineContents << "\n";
    std::cout << "\n___________________________________________________________\n";
    std::cout << "\t\tPROCESS BASIC INFORMATION  x64 :\n";
    std::cout << "pbi PEBBASEADDRESS :" << pbi64.PebBaseAddress << "\n";
    std::cout << "pbi UNIQUEPROCESSID :" << pbi64.uUniqueProcessId << "\n";
    std::cout << "_____________________________________________________________\n";
    std::cout << "\t\tProcess PEB structure \n";
    if (peb.BeingDebugged == TRUE)
    {
        std::cout << "PEB Process debugged: ture\n";
    }
    else
    {
        std::cout << "PEB Process debugged: false\n";
    }
    std::cout << "_____________________________________________________________\n";

    return 0;
}
