#include <windows.h>
#include <stdio.h>
#include <Imagehlp.h>
#include<string>
#pragma comment(lib, "Crypt32")
#pragma comment(lib, "Imagehlp")
 
 
 using namespace std;

 TCHAR* get_pe_cert(LPCSTR PE)
{
     
 DWORD type = CERT_X500_NAME_STR;

    HANDLE file = NULL;
    DWORD certCount = 0;
    DWORD indexes[128];
    DWORD i;
    BOOL status = FALSE;
    LPWIN_CERTIFICATE cert = NULL;
    DWORD certLen = 0;
    HCERTSTORE store;
    PCCERT_CONTEXT  certContext = NULL;   
    CERT_ENHKEY_USAGE keyUsage;
    char signingOID[] = szOID_PKIX_KP_CODE_SIGNING;
 
    file = CreateFile(PE, FILE_READ_DATA , FILE_SHARE_READ, NULL, OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL , NULL);
    if (INVALID_HANDLE_VALUE == file)
        return (char*)"CreateFile failed!";
    else
    {
        status = ImageEnumerateCertificates(file, CERT_SECTION_TYPE_ANY, &certCount, indexes, 128);
        if (!status)
            return (char*)"ImageEnumerateCertificates failed!\n";
        else if (certCount == 0)
            return (char*)"No available certificates!\n";
        else
        {
            for (i = 0; i < certCount; i++)
            {
                certLen = 0;
                cert = NULL;
                status = ImageGetCertificateData(file, indexes[i], NULL, &certLen);
                if (!status && (ERROR_INSUFFICIENT_BUFFER == GetLastError()))
                {
                    cert = (LPWIN_CERTIFICATE) LocalAlloc(0, certLen);
                    status = ImageGetCertificateData(file, indexes[i], cert, &certLen);
                    if (!status)
                        return (char*)"ImageGetCertificateData failed on index ";//, indexes[i], GetLastError());
                    else
                    {
                        CRYPT_DATA_BLOB p7Data;
                        p7Data.cbData = certLen - sizeof(DWORD) - sizeof(WORD) - sizeof(WORD);
                        p7Data.pbData = cert->bCertificate;
                        store = CertOpenStore(CERT_STORE_PROV_PKCS7, X509_ASN_ENCODING|PKCS_7_ASN_ENCODING, 0, 0, &p7Data);
                        if (store)
                        {
                            int count = 0;
                            keyUsage.cUsageIdentifier = 1;
                            keyUsage.rgpszUsageIdentifier = (LPSTR*) LocalAlloc(0, sizeof(LPSTR));
                            keyUsage.rgpszUsageIdentifier[0] = &signingOID[0];
 
                            do
                            {
                                certContext = CertFindCertificateInStore(store,
                                           X509_ASN_ENCODING | PKCS_7_ASN_ENCODING ,
                                           CERT_FIND_EXT_ONLY_ENHKEY_USAGE_FLAG,
                                           CERT_FIND_ENHKEY_USAGE,
                                           &keyUsage,
                                           certContext);
                     
                                if (certContext)
                                {        
                                    count++;
                                       
    DWORD count = CertGetNameString(certContext,CERT_NAME_RDN_TYPE,0,&type,0,0);
        if (count)
    {
        char *subject = (char *) LocalAlloc(0, count * sizeof(char));
        count = CertGetNameString(certContext,CERT_NAME_RDN_TYPE,0,&type,subject,count);
       
            //printf("%d - Certificate Subject: %s\n", count, subject);



        CloseHandle(file);
        LocalFree(subject);
        LocalFree(cert); 
         LocalFree(keyUsage.rgpszUsageIdentifier); 
        return subject;
               
   }   
                              
                                }
                       
                            } while (certContext);
 
                        if (count == 0)
                            return (char*)"No Code Signing certificates found\n";
                      
                        LocalFree(keyUsage.rgpszUsageIdentifier);
                        CertCloseStore(store, CERT_CLOSE_STORE_FORCE_FLAG);
                    }
                    else
                        return (char*)"Error on CertOpenStore.Index";
                }
                LocalFree(cert);
                }
                else
                    return (char*)"Error from ImageGetCertificateData on cert index";
            }
        }
        CloseHandle(file);
    }
    return (TCHAR*)"NeN";
}


int main()
{

   printf("File Certificate information :%s",get_pe_cert( "C:\\Program Files\\Avast Software\\Avast\\AvastUI.exe"));
  return  (TCHAR*)"NeN";
}
