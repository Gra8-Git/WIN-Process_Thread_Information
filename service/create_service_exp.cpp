#include <windows.h>
#include<iostream>
using namespace std;

TCHAR *service_name = LPTSTR("testcomm 0.2");

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
                break;
                    case ERROR_CIRCULAR_DEPENDENCY:
                std::cout<<" Create Service Object and add to SC_manager database (SC_Manager) ERROR: A circular service dependency was specified."<<(int)dwError<<"\n"; 
                break;
                    case ERROR_DUPLICATE_SERVICE_NAME:
                std::cout<<" Create Service Object and add to SC_manager database (SC_Manager) ERROR: Service Name already exists in the service control manager database."<<(int)dwError<<"\n"; 
                break;
                    case ERROR_INVALID_HANDLE:
                std::cout<<" Create Service Object and add to SC_manager database (SC_Manager) ERROR: Handler to the specific control manager database is invalid."<<(int)dwError<<"\n"; 
                break;
                    case ERROR_INVALID_NAME :
                std::cout<<" Create Service Object and add to SC_manager database (SC_Manager) ERROR: The specific service name is invalid."<<(int)dwError<<"\n"; 
                break;
                    case ERROR_INVALID_PARAMETER :
                std::cout<<" Create Service Object and add to SC_manager database (SC_Manager) ERROR: A parameter that was specified is invalid."<<(int)dwError<<"\n"; 
                break;
                    case ERROR_INVALID_SERVICE_ACCOUNT :
                std::cout<<" Create Service Object and add to SC_manager database (SC_Manager) ERROR: The user account name specified in the lpServiceStartName parameter does not exist."<<(int)dwError<<"\n"; 
                break;
                    case ERROR_SERVICE_EXISTS :
                std::cout<<" Create Service Object and add to SC_manager database (SC_Manager) ERROR: The specified service already exists in this database."<<(int)dwError<<"\n"; 
                break;
                    case ERROR_SERVICE_MARKED_FOR_DELETE :
                std::cout<<" Create Service Object and add to SC_manager database (SC_Manager) ERROR: The specified service already exists in this database and has been marked for deletion."<<(int)dwError<<"\n"; 
                break;
                    default:
                std::cout<<" Create Service Object and add to SC_manager database (SC_Manager) ERROR: UNKOWN ERROR "<<(int)dwError<<"\n";      

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
}


int main(int argc, TCHAR *argv[])
{

    Create_Service();
    return 0;
}

