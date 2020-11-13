#include <windows.h>
#include<iostream>
using namespace std;

TCHAR *service_name = LPTSTR("testc 0.1");
SERVICE_STATUS Service_Status;
SERVICE_STATUS_HANDLE Service_Status_Handle = 0;
HANDLE Stop_Service_Event = 0;
//CREATE SERVICE SC -Manager database
void Create_Service()
{
    SC_HANDLE Open_Control_Manager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
    if (Open_Control_Manager)
    {
        TCHAR path[_MAX_PATH + 1];
        if (GetModuleFileName(0, path, sizeof(path) / sizeof(path[0])) > 0)     //Get path of the application to create on his base service 
        {
            SC_HANDLE service = CreateService(Open_Control_Manager, service_name, service_name,
                                              SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,
                                              SERVICE_AUTO_START, SERVICE_ERROR_IGNORE, path,
                                              0, 0, 0, 0, 0);
            if (service)
            {
                CloseServiceHandle(service);
            }
            else
            {
            DWORD dwError = GetLastError();   //Get all errors for CreateService() 
            switch (dwError) 
            {
                    case ERROR_ACCESS_DENIED:
                std::cout<<" Create Service Object and add to SC_manager database (SC_Manager) ERROR: Handle haven't  ACCESS RIGHTS."<<(int)dwError<<"\n"; 
                CloseServiceHandle(Open_Control_Manager);
                break;
                    case ERROR_CIRCULAR_DEPENDENCY:
                std::cout<<" Create Service Object and add to SC_manager database (SC_Manager) ERROR: A circular service dependency was specified."<<(int)dwError<<"\n"; 
                CloseServiceHandle(Open_Control_Manager);
                break;
                    case ERROR_DUPLICATE_SERVICE_NAME:
                std::cout<<" Create Service Object and add to SC_manager database (SC_Manager) ERROR: Service Name already exists in the service control manager database."<<(int)dwError<<"\n"; 
                CloseServiceHandle(Open_Control_Manager);
                break;
                    case ERROR_INVALID_HANDLE:
                std::cout<<" Create Service Object and add to SC_manager database (SC_Manager) ERROR: Handler to the specific control manager database is invalid."<<(int)dwError<<"\n"; 
                CloseServiceHandle(Open_Control_Manager);
                break;
                    case ERROR_INVALID_NAME :
                std::cout<<" Create Service Object and add to SC_manager database (SC_Manager) ERROR: The specific service name is invalid."<<(int)dwError<<"\n"; 
                CloseServiceHandle(Open_Control_Manager);
                break;
                    case ERROR_INVALID_PARAMETER :
                std::cout<<" Create Service Object and add to SC_manager database (SC_Manager) ERROR: A parameter that was specified is invalid."<<(int)dwError<<"\n"; 
                CloseServiceHandle(Open_Control_Manager);
                break;
                    case ERROR_INVALID_SERVICE_ACCOUNT :
                std::cout<<" Create Service Object and add to SC_manager database (SC_Manager) ERROR: The user account name specified in the lpServiceStartName parameter does not exist."<<(int)dwError<<"\n"; 
                CloseServiceHandle(Open_Control_Manager);
                break;
                    case ERROR_SERVICE_EXISTS :
                std::cout<<" Create Service Object and add to SC_manager database (SC_Manager) ERROR: The specified service already exists in this database."<<(int)dwError<<"\n"; 
                CloseServiceHandle(Open_Control_Manager);
                break;
                    case ERROR_SERVICE_MARKED_FOR_DELETE :
                std::cout<<" Create Service Object and add to SC_manager database (SC_Manager) ERROR: The specified service already exists in this database and has been marked for deletion."<<(int)dwError<<"\n"; 
                CloseServiceHandle(Open_Control_Manager);
                break;
                    default:
                std::cout<<" Create Service Object and add to SC_manager database (SC_Manager) ERROR: UNKOWN ERROR "<<(int)dwError<<"\n"; 
                     CloseServiceHandle(Open_Control_Manager);
            }
            }
        }
        CloseServiceHandle(Open_Control_Manager);
    }
    else
    {
    DWORD dwError = GetLastError();  //Get all errors for OpenSCManager()
            switch (dwError) 
            {
                    case ERROR_ACCESS_DENIED:
                std::cout<<" Connection to Service Manager (SC_Manager) ERROR: Access Deny"<<(int)dwError<<"\n"; 
                break;
                    case ERROR_DATABASE_DOES_NOT_EXIST:
                std::cout<<" Connection to Service Manager (SC_Manager) ERROR: DATABSE OF SERVICE MANAGER NOT EXISTS"<<(int)dwError<<"\n"; 
                break;
                    case ERROR_INVALID_PARAMETER:
                std::cout<<" Connection to Service Manager (SC_Manager) ERROR: INVALID PARAMETERS"<<(int)dwError<<"\n";      
                break;
                    default:
                std::cout<<" Connection to Service Manager (SC_Manager) ERROR: UNKOWN ERROR "<<(int)dwError<<"\n";      

            }
    }
    std::cout<<"Service was created :"<<service_name<<"\n";
}
//UNINSTALL SERVICE SC -Manager database
void Uninstall_Service()
{
    SC_HANDLE Open_Control_Manager = OpenSCManager(0, 0, SC_MANAGER_CONNECT);

    if (Open_Control_Manager)
    {
        SC_HANDLE service = OpenService(Open_Control_Manager,service_name, SERVICE_QUERY_STATUS | DELETE); //return handler of the service from SC-manager database
        if (service)
        {
            SERVICE_STATUS serviceStatus;
            if (QueryServiceStatus(service, &serviceStatus)) //Retrieves the current status of the specified service.
            {
            switch (serviceStatus.dwCurrentState) 
            {
                    case SERVICE_STOPPED:
                    if(DeleteService(service)){
                std::cout<<"Service was removed from SC manager:"<<service_name<<" \n"; }else{
                    std::cout<<"Can't  removed service from SC manager:"<<service_name<<" \n"; 
                }
                    case SERVICE_STOP_PENDING:
                std::cout<<"The service is stopping:"<<service_name<<" \n"; 
                    case SERVICE_START_PENDING:
                std::cout<<"The service is starting. :"<<service_name<<" \n"; 
                    case SERVICE_RUNNING:
                std::cout<<"The service is running.:"<<service_name<<" \n"; 
                    case SERVICE_PAUSED:
                std::cout<<"The service is paused. :"<<service_name<<" \n"; 
                    case SERVICE_PAUSE_PENDING:
                std::cout<<"The service pause is pending. :"<<service_name<<" \n"; 
                    case SERVICE_CONTINUE_PENDING:
                std::cout<<"The service continue is pending. :"<<service_name<<" \n"; 
                    default:
                std::cout<<" UNKNOWN STATUS OF THE SERVICE TRY TO REMOVE IT MANUALY with command line.\n";      
            }
            }
            else
            {
                   DWORD dwError = GetLastError();  //Get all errors for OpenSCManager()
            switch (dwError) 
            {
                    case ERROR_ACCESS_DENIED:
                std::cout<<" Query Service Status (SC_Manager) ERROR: The handle does not have the SERVICE_QUERY_STATUS access right."<<(int)dwError<<"\n"; 
                break;
                    case ERROR_INVALID_HANDLE:
                std::cout<<" Query to Service Status (SC_Manager) ERROR: The handle is invalid."<<(int)dwError<<"\n"; 
                break;
                    default:
                std::cout<<" Query to Service Status (SC_Manager) ERROR: UNKOWN ERROR "<<(int)dwError<<"\n";
            }
            }
            CloseServiceHandle(service);
        }
        else
        {
            DWORD dwError = GetLastError();  //Get all errors for OpenSCManager()
            switch (dwError) 
            {
                    case ERROR_ACCESS_DENIED:
                std::cout<<" Open Service (SC_Manager) ERROR: Access Deny."<<(int)dwError<<"\n"; 
                break;
                    case ERROR_SERVICE_DOES_NOT_EXIST:
                std::cout<<" Open Service (SC_Manager) ERROR: SERVICE DOSE NOT EXIST."<<(int)dwError<<"\n"; 
                break;
                    case ERROR_INVALID_HANDLE:
                std::cout<<" Open Service (SC_Manager) ERROR: INVALID HANDLER. "<<(int)dwError<<"\n";      
                break;
                    case ERROR_INVALID_NAME:
                std::cout<<" Open Service (SC_Manager) ERROR: Specific Service name is invalid. "<<(int)dwError<<"\n";      
                break;
                    default:
                std::cout<<" Open Service (SC_Manager) ERROR: UNKOWN ERROR. "<<(int)dwError<<"\n";      

            }
        }

        CloseServiceHandle(Open_Control_Manager);
    }
        else
    {
    DWORD dwError = GetLastError();  //Get all errors for OpenSCManager()
            switch (dwError) 
            {
                    case ERROR_ACCESS_DENIED:
                std::cout<<" Connection to Service Manager (SC_Manager) ERROR: Access Deny"<<(int)dwError<<"\n"; 
                break;
                    case ERROR_DATABASE_DOES_NOT_EXIST:
                std::cout<<" Connection to Service Manager (SC_Manager) ERROR: DATABSE OF SERVICE MANAGER NOT EXISTS"<<(int)dwError<<"\n"; 
                break;
                    case ERROR_INVALID_PARAMETER:
                std::cout<<" Connection to Service Manager (SC_Manager) ERROR: INVALID PARAMETERS"<<(int)dwError<<"\n";      
                break;
                    default:
                std::cout<<" Connection to Service Manager (SC_Manager) ERROR: UNKOWN ERROR "<<(int)dwError<<"\n";      

            }
    }
    std::cout<<"Service removed :"<<service_name<<"\n";
}
//STOP SERVICE  SC -Manager database
void Stop_Service()
{
    SERVICE_STATUS_PROCESS serviceStatus;
    DWORD dwOldCheckPoint;
    DWORD dwStartTickCount;
    DWORD dwWaitTime;
    DWORD dwBytesNeeded;
    SC_HANDLE Open_Control_Manager = OpenSCManager(0, 0, SC_MANAGER_CONNECT);

    if (Open_Control_Manager)
    {
        SC_HANDLE service = OpenService(Open_Control_Manager,service_name, SERVICE_STOP |  SERVICE_QUERY_STATUS |  SERVICE_ENUMERATE_DEPENDENTS ); //return handler of the service from SC-manager database
        if (service)
        {
            SERVICE_STATUS serviceStatus;
            if (QueryServiceStatus(service, &serviceStatus)) //Retrieves the current status of the specified service.
            {
            switch (serviceStatus.dwCurrentState) 
            {
                    case SERVICE_STOPPED:
                std::cout<<"Service was stoped :"<<service_name<<" \n"; 
                    case SERVICE_STOP_PENDING:
                std::cout<<"The service is stopping:"<<service_name<<" \n"; 
                dwStartTickCount = GetTickCount();
                dwOldCheckPoint = serviceStatus.dwCheckPoint;
                while (serviceStatus.dwCurrentState == SERVICE_STOP_PENDING)
    {
        dwWaitTime = serviceStatus.dwWaitHint / 10;

        if (dwWaitTime < 1000)
            dwWaitTime = 1000;
        else if (dwWaitTime > 10000)
            dwWaitTime = 10000;

        Sleep(dwWaitTime);

        if (!QueryServiceStatusEx(
                service,                     // handle to service
                SC_STATUS_PROCESS_INFO,         // information level
                (LPBYTE)&serviceStatus,              // address of structure
                sizeof(SERVICE_STATUS_PROCESS), // size of structure
                &dwBytesNeeded))                // size needed if buffer is too small
        {
            printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
            CloseServiceHandle(service);
            CloseServiceHandle(Open_Control_Manager);
            return;
        }

        if (serviceStatus.dwCheckPoint > dwOldCheckPoint)
        {
            dwStartTickCount = GetTickCount();
            dwOldCheckPoint = serviceStatus.dwCheckPoint;
        }
        else
        {
            if (GetTickCount() - dwStartTickCount > serviceStatus.dwWaitHint)
            {
                printf("Timeout waiting for service to stop\n");
                CloseServiceHandle(service);
                CloseServiceHandle(Open_Control_Manager);
                return;
            }
        }
    }
                if (!ControlService(service,SERVICE_CONTROL_STOP,(LPSERVICE_STATUS) &serviceStatus))// send code to stop service
                    {
                    std::cout<<"ControlService failed "<< GetLastError()<<"\n";
                    }
                    else
                    {
                    std::cout<<"Control Service Stop the service\n";
                    }

                    case SERVICE_START_PENDING:
                std::cout<<"The service is starting. :"<<service_name<<" \n"; 
                    case SERVICE_RUNNING:
                std::cout<<"The service is running.:"<<service_name<<" \n"; 
                    case SERVICE_PAUSED:
                std::cout<<"The service is paused. :"<<service_name<<" \n"; 
                    case SERVICE_PAUSE_PENDING:
                std::cout<<"The service pause is pending. :"<<service_name<<" \n"; 
                    case SERVICE_CONTINUE_PENDING:
                std::cout<<"The service continue is pending. :"<<service_name<<" \n"; 
                    default:
                std::cout<<" UNKNOWN STATUS OF THE SERVICE TRY TO REMOVE IT MANUALY with command line.\n";      
            }
            }
            else
            {
                   DWORD dwError = GetLastError();  //Get all errors for OpenSCManager()
            switch (dwError) 
            {
                    case ERROR_ACCESS_DENIED:
                std::cout<<" Query Service Status (SC_Manager) ERROR: The handle does not have the SERVICE_QUERY_STATUS access right."<<(int)dwError<<"\n"; 
                break;
                    case ERROR_INVALID_HANDLE:
                std::cout<<" Query to Service Status (SC_Manager) ERROR: The handle is invalid."<<(int)dwError<<"\n"; 
                break;
                    default:
                std::cout<<" Query to Service Status (SC_Manager) ERROR: UNKOWN ERROR "<<(int)dwError<<"\n";
            }
            }
            CloseServiceHandle(service);
        }
        else
        {
            DWORD dwError = GetLastError();  //Get all errors for OpenSCManager()
            switch (dwError) 
            {
                    case ERROR_ACCESS_DENIED:
                std::cout<<" Open Service (SC_Manager) ERROR: Access Deny."<<(int)dwError<<"\n"; 
                break;
                    case ERROR_SERVICE_DOES_NOT_EXIST:
                std::cout<<" Open Service (SC_Manager) ERROR: SERVICE DOSE NOT EXIST."<<(int)dwError<<"\n"; 
                break;
                    case ERROR_INVALID_HANDLE:
                std::cout<<" Open Service (SC_Manager) ERROR: INVALID HANDLER. "<<(int)dwError<<"\n";      
                break;
                    case ERROR_INVALID_NAME:
                std::cout<<" Open Service (SC_Manager) ERROR: Specific Service name is invalid. "<<(int)dwError<<"\n";      
                break;
                    default:
                std::cout<<" Open Service (SC_Manager) ERROR: UNKOWN ERROR. "<<(int)dwError<<"\n";      

            }
        }
        CloseServiceHandle(Open_Control_Manager);
    }
        else
    {
    DWORD dwError = GetLastError();  //Get all errors for OpenSCManager()
            switch (dwError) 
            {
                    case ERROR_ACCESS_DENIED:
                std::cout<<" Connection to Service Manager (SC_Manager) ERROR: Access Deny"<<(int)dwError<<"\n"; 
                break;
                    case ERROR_DATABASE_DOES_NOT_EXIST:
                std::cout<<" Connection to Service Manager (SC_Manager) ERROR: DATABSE OF SERVICE MANAGER NOT EXISTS"<<(int)dwError<<"\n"; 
                break;
                    case ERROR_INVALID_PARAMETER:
                std::cout<<" Connection to Service Manager (SC_Manager) ERROR: INVALID PARAMETERS"<<(int)dwError<<"\n";      
                break;
                    default:
                std::cout<<" Connection to Service Manager (SC_Manager) ERROR: UNKOWN ERROR "<<(int)dwError<<"\n";      

            }
    }
}
//START SERVICE  SC -Manager database
void Start_Service()
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
        service_name,         // name of service
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
//REPORT SERVICE STATUS IN SC -MANAGER (FUNCTION FOR SERVICE MAIN)
void ReportServiceStatus( DWORD dwCurrentState,DWORD dwWin32ExitCode, DWORD dwWaitHint)
{
    static DWORD dwCheckPoint = 1;
    // Fill in the SERVICE_STATUS structure.
    Service_Status.dwCurrentState = dwCurrentState;
    Service_Status.dwWin32ExitCode = dwWin32ExitCode;
    Service_Status.dwWaitHint = dwWaitHint;
    if (dwCurrentState == SERVICE_START_PENDING)
        Service_Status.dwControlsAccepted = 0;
    else Service_Status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    if ( (dwCurrentState == SERVICE_RUNNING) ||
           (dwCurrentState == SERVICE_STOPPED) )
        Service_Status.dwCheckPoint = 0;
    else Service_Status.dwCheckPoint = dwCheckPoint++;
    
    if(SetServiceStatus( Service_Status_Handle, &Service_Status ))// Report the status of the service to the SCM.
{
std::cout<<"Service stattus was updated"<<Service_Status.dwCurrentState<<"\n";
}
else
{
            DWORD dwError = GetLastError();  //Get all errors for OpenSCManager()
            switch (dwError) 
            {
                    case ERROR_INVALID_DATA:
                std::cout<<" Set Service Status Report (SC_Manager) ERROR: The specified service status structure is invalid."<<(int)dwError<<"\n"; 
                break;
                    case ERROR_INVALID_HANDLE:
                std::cout<<" Set Service Status Report (SC_Manager) ERROR: The specified handle is invalid."<<(int)dwError<<"\n"; 
                break;
                    default:
                std::cout<<" Set Service Status Report (SC_Manager) ERROR: UNKOWN ERROR. "<<(int)dwError<<"\n";      
            }
}

}
//HANDLER FOR SERVICE STATUS FOR SC -MANAGER (FUNCTION FOR SERVICE MAIN)
void WINAPI Service_Control_Handler(DWORD controlCode)
{
    switch (controlCode)
    {
    case SERVICE_CONTROL_INTERROGATE:
        break;
    case SERVICE_CONTROL_SHUTDOWN:
        std::cout<<"Service ShutDown";
        Service_Status.dwWin32ExitCode = 0;
        Service_Status.dwCurrentState =SERVICE_STOPPED;
        SetServiceStatus(Service_Status_Handle, &Service_Status);
        SetEvent(Stop_Service_Event);
    case SERVICE_CONTROL_STOP:
        std::cout<<"Service Stoped\n";
        Service_Status.dwWin32ExitCode = 0;
        Service_Status.dwCurrentState =SERVICE_STOPPED;
        SetServiceStatus(Service_Status_Handle, &Service_Status);
        SetEvent(Stop_Service_Event);
    case SERVICE_CONTROL_PAUSE:
        std::cout<<"Service paused\n";
        Service_Status.dwCurrentState =SERVICE_PAUSED;
        SetServiceStatus(Service_Status_Handle, &Service_Status);
        SetEvent(Stop_Service_Event);
    case SERVICE_CONTROL_CONTINUE:
        std::cout<<"Service Running\n";
        Service_Status.dwCurrentState =SERVICE_RUNNING;
        SetServiceStatus(Service_Status_Handle, &Service_Status);
        SetEvent(Stop_Service_Event);
    default:
        break;
    }

    if(SetServiceStatus(Service_Status_Handle, &Service_Status))
{
std::cout<<"Service stattus was updated"<<Service_Status.dwCurrentState<<"\n";
}
else
{
            DWORD dwError = GetLastError();  //Get all errors for OpenSCManager()
            switch (dwError) 
            {
                    case ERROR_INVALID_DATA:
                std::cout<<" Set Service Status Report (SC_Manager) ERROR: The specified service status structure is invalid."<<(int)dwError<<"\n"; 
                break;
                    case ERROR_INVALID_HANDLE:
                std::cout<<" Set Service Status Report (SC_Manager) ERROR: The specified handle is invalid."<<(int)dwError<<"\n"; 
                break;
                    default:
                std::cout<<" Set Service Status Report (SC_Manager) ERROR: UNKOWN ERROR. "<<(int)dwError<<"\n";      
            }
}

}
//SERVICE MAIN FUNCTION 
void WINAPI ServiceMain(DWORD argc, TCHAR *  argv[])
{
        // initialise service status
    Service_Status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    Service_Status.dwCurrentState = SERVICE_DEMAND_START;
    Service_Status.dwControlsAccepted = 0;
    Service_Status.dwWin32ExitCode = 0;
    Service_Status.dwServiceSpecificExitCode = NO_ERROR;
    Service_Status.dwCheckPoint = 0;
    Service_Status.dwWaitHint = 0;

    ReportServiceStatus(SERVICE_DEMAND_START, NO_ERROR, 300000);
    ReportServiceStatus(SERVICE_RUNNING, NO_ERROR, 0);

    
    Service_Status_Handle = RegisterServiceCtrlHandler(service_name, Service_Control_Handler);


    if (Service_Status_Handle)
    {
        Service_Status.dwCurrentState = SERVICE_DEMAND_START;
        SetServiceStatus(Service_Status_Handle, &Service_Status);

        Stop_Service_Event = CreateEvent(0, FALSE, FALSE, 0);

        Service_Status.dwControlsAccepted |= (SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN);
        Service_Status.dwCurrentState = SERVICE_RUNNING;
        SetServiceStatus(Service_Status_Handle, &Service_Status);
        WaitForSingleObject(Stop_Service_Event, 0);

        while (1)
        {

            WaitForSingleObject(Stop_Service_Event, INFINITE);

            ReportServiceStatus(SERVICE_STOPPED, NO_ERROR, 0);
            return;
        }
        Service_Status.dwCurrentState = SERVICE_STOP_PENDING;
        SetServiceStatus(Service_Status_Handle, &Service_Status);
        CloseHandle(Stop_Service_Event);
        Stop_Service_Event = 0;
        Service_Status.dwControlsAccepted &= ~(SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN);
        Service_Status.dwCurrentState = SERVICE_STOPPED;
        SetServiceStatus(Service_Status_Handle, &Service_Status);
    }

}

int main(int argc, TCHAR *argv[])
{
   if( lstrcmpi( argv[1], TEXT("install")) == 0 )
    {
    Create_Service();    
    }
   if( lstrcmpi( argv[1], TEXT("start")) == 0 )
    {
    Start_Service(); 
    }
    if( lstrcmpi( argv[1], TEXT("uninstall")) == 0 )
    {
    Uninstall_Service();
    }
    if( lstrcmpi( argv[1], TEXT("stop")) == 0 )
    {
    Stop_Service();
    }

    return 0;
}
