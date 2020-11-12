#include <windows.h>
#include <tchar.h>
#include <cstdio>
#pragma comment(lib, "msvcr100.lib")
#define SERVICE_ACCOUNT "NT AUTHORITY\\LocalService"
// The password to the service account name
#define SERVICE_PASSWORD NULL

TCHAR *serviceName = LPTSTR("testcomm 0.2");
SERVICE_STATUS serviceStatus;
SERVICE_STATUS_HANDLE serviceStatusHandle = 0;
HANDLE stopServiceEvent = 0;

VOID ReportSvcStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint)
{
    static DWORD dwCheckPoint = 1;

    // Fill in the SERVICE_STATUS structure.

    serviceStatus.dwCurrentState = dwCurrentState;
    serviceStatus.dwWin32ExitCode = dwWin32ExitCode;
    serviceStatus.dwWaitHint = dwWaitHint;

    if (dwCurrentState == SERVICE_START_PENDING)
        serviceStatus.dwControlsAccepted = 0;
    else
        serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

    if ((dwCurrentState == SERVICE_RUNNING) ||
        (dwCurrentState == SERVICE_STOPPED))
        serviceStatus.dwCheckPoint = 0;
    else
        serviceStatus.dwCheckPoint = dwCheckPoint++;

    // Report the status of the service to the SCM.
    SetServiceStatus(serviceStatusHandle, &serviceStatus);
}

void WINAPI ServiceControlHandler(DWORD controlCode)
{
    switch (controlCode)
    {
    case SERVICE_CONTROL_INTERROGATE:
        break;

    case SERVICE_CONTROL_SHUTDOWN:
    case SERVICE_CONTROL_STOP:
        serviceStatus.dwCurrentState = SERVICE_STOP_PENDING;
        SetServiceStatus(serviceStatusHandle, &serviceStatus);

        SetEvent(stopServiceEvent);
        return;

    case SERVICE_CONTROL_PAUSE:
        break;

    case SERVICE_CONTROL_CONTINUE:
        break;

    default:

        break;
    }

    SetServiceStatus(serviceStatusHandle, &serviceStatus);
}

void WINAPI ServiceMain(DWORD /*argc*/, TCHAR * /*argv*/[])
{

    LPVOID lpDisplayBuf;

    // initialise service status
    serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    serviceStatus.dwCurrentState = SERVICE_DEMAND_START;
    serviceStatus.dwControlsAccepted = 0;
    serviceStatus.dwWin32ExitCode = 0;
    serviceStatus.dwServiceSpecificExitCode = NO_ERROR;
    serviceStatus.dwCheckPoint = 0;
    serviceStatus.dwWaitHint = 0;

    ReportSvcStatus(SERVICE_DEMAND_START, NO_ERROR, 300000);

    ReportSvcStatus(SERVICE_RUNNING, NO_ERROR, 0);

    serviceStatusHandle = RegisterServiceCtrlHandler(serviceName, ServiceControlHandler);

    if (serviceStatusHandle)
    {
        serviceStatus.dwCurrentState = SERVICE_DEMAND_START;
        SetServiceStatus(serviceStatusHandle, &serviceStatus);

        stopServiceEvent = CreateEvent(0, FALSE, FALSE, 0);

        serviceStatus.dwControlsAccepted |= (SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN);
        serviceStatus.dwCurrentState = SERVICE_RUNNING;
        SetServiceStatus(serviceStatusHandle, &serviceStatus);
        WaitForSingleObject(stopServiceEvent, 0);

        while (1)
        {

            WaitForSingleObject(stopServiceEvent, INFINITE);

            ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
            return;
        }
        serviceStatus.dwCurrentState = SERVICE_STOP_PENDING;
        SetServiceStatus(serviceStatusHandle, &serviceStatus);
        CloseHandle(stopServiceEvent);
        stopServiceEvent = 0;
        serviceStatus.dwControlsAccepted &= ~(SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN);
        serviceStatus.dwCurrentState = SERVICE_STOPPED;
        SetServiceStatus(serviceStatusHandle, &serviceStatus);
    }
}

void RunService()
{
    SERVICE_TABLE_ENTRY serviceTable[] =
        {
            {serviceName, ServiceMain},
            {0, 0}};

    StartServiceCtrlDispatcher(serviceTable);
}

void InstallService()
{
    SC_HANDLE serviceControlManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);

    if (serviceControlManager)
    {
        TCHAR path[_MAX_PATH + 1];
        if (GetModuleFileName(0, path, sizeof(path) / sizeof(path[0])) > 0)
        {
            SC_HANDLE service = CreateService(serviceControlManager,
                                              serviceName, serviceName,
                                              SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,
                                              SERVICE_AUTO_START, SERVICE_ERROR_IGNORE, path,
                                              0, 0, 0, 0, 0);
            if (service)
                CloseServiceHandle(service);
        }

        CloseServiceHandle(serviceControlManager);
    }
}

void UninstallService()
{
    SC_HANDLE serviceControlManager = OpenSCManager(0, 0, SC_MANAGER_CONNECT);

    if (serviceControlManager)
    {
        SC_HANDLE service = OpenService(serviceControlManager,
                                        serviceName, SERVICE_QUERY_STATUS | DELETE);
        if (service)
        {
            SERVICE_STATUS serviceStatus;
            if (QueryServiceStatus(service, &serviceStatus))
            {
                if (serviceStatus.dwCurrentState == SERVICE_STOPPED)
                    DeleteService(service);
            }

            CloseServiceHandle(service);
        }

        CloseServiceHandle(serviceControlManager);
    }
}

