#include <stdio.h>
#include <windows.h>
#include <Wincrypt.h>
#include <TCHAR.H>
#include <iostream>
#include <windows.h>
#include <string>
#define BUFSIZE 1024
#define MD5LEN  16     //32 digits
#define SHA1LEN 20  //40  digits
#define SHA256LEN 50 //60 digits
//typedef unsigned int ALG_ID;
using namespace std;

char* MD5(LPCSTR filename, ALG_ID encprot,int size)
{
    std::string hashline;
    char*  hashc[32];
    char *strHash;
    strHash = (char*)malloc(500);
    memset(strHash, '\0', 500);


    DWORD dwStatus = 0;
    BOOL bResult = FALSE;
    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    HANDLE hFile = NULL;
    BYTE rgbFile[BUFSIZE];
    DWORD cbRead = 0;
 
    DWORD cbHash = 0;
    CHAR rgbDigits[] = "0123456789abcdef";
   BYTE rgbHash[size];

    hFile = CreateFile(filename, GENERIC_READ,  FILE_SHARE_READ,  NULL,  OPEN_EXISTING,  FILE_FLAG_SEQUENTIAL_SCAN, NULL);

    if (INVALID_HANDLE_VALUE == hFile)
    {
        dwStatus = GetLastError();
        printf("Error opening file %s\nError: %d\n", filename, 
            dwStatus); 
 
    }

    // Get handle to the crypto provider  MD5
if(encprot==CALG_MD5){
    if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
    {
        dwStatus = GetLastError();
        printf("CryptAcquireContext failed: %d\n", dwStatus); 
        CloseHandle(hFile);

    }}

if(encprot==CALG_SHA1){
    if (!CryptAcquireContext(&hProv,NULL,NULL,PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)){
        dwStatus = GetLastError();
        printf("CryptAcquireContext failed: %d\n", dwStatus);
        CloseHandle(hFile);
    }}

if(encprot==CALG_SHA_256){
    if (!CryptAcquireContext(&hProv,NULL,NULL,PROV_RSA_AES,0)){
        dwStatus = GetLastError();
        printf("CryptAcquireContext failed: %d\n", dwStatus);
        CloseHandle(hFile);
    }}


    if (!CryptCreateHash(hProv, encprot, 0, 0, &hHash))
    {
        dwStatus = GetLastError();
        printf("CryptAcquireContext failed: %d\n", dwStatus); 
        CloseHandle(hFile);
        CryptReleaseContext(hProv, 0);

    }

    while (bResult = ReadFile(hFile, rgbFile, BUFSIZE, &cbRead, NULL))
    {
        if (0 == cbRead)
        {
            break;
        }

        if (!CryptHashData(hHash, rgbFile, cbRead, 0))
        {
            dwStatus = GetLastError();
            printf("CryptHashData failed: %d\n", dwStatus); 
            CryptReleaseContext(hProv, 0);
            CryptDestroyHash(hHash);
            CloseHandle(hFile);
  
        }
    }

    if (!bResult)
    {
        dwStatus = GetLastError();
        printf("ReadFile failed: %d\n", dwStatus); 
        CryptReleaseContext(hProv, 0);
        CryptDestroyHash(hHash);
        CloseHandle(hFile);
   
    }

    cbHash = size;
    
    if (CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0))
    {
      
        for (DWORD i = 0; i < cbHash; i++)
        {
            strHash[i * 2] = rgbDigits[rgbHash[i] >> 4];
            strHash[(i * 2) + 1] =rgbDigits[rgbHash[i] & 0xf];
           
        }
    
        
    }
    else
    {
        dwStatus = GetLastError();
        printf("CryptGetHashParam failed: %d\n", dwStatus); 
    }

if(encprot==CALG_MD5){
      printf("MD5 hash of file %s is: ", filename);
    printf("%s\n",strHash);
}
if(encprot==CALG_SHA1){
      printf("SHA1 hash of file %s is: ", filename);
    printf("%s\n",strHash);
}

if(encprot==CALG_SHA_256){
      printf("SHA256 hash of file %s is: ", filename);
    printf("%s\n",strHash);
}
    
    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);
    CloseHandle(hFile);

   return strHash;
}   

int main()
{
  LPCSTR GetString;
    TCHAR a[]="C:\\WINDOWS\\system32\\cmd.exe ";
  GetString=a;
    MD5(GetString,CALG_MD5,MD5LEN);
   MD5(GetString,CALG_SHA1,SHA1LEN);
    MD5(GetString,CALG_SHA_256,SHA256LEN);
return 0;
}
