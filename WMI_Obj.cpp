
//template
#define _WIN32_DCOM
#include <iostream>
using namespace std;
#include <comdef.h>
#include <Wbemidl.h>
#pragma comment(lib, "wbemuuid.lib")

int main(int argc, char** argv)
{
    HRESULT hres;


    hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hres))
    {
        cout << "Failed to initialize COM library. Error code = 0x"
            << hex << hres << endl;
        return 1;                
    }

  

    hres = CoInitializeSecurity(
        NULL,
        -1,                       
        NULL,                     
        NULL,                        
        RPC_C_AUTHN_LEVEL_DEFAULT,  
        RPC_C_IMP_LEVEL_IMPERSONATE, 
        NULL,                        
        EOAC_NONE,                  
        NULL                        
    );


    if (FAILED(hres))
    {
        cout << "Failed to initialize security. Error code = 0x"
            << hex << hres << endl;
        CoUninitialize();
        return 1;                  
    }


    IWbemLocator* pLoc = NULL;

    hres = CoCreateInstance(
        CLSID_WbemLocator,
        0,
        CLSCTX_INPROC_SERVER,
        IID_IWbemLocator, (LPVOID*)&pLoc);

    if (FAILED(hres))
    {
        cout << "Failed to create IWbemLocator object."
            << " Err code = 0x"
            << hex << hres << endl;
        CoUninitialize();
        return 1;                
    }



    IWbemServices* pSvc = NULL;

    // Connect to the root\cimv2 namespace with
    // the current user and obtain pointer pSvc
    // to make IWbemServices calls.
    hres = pLoc->ConnectServer(
        _bstr_t(L"ROOT\\CIMV2"), 
        NULL,                    
        NULL,                  
        0,                      
        NULL,                 
        0,                    
        0,                    
        &pSvc                 
    );

    if (FAILED(hres))
    {
        cout << "Could not connect. Error code = 0x"
            << hex << hres << endl;
        pLoc->Release();
        CoUninitialize();
        return 1;              
    }

    cout << "Connected to ROOT\\CIMV2 WMI namespace" << endl;


  

    hres = CoSetProxyBlanket(
        pSvc,                      
        RPC_C_AUTHN_WINNT,          
        RPC_C_AUTHZ_NONE,          
        NULL,                       
        RPC_C_AUTHN_LEVEL_CALL,     
        RPC_C_IMP_LEVEL_IMPERSONATE, 
        NULL,                        
        EOAC_NONE                 
    );

    if (FAILED(hres))
    {
        cout << "Could not set proxy blanket. Error code = 0x"
            << hex << hres << endl;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return 1;               
    }

  
   
    IEnumWbemClassObject* pEnumerator = NULL;
    hres = pSvc->ExecQuery(
        bstr_t("WQL"),
        bstr_t("SELECT * FROM  Win32_Process"),
        WBEM_FLAG_FORWARD_ONLY ,
        NULL,
        &pEnumerator);

    if (FAILED(hres))
    {
        cout << "Query for operating system name failed."
            << " Error code = 0x"
            << hex << hres << endl;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return 1;              
    }

  

    IWbemClassObject* pclsObj = NULL;
    ULONG uReturn = 0;

    while (pEnumerator)
    {
        HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
            &pclsObj, &uReturn);

        if (0 == uReturn)
        {
            break;
        }
        VARIANT ProcessId;
        VARIANT CommandLine;
        VARIANT ExecutablePath;



        hr = pclsObj->Get(L"ProcessId", 0, &ProcessId,0, 0);
        if (FAILED(hr))
        {
            wprintf(L"GetMethod hres = %08x\n", hr);
            pSvc->Release();
            pLoc->Release();
            pEnumerator->Release();
            CoUninitialize();
            return 1;
        }
        wprintf(L"ProcessID :  \n", ProcessId.bstrVal);
        VariantClear(&ProcessId);

      
        hr = pclsObj->Get(L"CommandLine", 0, &CommandLine, 0, 0);
        if (FAILED(hr))
        {
            wprintf(L"GetMethod hres = %08x\n", hr);
            pSvc->Release();
            pLoc->Release();
            pEnumerator->Release();
            CoUninitialize();
            return 1;
        }
        wprintf(L"CommandLine :  %s\n", CommandLine.bstrVal);
        VariantClear(&CommandLine);

        pclsObj->Release();
    }

    // Cleanup
    // ========

    pSvc->Release();
    pLoc->Release();
    pEnumerator->Release();
    CoUninitialize();

    return 0;  

}
