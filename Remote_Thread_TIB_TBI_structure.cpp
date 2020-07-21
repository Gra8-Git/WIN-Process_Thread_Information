#include <iostream>
#include <windows.h>
#include <TlHelp32.h>
typedef LONG NTSTATUS;
typedef DWORD KPRIORITY;
typedef WORD UWORD;


// START
typedef struct _CLIENT_ID
{
    PVOID UniqueProcess;
    PVOID UniqueThread;
} CLIENT_ID, * PCLIENT_ID;

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


THREAD_BASIC_INFORMATION GetThreadStackTopAddress(HANDLE hProcess, HANDLE hThread)
{
    bool loadedManually = false;

        NT_TIB tib = { 0 };
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
            ReadProcessMemory(hProcess, tbi1.TebBaseAddress, &tib, sizeof(tbi1), nullptr);

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
    THREAD_BASIC_INFORMATION tebBaseAddr;
    hprocess = OpenProcess(PROCESS_ALL_ACCESS, true, pid);
   
    tebBaseAddr = GetThreadStackTopAddress(hprocess, GetMainThreadId(pid));
    printf("TEB BASE ADDR:   %p\n",tebBaseAddr.TebBaseAddress);
    printf("TEB AFFINITY MASK:   %u\n", tebBaseAddr.AffinityMask);
    printf("TEB PRIORITY:   %u\n", tebBaseAddr.Priority);
    printf("TEB BASE PRIORITY:   %u\n", tebBaseAddr.BasePriority);
    printf("TEB CLIENTID:  %u\n", tebBaseAddr.ClientId);
      
    return 0;
}
