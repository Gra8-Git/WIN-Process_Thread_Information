#include <list>
#include <iostream>
#include <Windows.h>

#include <Tlhelp32.h>
#include <atomic>
#include <thread>
#include <chrono>
#include <tchar.h>

#define NT_SUSPEND_PROCESS "NtSuspendProcess"
#define NT_RESUME_PROCESS "NtResumeProcess"

#define  SLENGTH   2000




typedef LONG (NTAPI *NtSuspendProcess)(IN HANDLE ProcessHandle);
typedef LONG (NTAPI *NtResumeProcess)(IN HANDLE ProcessHandle);

//CONVERT WCHAR TO STRING
std::string WCHAR_to_String(WCHAR *wch)
{
    std::string str;
    //convert from WCHAR
    char  ch[SLENGTH];
    memset(ch, '\0', SLENGTH);
    wcstombs(ch, wch, 2000);
    str=ch;
    return str;
}


//ENUMPROCESS LIST
DWORD ToolHelp_EnumProcesses(std::list<PROCESSENTRY32>& listProcessInfo)
{
    DWORD dwRet = NO_ERROR;
    listProcessInfo.clear();
    //enum process snapshots
    DWORD dwFlags = TH32CS_SNAPPROCESS;
    HANDLE hSnapshot = ::CreateToolhelp32Snapshot(dwFlags, 0);
    if (INVALID_HANDLE_VALUE == hSnapshot)
    {
        return ::GetLastError();
    }
    PROCESSENTRY32 processEntry = {0};
    processEntry.dwSize = sizeof(PROCESSENTRY32);
    
    if (::Process32First(hSnapshot, &processEntry))
    {
        do
        {
            listProcessInfo.push_back(processEntry);
        } while (::Process32Next(hSnapshot, &processEntry));
    }
    else
    {
        dwRet = ::GetLastError();
    }
    ::CloseHandle(hSnapshot);
    return dwRet;
}
//SUSPEND RESUME PROCESS
void suspend_process(DWORD processId, bool isEnable)
{
    HINSTANCE ntdll;
    NtSuspendProcess pfnNtSuspendProcess=NULL;

    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    ntdll = LoadLibrary(_T("ntdll.dll")); //load liberary and get hInstance to get function
    if (ntdll == NULL)
    {
        std::cout << "Unable to load ntdll\n";
    }
    pfnNtSuspendProcess  = (NtSuspendProcess)GetProcAddress(ntdll, NT_SUSPEND_PROCESS); //request function
    if (pfnNtSuspendProcess == NULL)
    {
        std::cout << "Unable to get address of " << NT_SUSPEND_PROCESS << "\n";
    }

    NtResumeProcess pfnNtResumeProcess  = (NtResumeProcess)GetProcAddress(ntdll, NT_RESUME_PROCESS); //request function
    if (pfnNtResumeProcess == NULL)
    {
        std::cout << "Unable to get address of " << NT_RESUME_PROCESS << "\n";
    }

    if (isEnable)
    {pfnNtSuspendProcess(processHandle);}
    else
    {pfnNtResumeProcess(processHandle);}
    
    CloseHandle(processHandle);
}

void main_function( std::atomic<bool>& program_is_running, unsigned int millisec )
{

    const auto wait_duration = std::chrono::milliseconds(millisec) ;
    while(program_is_running )
    {
        
    std::list<PROCESSENTRY32> listProcessInfo;
    DWORD dwRet = ToolHelp_EnumProcesses(listProcessInfo);
    if(NO_ERROR == dwRet)
    {
        const std::list<PROCESSENTRY32>::const_iterator end = listProcessInfo.end();
        std::list<PROCESSENTRY32>::const_iterator iter = listProcessInfo.begin();
        for (; iter != end; ++iter)
        {
            const PROCESSENTRY32& processEntry = *iter;


            // NOTE: UNICODE & _UNICODE preprocessor constants are defined in this project
            std::wcout << L"Process ID: " << processEntry.th32ProcessID << std::endl;
            std::wcout << L"Exe file name: " << processEntry.szExeFile << std::endl;
            std::wcout << L"Parent process ID: " << processEntry.th32ParentProcessID << std::endl;
            std::wcout << L"Threads count: " << processEntry.cntThreads << std::endl;
        }
    }
    else
    {
        std::cout << "ToolHelp_EnumProcesses falied. Error: " << dwRet << std::endl;
      
    }

        std::this_thread::sleep_for(wait_duration) ; 
    }
}


int main()
{
 std::cout << "*** press enter to exit the program gracefully\n\n" ;

    std::atomic<bool> running { true } ;
    const unsigned int millisec = 10 ; // update after every 50 milliseconds
    std::thread update_thread( main_function, std::ref(running), millisec ) ;

    // do other stuff in parallel: simulated below
    std::cin.get() ;

    // exit gracefully
    running = false ;
    update_thread.join() ;
	return 0;
}
