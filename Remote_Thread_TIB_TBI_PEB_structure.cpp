#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <TlHelp32.h>
//#include <winnt.h>
//#include"main.h"
typedef LONG NTSTATUS;
typedef DWORD KPRIORITY;
//typedef DWORD PPEB;
//typedef PVOID _PEB;
typedef WORD UWORD;
//typedef BYTE PPEB;
//typedef BYTE _PEB;
//typedef LONG PRTL_USER_PROCESS_PARAMETERS;
//typedef  DWORD PPEB_LDR_DATA;
//to remove

using namespace std;

typedef struct _UNICODE_STRING
{
    USHORT Length;
    USHORT MaximumLength;
    PWSTR Buffer;
} UNICODE_STRING, * PUNICODE_STRING;
typedef const UNICODE_STRING* PCUNICODE_STRING;


#define RTL_MAX_DRIVE_LETTERS 32
#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)
#define NT_ERROR(Status) ((((ULONG)(Status)) >> 30) == 3)
#define GDI_HANDLE_BUFFER_SIZE32 34
#define GDI_HANDLE_BUFFER_SIZE64 60


#ifndef _WIN64
#define GDI_HANDLE_BUFFER_SIZE GDI_HANDLE_BUFFER_SIZE32
#else
#define GDI_HANDLE_BUFFER_SIZE GDI_HANDLE_BUFFER_SIZE64
#endif

typedef ULONG GDI_HANDLE_BUFFER32[GDI_HANDLE_BUFFER_SIZE32];
typedef ULONG GDI_HANDLE_BUFFER64[GDI_HANDLE_BUFFER_SIZE64];
typedef ULONG GDI_HANDLE_BUFFER[GDI_HANDLE_BUFFER_SIZE];



// START
typedef struct _CLIENT_ID
{
    PVOID UniqueProcess;
    PVOID UniqueThread;
} CLIENT_ID, * PCLIENT_ID;

typedef struct _TEB {
    NT_TIB NtTib;
    PVOID EnvironmentPointer;
    CLIENT_ID ClientId;
    PVOID ActiveRpcHandle;
    PVOID ThreadLocalStoragePointer;
    PVOID ProcessEnvironmentBlock;
} TEB, *PTEB;


typedef struct _THREAD_BASIC_INFORMATION
{
    NTSTATUS                ExitStatus;
    PVOID                   TebBaseAddress;
    CLIENT_ID               ClientId;
    KAFFINITY               AffinityMask;
    KPRIORITY               Priority;
    KPRIORITY               BasePriority;
} THREAD_BASIC_INFORMATION, * PTHREAD_BASIC_INFORMATION;

enum THREADINFOCLASS
{
    ThreadBasicInformation,
};

// Get TEB BASE ADDRESS END

typedef struct _PEB {
    BYTE Reserved1[2];
    BYTE BeingDebugged;
    BYTE Reserved2[1];
    PVOID Reserved3[2];
    PVOID Ldr;
    PVOID ProcessParameters;
    PVOID Reserved4[3];
    PVOID AtlThunkSListPtr;
    PVOID Reserved5;
    ULONG Reserved6;
    PVOID Reserved7;
    ULONG Reserved8;
} PEB, *PPEB;

typedef struct _RTL_USER_PROCESS_PARAMETERS {
    BYTE Reserved1[16];
    PVOID Reserved2[10];
    UNICODE_STRING ImagePathName;
    UNICODE_STRING CommandLine;
} RTL_USER_PROCESS_PARAMETERS, * PRTL_USER_PROCESS_PARAMETERS;

typedef struct _PEB_LDR_DATA {
    BYTE Reserved1[8];
    PVOID Reserved2[3];
    LIST_ENTRY InMemoryOrderModuleList;
} PEB_LDR_DATA, * PPEB_LDR_DATA;

typedef struct _LDR_DATA_TABLE_ENTRY {
    PVOID Reserved1[2];
    LIST_ENTRY InMemoryOrderLinks;
    PVOID Reserved2[2];
    PVOID BaseAddress;
    PVOID Reserved3[2];
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;
    BYTE Reserved4[8];
    PVOID Reserved5[3];
#pragma warning(push)
#pragma warning(disable: 4201) // we'll always use the Microsoft compiler
    union {
        ULONG CheckSum;
        PVOID Reserved6;
    } DUMMYUNIONNAME;
#pragma warning(pop)
    ULONG TimeDateStamp;
} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;