//Start service  automatically
VOID __stdcall DoStartSvc()
{
    SERVICE_STATUS_PROCESS ssStatus;
    DWORD dwOldCheckPoint;
    DWORD dwStartTickCount;
    DWORD dwWaitTime;
    DWORD dwBytesNeeded;
    //SCM_HANDLE to start service
    SC_HANDLE schSCManager;
    SC_HANDLE schService;
    // Get a handle to the SCM database.

    schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS); // full access rights

    if (NULL == schSCManager)
    {
        printf("OpenSCManager failed (%d)\n", GetLastError());
        return;
    }

    schService = OpenService(
        schSCManager,        // SCM database
        serviceName,         // name of service
        SERVICE_ALL_ACCESS); // full access

    if (schService == NULL)
    {
        printf("OpenService failed (%d)\n", GetLastError());
        CloseServiceHandle(schSCManager);
        return;
    }

    // Check the status in case the service is not stopped.

    if (!QueryServiceStatusEx(
            schService,                     // handle to service
            SC_STATUS_PROCESS_INFO,         // information level
            (LPBYTE)&ssStatus,              // address of structure
            sizeof(SERVICE_STATUS_PROCESS), // size of structure
            &dwBytesNeeded))                // size needed if buffer is too small
    {
        printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return;
    }

    if (ssStatus.dwCurrentState != SERVICE_STOPPED && ssStatus.dwCurrentState != SERVICE_STOP_PENDING)
    {
        printf("Cannot start the service because it is already running\n");
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return;
    }

    dwStartTickCount = GetTickCount();
    dwOldCheckPoint = ssStatus.dwCheckPoint;

    while (ssStatus.dwCurrentState == SERVICE_STOP_PENDING)
    {
        dwWaitTime = ssStatus.dwWaitHint / 10;

        if (dwWaitTime < 1000)
            dwWaitTime = 1000;
        else if (dwWaitTime > 10000)
            dwWaitTime = 10000;

        Sleep(dwWaitTime);

        if (!QueryServiceStatusEx(
                schService,                     // handle to service
                SC_STATUS_PROCESS_INFO,         // information level
                (LPBYTE)&ssStatus,              // address of structure
                sizeof(SERVICE_STATUS_PROCESS), // size of structure
                &dwBytesNeeded))                // size needed if buffer is too small
        {
            printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
            CloseServiceHandle(schService);
            CloseServiceHandle(schSCManager);
            return;
        }

        if (ssStatus.dwCheckPoint > dwOldCheckPoint)
        {
            dwStartTickCount = GetTickCount();
            dwOldCheckPoint = ssStatus.dwCheckPoint;
        }
        else
        {
            if (GetTickCount() - dwStartTickCount > ssStatus.dwWaitHint)
            {
                printf("Timeout waiting for service to stop\n");
                CloseServiceHandle(schService);
                CloseServiceHandle(schSCManager);
                return;
            }
        }
    }

    // Attempt to start the service.

    if (!StartService(
            schService, // handle to service
            0,          // number of arguments
            NULL))      // no arguments
    {
        printf("StartService failed (%d)\n", GetLastError());
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return;
    }
    else
        printf("Service start pending...\n");

    if (!QueryServiceStatusEx(
            schService,                     // handle to service
            SC_STATUS_PROCESS_INFO,         // info level
            (LPBYTE)&ssStatus,              // address of structure
            sizeof(SERVICE_STATUS_PROCESS), // size of structure
            &dwBytesNeeded))                // if buffer too small
    {
        printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return;
    }

    // Save the tick count and initial checkpoint.

    dwStartTickCount = GetTickCount();
    dwOldCheckPoint = ssStatus.dwCheckPoint;

    while (ssStatus.dwCurrentState == SERVICE_START_PENDING)
    {
        dwWaitTime = ssStatus.dwWaitHint / 10;

        if (dwWaitTime < 1000)
            dwWaitTime = 1000;
        else if (dwWaitTime > 10000)
            dwWaitTime = 10000;

        Sleep(dwWaitTime);

        // Check the status again.

        if (!QueryServiceStatusEx(
                schService,                     // handle to service
                SC_STATUS_PROCESS_INFO,         // info level
                (LPBYTE)&ssStatus,              // address of structure
                sizeof(SERVICE_STATUS_PROCESS), // size of structure
                &dwBytesNeeded))                // if buffer too small
        {
            printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
            break;
        }

        if (ssStatus.dwCheckPoint > dwOldCheckPoint)
        {
            // Continue to wait and check.

            dwStartTickCount = GetTickCount();
            dwOldCheckPoint = ssStatus.dwCheckPoint;
        }
        else
        {
            if (GetTickCount() - dwStartTickCount > ssStatus.dwWaitHint)
            {
                // No progress made within the wait hint.
                break;
            }
        }
    }

    // Determine whether the service is running.

    if (ssStatus.dwCurrentState == SERVICE_RUNNING)
    {
        printf("Service started successfully.\n");
    }
    else
    {
        printf("Service not started. \n");
        printf("  Current State: %d\n", ssStatus.dwCurrentState);
        printf("  Exit Code: %d\n", ssStatus.dwWin32ExitCode);
        printf("  Check Point: %d\n", ssStatus.dwCheckPoint);
        printf("  Wait Hint: %d\n", ssStatus.dwWaitHint);
    }

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
}

int _tmain(int argc, TCHAR *argv[])
{

    InstallService();
    DoStartSvc();
    RunService();

    //UninstallService();
    return 0;
}

