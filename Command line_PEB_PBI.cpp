/* compiled with mingw  and couple lib's :)   for x86 and x64 OS */
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

#ifdef _WOW64
typedef UINT64 PTR;
typedef ULONGLONG priv_ULONG;
typedef PULONGLONG P_priv_ULONG;
#define NT_QUERY_INFORMATION_PROCESS_NAME "NtWow64QueryInformationProcess64"
#else
typedef PVOID Privvalue;
typedef ULONG priv_ULONG;
typedef PULONG P_priv_ULONG;
#define NT_QUERY_INFORMATION_PROCESS_NAME "NtQueryInformationProcess"
#endif
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
        printf("\n Failed to get Process Token :%d.", GetLastError());
        goto Cleanup; // if Failed, we treat as False
    }

    if (!GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &dwSize))
    {
        printf("\nFailed to get Token Information :%d.", GetLastError());
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

_PROCESS_BASIC_INFORMATION64 readPibmemory(HANDLE hprocess)
{
    int pid, ret;
    priv_ULONG read_length;
    HINSTANCE ntdll;
    FN_NtQueryInformationProcess NtQueryInformationProcess = NULL;
    _PROCESS_BASIC_INFORMATION pbi{0};
    _PROCESS_BASIC_INFORMATION64 pbi64{0};
    SIZE_T *dwBytesRead = NULL;

    ntdll = LoadLibrary(L"ntdll.dll"); //load liberary and get hInstance to get function
    if (ntdll == NULL)
    {
        puts("Unable to load ntdll");
        return pbi64;
    }

    NtQueryInformationProcess = (FN_NtQueryInformationProcess)GetProcAddress(ntdll, NT_QUERY_INFORMATION_PROCESS_NAME); //request function
    if (NtQueryInformationProcess == NULL)
    {
        printf("Unable to get address of %s\n", NT_QUERY_INFORMATION_PROCESS_NAME);
        return pbi64;
    }

#ifdef _WIN64
    ZeroMemory(&pbi64, sizeof(pbi64));
    ret = NtQueryInformationProcess(hprocess, ProcessBasicInformation, &pbi64, sizeof(pbi64), &read_length); //get pbi  from the remote process
    if (ret != 0)
    {
        printf("Error at %s, ret: %d\n", NT_QUERY_INFORMATION_PROCESS_NAME, ret);
        return pbi64;
    }
#else
    ZeroMemory(&pbi, sizeof(pbi));
    ret = NtQueryInformationProcess(hprocess, ProcessBasicInformation, &pbi, sizeof(pbi), &read_length); //get pbi  from the remote process
    if (ret != 0)
    {
        printf("Error at %s, ret: %d\n", NT_QUERY_INFORMATION_PROCESS_NAME, ret);
        return pbi64;
    }
#endif
}

PEB readPEBmemory(HANDLE hprocess, _PROCESS_BASIC_INFORMATION64 pbi64)
{
    PEB peb;
    UINT64 dwSize = 0;
    if (!ReadProcessMemory(hprocess, pbi64.PebBaseAddress, &peb, sizeof(PEB), &dwSize))
    {
        printf("Could not read the address of PEB!%d\n", GetLastError());
    }
    return peb;
}

PVOID readRTLUserProcessParam(HANDLE hprocess, PVOID peb)
{
    PVOID rtlUserProcParamsAddress;

    /* get the address of ProcessParameters */
    if (!ReadProcessMemory(hprocess, &(((_PEB *)peb)->ProcessParameters), &rtlUserProcParamsAddress, sizeof(PVOID), NULL))
    {
        printf("Could not read the address of ProcessParameters!%d\n", GetLastError());
    }
    return rtlUserProcParamsAddress;
}

UNICODE_STRING readRTLcommand(HANDLE hprocess, PVOID rtlUserProcParamsAddress)
{
    UNICODE_STRING commandLine = {};

    SIZE_T *readBytes = new SIZE_T;
    /* read the CommandLine UNICODE_STRING structure */

    /*We use PVOID to save address inside varible and create point to read memory  */
    if (!ReadProcessMemory(hprocess, &(((_RTL_USER_PROCESS_PARAMETERS *)rtlUserProcParamsAddress)->CommandLine), &commandLine, sizeof(commandLine), NULL))
    {
        printf("Could not read CommandLine!  rtlUserProcParamsAddress.CommandLine %d\n", GetLastError());
    }
    return commandLine;
}

WCHAR *readRTLcommandLine(HANDLE hprocess, UNICODE_STRING commandLine)
{
    const int MAX_SIZE = 512;
    SIZE_T readBytes = 0;
    WCHAR *commandLineContents;

    commandLineContents = (WCHAR *)malloc(commandLine.Length);
    /* read the command line */
    if (!ReadProcessMemory(hprocess, commandLine.Buffer, commandLineContents, commandLine.Length, NULL))
    {
        printf("Could not read the command line   commandLine.Buffer string! %p\n", GetLastError());
    }

    return commandLineContents;
}

int main(void)
{
    HANDLE pToken = NULL;
    HANDLE hprocess;
    PVOID rtlUserProcParamsAddress;
    UNICODE_STRING commandLine;
    WCHAR *commandLineContents;

    PEB peb;
    PVOID pebbaseaddress;
    //process id to enable external debug mode
    int PID = 6316;
    hprocess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);

    if (IsProcessUAC(hprocess) == TRUE)
    {
        printf("process Elevated");
    }
    else
    {
        printf("procen not elevated");
    }
    if (!EnableWindowsPrivilege(TRUE, hprocess))
    {
        printf("Could not enable Debug!\n");
        return 1;
    }
    else
    {
        printf("SE_DEBUG_NAME: privilages enabled\n");
    }
    _PROCESS_BASIC_INFORMATION64 pbi64 = readPibmemory(hprocess);

    peb = readPEBmemory(hprocess, pbi64);
    rtlUserProcParamsAddress = readRTLUserProcessParam(hprocess, pbi64.PebBaseAddress);
    commandLine = readRTLcommand(hprocess, rtlUserProcParamsAddress);
    commandLineContents = readRTLcommandLine(hprocess, commandLine);
    printf("Command Line : %.*S\n", commandLine.Length / 2, commandLineContents);
    printf("\npib structure of the process PROCESS BASIC INFORMATION  x64 :\n");
    printf("\n___________________________________________________________\n");
    printf("pbi PEBBASEADDRESS : %p\n", pbi64.PebBaseAddress);
    printf("pbi AFFINITYMASK : %u\n", pbi64.AffinityMask);
    printf("pbi BASEPRIORITY : %d\n", pbi64.BasePriority);
    printf("pbi EXITSTATUS : %p\n", pbi64.ExitStatus);
    printf("pbi UNIQUEPROCESSID : %u\n", pbi64.uUniqueProcessId);
    printf("pbi INHERITEDFROMUNIQUEPROCESSID : %u\n", pbi64.uInheritedFromUniqueProcessId);
    printf("pbi RESERVED0 : %p\n", pbi64.Reserved0);
    printf("pbi RESERVED1 : %p\n", pbi64.Reserved1);
    printf("_____________________________________________________________\n");
    printf("Process PEB structure \n");
    if (peb.BeingDebugged == TRUE)
    {
        printf("PEB Process debugged: ture\n");
    }
    else
    {
        printf("PEB Process debugged: false\n");
    }
    printf("PEB AnsiCodePageData: %p\n", peb.AnsiCodePageData);
    printf("PEB AtlThunkSListPtr32: %p\n", peb.AtlThunkSListPtr32);
    if (peb.InheritedAddressSpace == TRUE)
    {
        printf("PEB InheritedAddressSpace: true\n");
    }
    else
    {
        printf("PEB InheritedAddressSpace: false\n");
    }
    printf("PEB CriticalSectionTimeout: %d\n", peb.CriticalSectionTimeout);
    printf("PEB OSPlatformId: %p\n", peb.EnvironmentUpdateCount);
    printf("PEB FastPebLock: %p\n", peb.FastPebLock);
    printf("PEB FastPebLockRoutine: %p\n", peb.FastPebLockRoutine);
    printf("PEB FastPebUnlockRoutine: %p\n", peb.FastPebUnlockRoutine);
    printf("PEB FastPebUnlockRoutine: %p\n", peb.FreeList);
    printf("PEB GdiDCAttributeList: %p\n", peb.GdiDCAttributeList);
    printf("PEB GdiHandleBuffer: %p\n", peb.GdiHandleBuffer);
    printf("PEB GdiSharedHandleTable: %p\n", peb.GdiSharedHandleTable);
    printf("PEB HeapDeCommitFreeBlockThreshold: %p\n", peb.HeapDeCommitFreeBlockThreshold);
    printf("PEB HeapDeCommitTotalFreeThreshold: %p\n", peb.HeapDeCommitTotalFreeThreshold);
    printf("PEB HeapSegmentCommit: %p\n", peb.HeapSegmentCommit);
    printf("PEB HeapSegmentReserve: %p\n", peb.HeapSegmentReserve);
    printf("PEB ImageBaseAddress: %p\n", peb.ImageBaseAddress);
    printf("PEB ImageProcessAffinityMask: %p\n", peb.ImageProcessAffinityMask);
    printf("PEB ImageSubsystem: %p\n", peb.ImageSubsystem);
    printf("PEB ImageSubsystemMajorVersion: %p\n", peb.ImageSubsystemMajorVersion);
    printf("PEB ImageSubsystemMinorVersion: %p\n", peb.ImageSubsystemMinorVersion);

    printf("PEB ProcessParameters: %p\n", peb.ProcessParameters);
    printf("_____________________________________________________________\n");

    return 0;
}
