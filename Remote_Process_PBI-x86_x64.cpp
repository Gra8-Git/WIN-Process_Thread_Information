#include <stdio.h>
#include <windows.h>
#include <winnt.h>
#include <TlHelp32.h>
#include"main.h" //custom file with all structures and unions and values to retrive information
#ifdef WOW64
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
typedef int (*FN_NtQueryInformationProcess) (HANDLE, int, PVOID, priv_ULONG, P_priv_ULONG);


int main(int argc, char** argv) {

    int pid, ret;
    priv_ULONG read_length;
    HINSTANCE ntdll;
    FN_NtQueryInformationProcess NtQueryInformationProcess = NULL;
    HANDLE hprocess;
    _PROCESS_BASIC_INFORMATION pbi{0};
    _PROCESS_BASIC_INFORMATION64 pbi64{0};
    SIZE_T* dwBytesRead = NULL;
   // PTR addr;


    pid = 3580; //process pid  to retrive information
    printf("Process pid :  %d", pid);

    ntdll = LoadLibrary(L"ntdll.dll");  //load liberary and get hInstance to get function
    if (ntdll == NULL) {
        puts("Unable to load ntdll");
        return 1;
    }


    NtQueryInformationProcess = (FN_NtQueryInformationProcess)GetProcAddress(ntdll, NT_QUERY_INFORMATION_PROCESS_NAME);   //request function
    if (NtQueryInformationProcess == NULL) {
        printf("Unable to get address of %s\n", NT_QUERY_INFORMATION_PROCESS_NAME);
        return 1;
    }

    hprocess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);  //open process
    if (hprocess == NULL) {
        printf("Unable to open process %d\n", pid);
        return 1;
    }

#ifdef _WIN64	  
    ZeroMemory(&pbi64, sizeof(pbi64));
    ret = NtQueryInformationProcess(hprocess, ProcessBasicInformation, &pbi64, sizeof(pbi64), &read_length);   //get pbi  from the remote process
    if (ret != 0) {
        printf("Error at %s, ret: %d\n", NT_QUERY_INFORMATION_PROCESS_NAME, ret);
        return 0;
    }
#else	
    ZeroMemory(&pbi, sizeof(pbi));
    ret = NtQueryInformationProcess(hprocess, ProcessBasicInformation, &pbi, sizeof(pbi), &read_length);   //get pbi  from the remote process
    if (ret != 0) {
        printf("Error at %s, ret: %d\n", NT_QUERY_INFORMATION_PROCESS_NAME, ret);
        return 0;
    }
#endif


#ifdef _WIN64
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
#else
    printf("\npib structure of the process PROCESS BASIC INFORMATION x86 : \n");
    printf("\n___________________________________________________________\n");
    printf("pbi PEBBASEADDRESS : %p\n", pbi.PebBaseAddress);
    printf("pbi AFFINITYMASK : %u\n", pbi.AffinityMask);
    printf("pbi BASEPRIORITY : %d\n", pbi.BasePriority);
    printf("pbi EXITSTATUS : %p\n", pbi.ExitStatus);
    printf("pbi UNIQUEPROCESSID : %p\n", pbi.UniqueProcessId);
    printf("pbi INHERITEDFROMUNIQUEPROCESSID : %p\n", pbi.InheritedFromUniqueProcessId);
    printf("_____________________________________________________________\n");
#endif
    std::getchar();
    CloseHandle(hprocess);
    return 0;
}
