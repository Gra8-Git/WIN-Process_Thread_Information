#include <windows.h>
#include<iostream>
#define SERVICE_ACCEPT_STOP 0x00000001
using namespace std;

TCHAR *service_name = LPTSTR("testcomm 0.2");

//Uninstall servie from SC -Manager database
void UninstallService()
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
                DeleteService(service);
                std::cout<<"Service was removed from SC manager:"<<service_name<<" \n"; 
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
}

int main(int argc, TCHAR *argv[])
{

    UninstallService();

    return 0;
}

