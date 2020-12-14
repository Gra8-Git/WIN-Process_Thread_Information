#include <windows.h>
#include<iostream>
#include<winsvc.h>
using namespace std;


// SERVICE  LAUNCHPROTECTED TYPES SUPPORTED FOR ANTI-MALWARE PROTECTION
#define SERVICE_LAUNCH_PROTECTED_NONE                    0
#define SERVICE_LAUNCH_PROTECTED_WINDOWS                 1
#define SERVICE_LAUNCH_PROTECTED_WINDOWS_LIGHT           2
#define SERVICE_LAUNCH_PROTECTED_ANTIMALWARE_LIGHT       3

#define SERVICE_CONFIG_LAUNCH_PROTECTED                  12

typedef struct _SERVICE_LAUNCH_PROTECTED_INFO {
  DWORD dwLaunchProtected;
} SERVICE_LAUNCH_PROTECTED_INFO, *PSERVICE_LAUNCH_PROTECTED_INFO;

// SERVICE  LAUNCHPROTECTED TYPES SUPPORTED

TCHAR *service_name = LPTSTR("cLp 0.1");
SERVICE_DESCRIPTION sd;
LPTSTR description = LPTSTR("The service description.");
SERVICE_STATUS Service_Status;
SERVICE_STATUS_HANDLE Service_Status_Handle = 0;
HANDLE Stop_Service_Event = 0;
//CREATE SERVICE SC -Manager database
int Create_Service()
{
    //SERVICE ANTI-MALWARE PROTECTION
    SERVICE_LAUNCH_PROTECTED_INFO Info;
    Info.dwLaunchProtected = SERVICE_LAUNCH_PROTECTED_ANTIMALWARE_LIGHT;
    //SERVICE ANTI-MALWARE PROTECTION


    SC_HANDLE Open_Control_Manager=NULL;
    SC_HANDLE service=NULL;



//makecert.exe -a SHA256 -r -pe -ss my -n "CN=TestSrv" -eku 1.3.6.1.5.5.7.3.3 TestSrv.cer
//makecert.exe -a SHA256 -r -pe -ss my -n "CN=TestElam" -eku 1.3.6.1.4.1.311.61.4.1,1.3.6.1.5.5.7.3.3 TestElam.cer


//*.sys file 
/*
MicrosoftElamCertificateInfo MSElamCertInfoID
{
1, // count of entries, number of maximum entries allowed is 3
L"hash sha256\0", // certmgr.exe /v path\to\user\binary.exe
0x800C, // the user binary is signed with the sha256 file hash algorith (like the driver binary).
L"\0", //No EKU other then code signing EKU is present in the certificate used to sign the user binary.
}

The user binary is signed with the following command (Post-build):

"C:\Program Files (x86)\Windows Kits\10\bin\x64\signtool.exe" sign /fd SHA256 /a /v /ph /sha1 hash "$(TargetDir)$(TargetFileName)"

all procedures before protection of the service
*/


/*
HANDLE FileHandle = NULL;

FileHandle = CreateFile(L"C:\\Windows\\System32\\drivers\\elamsample.sys", FILE_READ_DATA,FILE_SHARE_READ,NULL,OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,NULL);

if (InstallElamCertificateInfo(FileHandle) == FALSE)
{
    std::cout<<"cant install cert"<<GetLastError();
    return 0;
}
*/


    Open_Control_Manager = OpenSCManager(
        NULL, //MACHINE NAME
        NULL, //DATABASE NAME
        SC_MANAGER_CREATE_SERVICE //OR SC_MANAGER_ALL_ACCESS  RIGHTS TO CONNECT
        );
    if (Open_Control_Manager)
    {
        TCHAR path[_MAX_PATH + 1];
        if (GetModuleFileName(0, path, sizeof(path) / sizeof(path[0])) > 0)     //Get path of the application to create on his base service 
        {
            service = CreateService(Open_Control_Manager, service_name, service_name,
                                              SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
                                              SERVICE_AUTO_START |SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL, path,
                                              0, 0, 0, 0, 0);

            /*
            if (ChangeServiceConfig2(service,
                         SERVICE_CONFIG_LAUNCH_PROTECTED,
                         &Info) == FALSE)
            {
            std::cout<<"ChangeServiceConfig error: "<<GetLastError();
            }
            */
            /*
            CreateService()
            SERVICE_DEMAND_START A service started by the service control manager when a process calls the StartService function.
            SERVICE_AUTO_START   A service started automatically by the service control manager during system startup. 
            SERVICE_BOOT_START   A device driver started by the system loader.
            SERVICE_DISABLED     A service that cannot be started. Attempts to start the service result in the error code ERROR_SERVICE_DISABLED.
            SERVICE_SYSTEM_START A device driver started by the IoInitSystem function.
            */
            if (service)
            {
                sd.lpDescription = description;
                ChangeServiceConfig2(service, SERVICE_CONFIG_DESCRIPTION, &sd);
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
        CloseServiceHandle(service);
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
    SC_HANDLE Open_Control_Manager = NULL;
    SC_HANDLE service =NULL;
    Open_Control_Manager = OpenSCManager(0, 0, SC_MANAGER_CONNECT);

    if (Open_Control_Manager)
    {
        service = OpenService(Open_Control_Manager,service_name, SERVICE_ALL_ACCESS | DELETE); //return handler of the service from SC-manager database
        if (service)
        {
            
            if (QueryServiceStatus(service, &Service_Status)) //Retrieves the current status of the specified service.
            {
            switch (Service_Status.dwCurrentState) 
            {
                    case SERVICE_STOPPED:
                    {
                    if(DeleteService(service)){
                std::cout<<"Service was removed from SC manager:"<<service_name<<Service_Status.dwCurrentState<<" \n"; }else{
                    std::cout<<"Can't  removed service from SC manager:"<<service_name<<" \n"; 
                }break;}
                    case SERVICE_STOP_PENDING:
                {std::cout<<"The service is stopping:"<<service_name<<" \n"; break;}
                    case SERVICE_START_PENDING:
                {std::cout<<"The service is starting. :"<<service_name<<" \n"; break;}
                    case SERVICE_RUNNING:
                {std::cout<<"The service is running.:"<<service_name<<" \n"; break;}
                    case SERVICE_PAUSED:
                {std::cout<<"The service is paused. :"<<service_name<<" \n"; break;}
                    case SERVICE_PAUSE_PENDING:
                {std::cout<<"The service pause is pending. :"<<service_name<<" \n"; break;}
                    case SERVICE_CONTINUE_PENDING:
                {std::cout<<"The service continue is pending. :"<<service_name<<" \n"; break;}
                    default:
                {std::cout<<" UNKNOWN STATUS OF THE SERVICE TRY TO REMOVE IT MANUALY with command line.\n";      }
            }
            }
            else
            {
                   DWORD dwError = GetLastError();  //Get all errors for OpenSCManager()
            switch (dwError) 
            {
                    case ERROR_ACCESS_DENIED:
                std::cout<<" Query Service Status (SC_Manager) ERROR: The handle does not have the SERVICE_QUERY_STATUS access right."<<(int)dwError<<"\n"; 
                CloseServiceHandle(Open_Control_Manager);
                break;
                    case ERROR_INVALID_HANDLE:
                std::cout<<" Query to Service Status (SC_Manager) ERROR: The handle is invalid."<<(int)dwError<<"\n"; 
                CloseServiceHandle(Open_Control_Manager);
                break;
                    default:
                std::cout<<" Query to Service Status (SC_Manager) ERROR: UNKOWN ERROR "<<(int)dwError<<"\n";
                CloseServiceHandle(Open_Control_Manager);
            }
            }
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
    CloseServiceHandle(service);
    CloseServiceHandle(Open_Control_Manager);
}
//STOP SERVICE  SC -Manager database
void Stop_Service(void)
{
    SERVICE_STATUS_PROCESS serviceStatus;
    DWORD dwOldCheckPoint;
    DWORD dwStartTickCount;
    DWORD dwWaitTime;
    DWORD dwBytesNeeded;

    BOOL BQueryServiceStatusEx=FALSE;
    //HANDLE'S
    SC_HANDLE Open_Control_Manager = NULL;
    SC_HANDLE service =NULL;
    //HANDLE'S
    Open_Control_Manager=OpenSCManager(0, 0, SC_MANAGER_CONNECT);

    if (Open_Control_Manager != NULL)
    {
        service = OpenService(Open_Control_Manager,service_name, SERVICE_STOP |  SERVICE_QUERY_STATUS |  SERVICE_ENUMERATE_DEPENDENTS ); //return handler of the service from SC-manager database
        if (service)
        {

            BQueryServiceStatusEx=QueryServiceStatusEx(service, SC_STATUS_PROCESS_INFO,(LPBYTE)&serviceStatus,sizeof(SERVICE_STATUS_PROCESS),&dwBytesNeeded);
            if (TRUE==BQueryServiceStatusEx) //Retrieves the current status of the specified service.
            {
            switch (serviceStatus.dwCurrentState) 
            {
                    case SERVICE_STOPPED:
                std::cout<<"Service was stoped :"<<service_name<<" \n"; 
                break;
                    case SERVICE_STOP_PENDING:
                std::cout<<"The service is stopping:"<<service_name<<" \n"; 
                if (!ControlService(service,SERVICE_CONTROL_STOP,(LPSERVICE_STATUS) &serviceStatus))// send code to stop service
                    {
                    std::cout<<"ControlService failed "<< GetLastError()<<"\n";
                    }
                    else
                    {
                    std::cout<<"Service stopped\n";
                    }
                    break;
                    case SERVICE_START_PENDING:
                std::cout<<"The service is starting status :"<<service_name<<" \n"; 
                break;
                    case SERVICE_RUNNING:
                    if (!ControlService(service,SERVICE_CONTROL_STOP,(LPSERVICE_STATUS) &serviceStatus))// send code to stop service
                    {
                    std::cout<<"ControlService failed "<< GetLastError()<<"\n";
                    }
                    else
                    {
                    std::cout<<"Service stopped\n";
                    }
                break;
                    case SERVICE_PAUSED:
                std::cout<<"The service is paused. :"<<service_name<<" \n"; 
                    if (!ControlService(service,SERVICE_CONTROL_STOP,(LPSERVICE_STATUS) &serviceStatus))// send code to stop service
                    {
                    std::cout<<"ControlService failed "<< GetLastError()<<"\n";
                    }
                    else
                    {
                    std::cout<<"Service was stopped\n";
                    }
                break;
                    case SERVICE_PAUSE_PENDING:
                std::cout<<"The service pause is pending. :"<<service_name<<" \n"; 
                    if (!ControlService(service,SERVICE_CONTROL_STOP,(LPSERVICE_STATUS) &serviceStatus))// send code to stop service
                    {
                    std::cout<<"ControlService failed "<< GetLastError()<<"\n";
                    }
                    else
                    {
                    std::cout<<"Service stopped\n";
                    }
                break;
                    case SERVICE_CONTINUE_PENDING:
                std::cout<<"The service continue is pending. :"<<service_name<<" \n"; 
                    if (!ControlService(service,SERVICE_CONTROL_STOP,(LPSERVICE_STATUS) &serviceStatus))// send code to stop service
                    {
                    std::cout<<"ControlService failed "<< GetLastError()<<"\n";
                    }
                    else
                    {
                    std::cout<<"Service stopped\n";
                    }
                break;
                    default:
                std::cout<<" UNKNOWN STATUS OF THE SERVICE TRY TO STOP IT MANUALY with command line. SC -Manager if stop service unsuccessful\n"; 
                    if (!ControlService(service,SERVICE_CONTROL_STOP,(LPSERVICE_STATUS) &serviceStatus))// send code to stop service
                    {
                    std::cout<<"ControlService failed "<< GetLastError()<<"\n";
                    }
                    else
                    {
                    std::cout<<"Service stopped\n";
                    }
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
void Start_Service(void)
{
    SERVICE_STATUS_PROCESS serviceStatus;
    DWORD dwOldCheckPoint;
    DWORD dwStartTickCount;
    DWORD dwWaitTime;
    DWORD dwBytesNeeded;

    //SCM_HANDLE 
    BOOL BQueryServiceStatusEx=FALSE;
    SC_HANDLE hOpenSCManager;
    SC_HANDLE service;
    //SCM_HANDLE

    //OPEN SC -MANAGER 
    hOpenSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS); // full access rights
    if (hOpenSCManager!=NULL)
        {
        
    //OPEN SERVICE 
    service = OpenService(hOpenSCManager,service_name,SERVICE_ALL_ACCESS); // full access rights
    if (service != NULL)
        {
        
            //READ SERVICE STATUS 
    BQueryServiceStatusEx=QueryServiceStatusEx(service,SC_STATUS_PROCESS_INFO,(LPBYTE)&serviceStatus,sizeof(SERVICE_STATUS_PROCESS),&dwBytesNeeded);
    if (TRUE==BQueryServiceStatusEx)
    {
        
        switch (serviceStatus.dwCurrentState) 
            {
                    case SERVICE_STOPPED:
                // START THE SERVICE
                if (!StartService(service, 0, NULL)) 
                {
                DWORD dwError = GetLastError();  //Get all errors for OpenSCManager()
                switch (dwError) 
                {
                    case ERROR_ACCESS_DENIED:
                std::cout<<" The handle does not have the SERVICE_START access right."<<(int)dwError<<"\n"; 
                break;
                    case ERROR_INVALID_HANDLE:
                std::cout<<"The handle is invalid. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_PATH_NOT_FOUND:
                std::cout<<"The service binary file could not be found. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_ALREADY_RUNNING:
                std::cout<<"An instance of the service is already running. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_DATABASE_LOCKED:
                std::cout<<"The database is locked. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_DEPENDENCY_DELETED:
                std::cout<<"The service depends on a service that does not exist or has been marked for deletion. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_DEPENDENCY_FAIL:
                std::cout<<"The service depends on another service that has failed to start. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_DISABLED:
                std::cout<<"The service has been disabled. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_LOGON_FAILED:
                std::cout<<"The service did not start due to a logon failure.\n This error occurs if the service is configured to run under  \nan account that does not have the Log on as a service right."<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_MARKED_FOR_DELETE:
                std::cout<<"The service has been marked for deletion. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_NO_THREAD:
                std::cout<<"A thread could not be created for the service. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_REQUEST_TIMEOUT:
                std::cout<<"The process for the service was started, but it did not call StartServiceCtrlDispatcher,\nor the thread that called StartServiceCtrlDispatcher may be blocked in a control handler function. "<<(int)dwError<<"\n"; 
                break;  
                              
                    default:
                std::cout<<" Connection to Service Manager (SC_Manager) ERROR: UNKOWN ERROR "<<(int)dwError<<"\n";      
                }
                CloseServiceHandle(service);
                CloseServiceHandle(hOpenSCManager);
                }else{ std::cout<<"Start service success \n";}
                //START SERVICE
                break;
                    case SERVICE_STOP_PENDING:
                std::cout<<"The service is stopping status:"<<service_name<<" \n"; 
                while (serviceStatus.dwCurrentState == SERVICE_STOP_PENDING)
                        {
                        //QUERY SERVICE
                        if (!QueryServiceStatusEx(service,SC_STATUS_PROCESS_INFO,(LPBYTE)&serviceStatus,sizeof(SERVICE_STATUS_PROCESS),&dwBytesNeeded))
                        {
                        std::cout<<"QueryServiceStatusEx failed :"<< GetLastError()<<"\n";
                        CloseServiceHandle(service);
                        CloseServiceHandle(hOpenSCManager);
                        }else{std::cout<<"Service Query success"<<serviceStatus.dwServiceFlags<<"\n";}
                        //QUERY SERVICE
                        }
                // START THE SERVICE
                if (!StartService(service, 0, NULL)) 
                {
                DWORD dwError = GetLastError();  //Get all errors for OpenSCManager()
                switch (dwError) 
                {
                    case ERROR_ACCESS_DENIED:
                std::cout<<" The handle does not have the SERVICE_START access right."<<(int)dwError<<"\n"; 
                break;
                    case ERROR_INVALID_HANDLE:
                std::cout<<"The handle is invalid. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_PATH_NOT_FOUND:
                std::cout<<"The service binary file could not be found. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_ALREADY_RUNNING:
                std::cout<<"An instance of the service is already running. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_DATABASE_LOCKED:
                std::cout<<"The database is locked. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_DEPENDENCY_DELETED:
                std::cout<<"The service depends on a service that does not exist or has been marked for deletion. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_DEPENDENCY_FAIL:
                std::cout<<"The service depends on another service that has failed to start. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_DISABLED:
                std::cout<<"The service has been disabled. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_LOGON_FAILED:
                std::cout<<"The service did not start due to a logon failure.\n This error occurs if the service is configured to run under  \nan account that does not have the Log on as a service right."<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_MARKED_FOR_DELETE:
                std::cout<<"The service has been marked for deletion. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_NO_THREAD:
                std::cout<<"A thread could not be created for the service. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_REQUEST_TIMEOUT:
                std::cout<<"The process for the service was started, but it did not call StartServiceCtrlDispatcher,\nor the thread that called StartServiceCtrlDispatcher may be blocked in a control handler function. "<<(int)dwError<<"\n"; 
                break;  
                              
                    default:
                std::cout<<" Connection to Service Manager (SC_Manager) ERROR: UNKOWN ERROR "<<(int)dwError<<"\n";      
                }
                CloseServiceHandle(service);
                CloseServiceHandle(hOpenSCManager);
                }else{ std::cout<<"Start service success \n";}
                //START SERVICE
                    break;
                    case SERVICE_START_PENDING:
                std::cout<<"The service is starting status:"<<service_name<<" \n"; 
                while (serviceStatus.dwCurrentState == SERVICE_START_PENDING)
                        {
                        //QUERY SERVICE
                        if (!QueryServiceStatusEx(service,SC_STATUS_PROCESS_INFO,(LPBYTE)&serviceStatus,sizeof(SERVICE_STATUS_PROCESS),&dwBytesNeeded))
                        {
                        std::cout<<"QueryServiceStatusEx failed :"<< GetLastError()<<"\n";
                        CloseServiceHandle(service);
                        CloseServiceHandle(hOpenSCManager);
                        }else{std::cout<<"Service Query success"<<serviceStatus.dwServiceFlags<<"\n";}
                        //QUERY SERVICE
                        }
                break;
                    case SERVICE_RUNNING:
                std::cout<<"The service is running status:"<<service_name<<" \n"; 
                    if (serviceStatus.dwCurrentState != SERVICE_STOPPED || serviceStatus.dwCurrentState != SERVICE_STOP_PENDING)
                      {std::cout<<"Service is already running\n";}
                    else{std::cout<<"Cannot start the service because it is stopped"<<GetLastError()<<"\n";}
                break;
                    case SERVICE_PAUSED:
                std::cout<<"The service is paused status :"<<service_name<<" \n"; 
                // START THE SERVICE
                if (!StartService(service, 0, NULL)) 
                {
                DWORD dwError = GetLastError();  //Get all errors for OpenSCManager()
                switch (dwError) 
                {
                    case ERROR_ACCESS_DENIED:
                std::cout<<" The handle does not have the SERVICE_START access right."<<(int)dwError<<"\n"; 
                break;
                    case ERROR_INVALID_HANDLE:
                std::cout<<"The handle is invalid. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_PATH_NOT_FOUND:
                std::cout<<"The service binary file could not be found. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_ALREADY_RUNNING:
                std::cout<<"An instance of the service is already running. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_DATABASE_LOCKED:
                std::cout<<"The database is locked. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_DEPENDENCY_DELETED:
                std::cout<<"The service depends on a service that does not exist or has been marked for deletion. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_DEPENDENCY_FAIL:
                std::cout<<"The service depends on another service that has failed to start. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_DISABLED:
                std::cout<<"The service has been disabled. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_LOGON_FAILED:
                std::cout<<"The service did not start due to a logon failure.\n This error occurs if the service is configured to run under  \nan account that does not have the Log on as a service right."<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_MARKED_FOR_DELETE:
                std::cout<<"The service has been marked for deletion. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_NO_THREAD:
                std::cout<<"A thread could not be created for the service. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_REQUEST_TIMEOUT:
                std::cout<<"The process for the service was started, but it did not call StartServiceCtrlDispatcher,\nor the thread that called StartServiceCtrlDispatcher may be blocked in a control handler function. "<<(int)dwError<<"\n"; 
                break;  
                              
                    default:
                std::cout<<" Connection to Service Manager (SC_Manager) ERROR: UNKOWN ERROR "<<(int)dwError<<"\n";      
                }
                CloseServiceHandle(service);
                CloseServiceHandle(hOpenSCManager);
                }else{ std::cout<<"Start service success \n";}
                //START SERVICE
                break;
                    case SERVICE_PAUSE_PENDING:
                std::cout<<"The service pause is pending status :"<<service_name<<" \n"; 
                while (serviceStatus.dwCurrentState == SERVICE_PAUSE_PENDING)
                        {
                        //QUERY SERVICE
                        if (!QueryServiceStatusEx(service,SC_STATUS_PROCESS_INFO,(LPBYTE)&serviceStatus,sizeof(SERVICE_STATUS_PROCESS),&dwBytesNeeded))
                        {
                        std::cout<<"QueryServiceStatusEx failed :"<< GetLastError()<<"\n";
                        CloseServiceHandle(service);
                        CloseServiceHandle(hOpenSCManager);
                        }else{std::cout<<"Service Query success"<<serviceStatus.dwServiceFlags<<"\n";}
                        //QUERY SERVICE
                        }
                // START THE SERVICE
                if (!StartService(service, 0, NULL)) 
                {
                DWORD dwError = GetLastError();  //Get all errors for OpenSCManager()
                switch (dwError) 
                {
                    case ERROR_ACCESS_DENIED:
                std::cout<<" The handle does not have the SERVICE_START access right."<<(int)dwError<<"\n"; 
                break;
                    case ERROR_INVALID_HANDLE:
                std::cout<<"The handle is invalid. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_PATH_NOT_FOUND:
                std::cout<<"The service binary file could not be found. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_ALREADY_RUNNING:
                std::cout<<"An instance of the service is already running. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_DATABASE_LOCKED:
                std::cout<<"The database is locked. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_DEPENDENCY_DELETED:
                std::cout<<"The service depends on a service that does not exist or has been marked for deletion. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_DEPENDENCY_FAIL:
                std::cout<<"The service depends on another service that has failed to start. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_DISABLED:
                std::cout<<"The service has been disabled. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_LOGON_FAILED:
                std::cout<<"The service did not start due to a logon failure.\n This error occurs if the service is configured to run under  \nan account that does not have the Log on as a service right."<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_MARKED_FOR_DELETE:
                std::cout<<"The service has been marked for deletion. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_NO_THREAD:
                std::cout<<"A thread could not be created for the service. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_REQUEST_TIMEOUT:
                std::cout<<"The process for the service was started, but it did not call StartServiceCtrlDispatcher,\nor the thread that called StartServiceCtrlDispatcher may be blocked in a control handler function. "<<(int)dwError<<"\n"; 
                break;  
                              
                    default:
                std::cout<<" Connection to Service Manager (SC_Manager) ERROR: UNKOWN ERROR "<<(int)dwError<<"\n";      
                }
                CloseServiceHandle(service);
                CloseServiceHandle(hOpenSCManager);
                }else{ std::cout<<"Start service success \n";}
                //START SERVICE
                break;
                    case SERVICE_CONTINUE_PENDING:
                std::cout<<"The service continue is pending status :"<<service_name<<" \n"; 
                while (serviceStatus.dwCurrentState == SERVICE_CONTINUE_PENDING)
                        {
                        //QUERY SERVICE
                        if (!QueryServiceStatusEx(service,SC_STATUS_PROCESS_INFO,(LPBYTE)&serviceStatus,sizeof(SERVICE_STATUS_PROCESS),&dwBytesNeeded))
                        {
                        std::cout<<"QueryServiceStatusEx failed :"<< GetLastError()<<"\n";
                        CloseServiceHandle(service);
                        CloseServiceHandle(hOpenSCManager);
                        }else{std::cout<<"Service Query success"<<serviceStatus.dwServiceFlags<<"\n";}
                        //QUERY SERVICE
                        }
                // START THE SERVICE
                if (!StartService(service, 0, NULL)) 
                {
                DWORD dwError = GetLastError();  //Get all errors for OpenSCManager()
                switch (dwError) 
                {
                    case ERROR_ACCESS_DENIED:
                std::cout<<" The handle does not have the SERVICE_START access right."<<(int)dwError<<"\n"; 
                break;
                    case ERROR_INVALID_HANDLE:
                std::cout<<"The handle is invalid. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_PATH_NOT_FOUND:
                std::cout<<"The service binary file could not be found. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_ALREADY_RUNNING:
                std::cout<<"An instance of the service is already running. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_DATABASE_LOCKED:
                std::cout<<"The database is locked. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_DEPENDENCY_DELETED:
                std::cout<<"The service depends on a service that does not exist or has been marked for deletion. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_DEPENDENCY_FAIL:
                std::cout<<"The service depends on another service that has failed to start. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_DISABLED:
                std::cout<<"The service has been disabled. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_LOGON_FAILED:
                std::cout<<"The service did not start due to a logon failure.\n This error occurs if the service is configured to run under  \nan account that does not have the Log on as a service right."<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_MARKED_FOR_DELETE:
                std::cout<<"The service has been marked for deletion. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_NO_THREAD:
                std::cout<<"A thread could not be created for the service. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_REQUEST_TIMEOUT:
                std::cout<<"The process for the service was started, but it did not call StartServiceCtrlDispatcher,\nor the thread that called StartServiceCtrlDispatcher may be blocked in a control handler function. "<<(int)dwError<<"\n"; 
                break;  
                              
                    default:
                std::cout<<" Connection to Service Manager (SC_Manager) ERROR: UNKOWN ERROR "<<(int)dwError<<"\n";      
                }
                CloseServiceHandle(service);
                CloseServiceHandle(hOpenSCManager);
                }else{ std::cout<<"Start service success \n";}
                //START SERVICE
                break;
                    default:
                std::cout<<" UNKNOWN STATUS OF THE SERVICE TRY TO START IT MANUALY with command line. SC -Manager if start service unsuccessful\n";
                // START THE SERVICE
                if (!StartService(service, 0, NULL)) 
                {
                DWORD dwError = GetLastError();  //Get all errors for OpenSCManager()
                switch (dwError) 
                {
                    case ERROR_ACCESS_DENIED:
                std::cout<<" The handle does not have the SERVICE_START access right."<<(int)dwError<<"\n"; 
                break;
                    case ERROR_INVALID_HANDLE:
                std::cout<<"The handle is invalid. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_PATH_NOT_FOUND:
                std::cout<<"The service binary file could not be found. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_ALREADY_RUNNING:
                std::cout<<"An instance of the service is already running. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_DATABASE_LOCKED:
                std::cout<<"The database is locked. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_DEPENDENCY_DELETED:
                std::cout<<"The service depends on a service that does not exist or has been marked for deletion. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_DEPENDENCY_FAIL:
                std::cout<<"The service depends on another service that has failed to start. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_DISABLED:
                std::cout<<"The service has been disabled. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_LOGON_FAILED:
                std::cout<<"The service did not start due to a logon failure.\n This error occurs if the service is configured to run under  \nan account that does not have the Log on as a service right."<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_MARKED_FOR_DELETE:
                std::cout<<"The service has been marked for deletion. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_NO_THREAD:
                std::cout<<"A thread could not be created for the service. "<<(int)dwError<<"\n"; 
                break;  
                    case ERROR_SERVICE_REQUEST_TIMEOUT:
                std::cout<<"The process for the service was started, but it did not call StartServiceCtrlDispatcher,\nor the thread that called StartServiceCtrlDispatcher may be blocked in a control handler function. "<<(int)dwError<<"\n"; 
                break;  
                              
                    default:
                std::cout<<" Connection to Service Manager (SC_Manager) ERROR: UNKOWN ERROR "<<(int)dwError<<"\n";      
                }
                CloseServiceHandle(service);
                CloseServiceHandle(hOpenSCManager);
                }else{ std::cout<<"Start service success \n";}
                //START SERVICE 
             }

    }else
    {
            DWORD dwError = GetLastError();  //Get all errors for OpenSCManager()
            switch (dwError) 
            {
                    case ERROR_ACCESS_DENIED:
                std::cout<<" Query Service Status (SC_Manager) ERROR: The handle does not have the SERVICE_QUERY_STATUS access right."<<(int)dwError<<"\n";
                CloseServiceHandle(service);
                CloseServiceHandle(hOpenSCManager);
                break;
                    case ERROR_INVALID_HANDLE:
                std::cout<<" Query to Service Status (SC_Manager) ERROR: The handle is invalid."<<(int)dwError<<"\n"; 
                CloseServiceHandle(service);
                CloseServiceHandle(hOpenSCManager);
                break;
                    default:
                std::cout<<" Query to Service Status (SC_Manager) ERROR: UNKOWN ERROR "<<(int)dwError<<"\n";
                CloseServiceHandle(service);
                CloseServiceHandle(hOpenSCManager);
            }
    }
    //READ SERVICE STATUS
        }
    else{
            DWORD dwError = GetLastError();  //Get all errors for OpenSCManager()
            switch (dwError) 
            {
                    case ERROR_ACCESS_DENIED:
                std::cout<<" Open Service (SC_Manager) ERROR: Access Deny."<<(int)dwError<<"\n"; 
                CloseServiceHandle(hOpenSCManager);
                break;
                    case ERROR_SERVICE_DOES_NOT_EXIST:
                std::cout<<" Open Service (SC_Manager) ERROR: SERVICE DOSE NOT EXIST."<<(int)dwError<<"\n";
                CloseServiceHandle(hOpenSCManager); 
                break;
                    case ERROR_INVALID_HANDLE:
                std::cout<<" Open Service (SC_Manager) ERROR: INVALID HANDLER. "<<(int)dwError<<"\n";
                CloseServiceHandle(hOpenSCManager);      
                break;
                    case ERROR_INVALID_NAME:
                std::cout<<" Open Service (SC_Manager) ERROR: Specific Service name is invalid. "<<(int)dwError<<"\n";
                CloseServiceHandle(hOpenSCManager);      
                break;
                    default:
                std::cout<<" Open Service (SC_Manager) ERROR: UNKOWN ERROR. "<<(int)dwError<<"\n";     
                CloseServiceHandle(hOpenSCManager); 
            }    
        }
    //OPEN SERVICE
        }
    else{
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
    //OPEN SC -MANAGER

    CloseServiceHandle(service);
    CloseServiceHandle(hOpenSCManager);
}
//REPORT SERVICE STATUS IN SC -MANAGER (FUNCTION FOR SERVICE MAIN)
void ReportServiceStatus( DWORD dwCurrentState,DWORD dwWin32ExitCode, DWORD dwWaitHint)
{
    static DWORD dwCheckPoint = 1;
    // SERVICE_STATUS structure.
    Service_Status.dwCurrentState = dwCurrentState;
    Service_Status.dwWin32ExitCode = dwWin32ExitCode;
    Service_Status.dwWaitHint = dwWaitHint;
    // SERVICE_STATUS structure.
    //CHECK CURRENT STATE OF THE SERVICE
    if (dwCurrentState == SERVICE_START_PENDING)  //SERVICE ABOUT TO START 
        {Service_Status.dwControlsAccepted = 0;}
    else{Service_Status.dwControlsAccepted = SERVICE_ACCEPT_STOP;}
    //CHECK CURRENT STATE OF THE SERVICE
    //PROGRESS FOR SERVICE OPERATION
    if ((dwCurrentState == SERVICE_RUNNING)||(dwCurrentState == SERVICE_STOPPED))
        {Service_Status.dwCheckPoint = 0;}
    else{Service_Status.dwCheckPoint = dwCheckPoint++;}
    //PROGRESS FOR SERVICE OPERATION
    //NITIFY THE CURRENT STATE OF SC -MANAGER
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
    //NITIFY THE CURRENT STATE OF SC -MANAGER
}
//HANDLER FOR SERVICE STATUS FOR SC -MANAGER (FUNCTION FOR SERVICE MAIN)
void WINAPI Service_Control_Handler(DWORD controlCode)
{
    switch (controlCode)
    {
    case SERVICE_CONTROL_INTERROGATE:
        break;
    case SERVICE_CONTROL_SHUTDOWN:
        
        Service_Status.dwWin32ExitCode = 0;
        Service_Status.dwCurrentState =SERVICE_CONTROL_SHUTDOWN;

        
        SetEvent(Stop_Service_Event);
        std::cout<<"Service ShutDown";
        break;
    case SERVICE_CONTROL_STOP:
        ReportServiceStatus(SERVICE_STOPPED,NO_ERROR,0);
        SetEvent(Stop_Service_Event);
        std::cout<<"Service Stoped\n";
        break;
    case SERVICE_CONTROL_PAUSE:
        
        Service_Status.dwCurrentState =SERVICE_PAUSED;
        SetServiceStatus(Service_Status_Handle, &Service_Status);
        SetEvent(Stop_Service_Event);
        std::cout<<"Service paused\n";
        break;
    case SERVICE_CONTROL_CONTINUE:
        
        Service_Status.dwCurrentState =SERVICE_RUNNING;
        SetServiceStatus(Service_Status_Handle, &Service_Status);
        SetEvent(Stop_Service_Event);
        std::cout<<"Service Running\n";
        break;
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
//INITIALIZE SERVICE 
void Service_Init(DWORD dwArgc, LPTSTR *lpArgv)
{
    std::cout<<"Service start\n";
    Stop_Service_Event = CreateEvent(NULL, TRUE, FALSE, NULL);
    if(NULL==Stop_Service_Event)
        {ReportServiceStatus(SERVICE_STOPPED,NO_ERROR,0);}
    else{ReportServiceStatus(SERVICE_RUNNING,NO_ERROR,0);}
    //CHECK WHETHER TO STOP THE SERVICE
    while(1)
    {
        //WAIT FOR SIGNAL
        WaitForSingleObject(Stop_Service_Event,INFINITE);
        

        ReportServiceStatus(SERVICE_STOPPED,NO_ERROR,0);
    }
}
//SERVICE MAIN FUNCTION 
void WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpArgv)  //ERROR
{
    BOOL ServicestatusB = FALSE;
    //first you need to register service 
    Service_Status_Handle = RegisterServiceCtrlHandler(service_name, Service_Control_Handler); //register service handler inside SC -Manager
   // initialise service status structure 

    Service_Status.dwServiceType = SERVICE_WIN32_OWN_PROCESS; 
    Service_Status.dwServiceSpecificExitCode = NO_ERROR;  //whene error occur service send error  
    // end inicialization before start
    ReportServiceStatus(SERVICE_START_PENDING, NO_ERROR, 3000); //30 sec to start service  for panding  report inside SC-manager
    SetServiceStatus(Service_Status_Handle,&Service_Status);
    if(FALSE==ServicestatusB)
    {
        std::cout<<"Service status initial Setup FAILED ="<<GetLastError()<<"\n"; 
    }
    else
    {
        std::cout<<"Service status initial Setup SUCCESS \n"; 
    }
    Service_Init(dwArgc,lpArgv);
//end

}
//SERVICE TABLE ENTRY TO ADD MAIN FUNCTION TO SERVICE 
BOOL ServiceDispatcherTable()
{
    BOOL stServiceCtrlDispatcher =FALSE;
    SERVICE_TABLE_ENTRY serviceTable[] =
        {
            {service_name, (LPSERVICE_MAIN_FUNCTION)ServiceMain},
            {NULL, NULL}};
    stServiceCtrlDispatcher=StartServiceCtrlDispatcher(serviceTable);
    if(FALSE==stServiceCtrlDispatcher)
    { std::cout<<"stServiceCtrlDispatcher FAILD :"<<GetLastError()<<"\n"; return FALSE;}
    else{std::cout<<"stServiceCtrlDispatcher Success \n";return TRUE;}

}


int main(int argc, TCHAR *argv[])
{
if (argc < 2)           
{
     if (!ServiceDispatcherTable())            //prevent from error 1053  if call was send not from Control manager
     {
      std::cout<<"ERROR :"<< GetLastError();
     }
} 
else {
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
    
}
    return 0;
}

