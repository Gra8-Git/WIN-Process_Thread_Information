#include "main.h"
#include <Windows.h>
#include <iostream>
#include <sddl.h>
#include <tchar.h>
#include <tlhelp32.h>
#include <string.h>
#include <thread>


typedef PVOID Privvalue;
typedef ULONG priv_ULONG;
typedef PULONG P_priv_ULONG;
#define  STRLENGTH   2000
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

    ntdll = LoadLibrary(_T("ntdll.dll")); //load liberary and get hInstance to get function
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

std::string WCHAR_to_String(WCHAR *wch)
{
    std::string str;
    char  ch[STRLENGTH];
    memset(ch, '\0', STRLENGTH);
    wcstombs(ch, wch, 1000);
    str=ch;
    return str;
}


void Processid_CommandLine_PEB_PBI( )
{
  HANDLE hProcessSnap={0};
  HANDLE hProcess={0};
  PROCESSENTRY32 pe32={0};
  DWORD dwPriorityClass={0};

  std::string spid={0};
  std::string spnamex={0};
  std::string spuac={0};
  std::string spdebug ={0};
  
  hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
  if( hProcessSnap == INVALID_HANDLE_VALUE)
  {
    std::cout<<"CreateToolhelp32Snapshot (of processes)";\
  
  }

  
  pe32.dwSize = sizeof( PROCESSENTRY32 );

  if( !Process32First( hProcessSnap, &pe32 ) )
  {
    std::cout<<"Process32First";
    CloseHandle( hProcessSnap );
  }

  do
  {
    
    

    HANDLE pToken = NULL;
    HANDLE hprocess = NULL;
    PVOID rtlUserProcParamsAddress={0};
    PVOID BeingDebugged={0};
    UNICODE_STRING commandLine={0};

    WCHAR *commandLineContents={0};
    std::string commandlinestr={0};
    
    PEB peb={0};
    int PID = pe32.th32ProcessID;
    spid=std::to_string(PID);

    hprocess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);

    if (IsProcessUAC(hprocess) == TRUE)
    {
        spuac= "TRUE";
    }
    else
    {
        spuac="FALSE";
    }
    if (!EnableWindowsPrivilege(TRUE, hprocess))
    {
        spdebug="FALSE";
        
    }
    else
    {
        spdebug="TRUE";
    }
#ifdef _WIN64
    _PROCESS_BASIC_INFORMATION64 pbi64 = readPibmemory64(hprocess);
    peb = readPEBmemory64(hprocess, pbi64);
    rtlUserProcParamsAddress = readRTLUserProcessParam(hprocess, pbi64.PebBaseAddress);  
    commandLine = readRTLcommand(hprocess, rtlUserProcParamsAddress);
    commandLineContents = readRTLcommandLine(hprocess, commandLine);
    commandlinestr=WCHAR_to_String(commandLineContents);
     
    spnamex = "{ \"Process_UAC\" : \""+ spuac  +"\","+"{\"Process_ID\" : \""+spid+"\", \"Process_name\" : \"" +(char*) pe32.szExeFile+"\", \"Command_line\" : \""+commandlinestr+"\"}\n";
   std::cout<<spnamex;
    

#else
    _PROCESS_BASIC_INFORMATION pbi = readPibmemory32(hprocess);
    peb = readPEBmemory32(hprocess, pbi);
    rtlUserProcParamsAddress = readRTLUserProcessParam(hprocess, pbi.PebBaseAddress);

    commandLine = readRTLcommand(hprocess, rtlUserProcParamsAddress);
    commandLineContents = readRTLcommandLine(hprocess, commandLine);
   commandlinestr=WCHAR_to_String(commandLineContents);
     
   spnamex = "{\"Process_ID\" : \""+spid+"\", \"Process_name\" : \"" +(char*) pe32.szExeFile+"\", \"Command_line\" : \""+commandlinestr+"\"}";
   std::cout<<spnamex;
#endif
    

  } while( Process32Next( hProcessSnap, &pe32 ) );

  CloseHandle( hProcessSnap );
 // return( TRUE );
}



int main(void)
{
std::thread t1{Processid_CommandLine_PEB_PBI};
t1.join();
    return 0;
}