THREAD_BASIC_INFORMATION GetThreadStackTopAddress(HANDLE hProcess, HANDLE hThread)
{
    bool loadedManually = false;

        NT_TIB tib = { 0 };
        TEB teb;
        THREAD_BASIC_INFORMATION tbi1 = { 0 };
    HMODULE module = GetModuleHandle(L"ntdll.dll");
    if (!module)
    {
        module = LoadLibrary(L"ntdll.dll");
        loadedManually = true;
    }

    NTSTATUS(__stdcall * NtQueryInformationThread)(HANDLE ThreadHandle, THREADINFOCLASS ThreadInformationClass, PVOID ThreadInformation, ULONG ThreadInformationLength, PULONG ReturnLength);
    NtQueryInformationThread = reinterpret_cast<decltype(NtQueryInformationThread)>(GetProcAddress(module, "NtQueryInformationThread"));

    if (NtQueryInformationThread)
    {


        NTSTATUS status = NtQueryInformationThread(hThread, ThreadBasicInformation, &tbi1, sizeof(tbi1), nullptr);
        if (status >= 0)
        {
            ReadProcessMemory(hProcess, tbi1.TebBaseAddress, &tib, sizeof(tib), nullptr);

            if (loadedManually)
            {
                FreeLibrary(module);
            }
            return tbi1;

           

        }
    }


    if (loadedManually)
    {
        FreeLibrary(module);
    }

    return tbi1;
}



HANDLE GetMainThreadId(DWORD cPID)    //scan all threads and find main handler of the thread and id
{
   
    const std::shared_ptr<void> hThreadSnapshot(CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0), CloseHandle);
    if (hThreadSnapshot.get() == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("Get Thread Id : failed");
    }
    THREADENTRY32 TE32;
    TE32.dwSize = sizeof(THREADENTRY32);
    DWORD result = 0;
    HANDLE hthread;

    for (BOOL success = Thread32First(hThreadSnapshot.get(), &TE32);
        !result && success && GetLastError() != ERROR_NO_MORE_FILES;
        success = Thread32Next(hThreadSnapshot.get(), &TE32))
    {
        if (TE32.th32OwnerProcessID == cPID) {
            result = TE32.th32ThreadID;
            printf("Thread ID: %u\n", result);
        }
    }
    return OpenThread(READ_CONTROL | THREAD_QUERY_INFORMATION,FALSE,result);
}



int main()
{
    HANDLE hprocess;
    int pid =7556;
    THREAD_BASIC_INFORMATION tebBaseAddr;                                      //  Thread Information Block
    hprocess = OpenProcess(PROCESS_ALL_ACCESS, true, pid);
   
    tebBaseAddr = GetThreadStackTopAddress(hprocess, GetMainThreadId(pid));
    printf("\n\t\tTBI structure :\n");
    printf("TBI BASE ADDR: %p\n",tebBaseAddr.TebBaseAddress);                //   Thread Environment Block
    printf("TBI AFFINITY MASK: %llu\n", tebBaseAddr.AffinityMask);
    printf("TBI PRIORITY: %u\n", tebBaseAddr.Priority);
    printf("TBI BASE PRIORITY: %u\n", tebBaseAddr.BasePriority);
    printf("TBI CLIENTID: %u\n", tebBaseAddr.ClientId);
    printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    TEB teb{0};
    
    ReadProcessMemory(hprocess,tebBaseAddr.TebBaseAddress,&teb,sizeof(TEB),0 );
    

    printf("\n\t\tTEB structure :\n");
    printf("\nTEB Process Enviroment Block %p\n",teb.ProcessEnvironmentBlock);
    printf("TEB Active RPC handle %p\n", teb.ActiveRpcHandle);
    printf("TEB Client ID %u\n", teb.ClientId);
    printf("TEB Thread Local storage pointer %p\n", teb.ThreadLocalStoragePointer);
    printf("TEB NT TIB %p\n", teb.NtTib);
    printf("TEB Enviroment Pointer %p\n", teb.EnvironmentPointer);
    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");

    PEB peb;

    ReadProcessMemory(hprocess, teb.ProcessEnvironmentBlock, &peb, sizeof(PEB), 0);

    printf("\n\t\tPEB structure :\n");
    printf("\nPEB AlLTHUNKSLISTPTR %p\n", peb.AtlThunkSListPtr);
    printf("PEB begin debug %p\n", peb.Ldr);
    printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");



    return 0;
}
