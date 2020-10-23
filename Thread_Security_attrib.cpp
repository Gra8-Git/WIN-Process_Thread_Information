#define _WIN32_WINNT 0x502
#define _WINVER 0x500
#define SDDL_REVISION_1 1
#include <iostream>
#include <windows.h>

/*
typedef struct _SECURITY_ATTRIBUTES
{
    DWORD nLength;                     size of the structure  sizeof 
    LPVOID lpSecurityDescriptor;       have by default NULL ALL ACCESS or pointer to SECURITY_DESCRIPTOR structure
    BOOL bInheritHandle;               if new process created inherid handler  TRUE FALSE
} SECURITY_ATTRIBUTES, *PSECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;

*/

using namespace std;
DWORD WINAPI myThread(LPVOID lpParameter)
{
    unsigned int &myCounter = *((unsigned int *)lpParameter);
    while (myCounter < 0xFFFFFFFF)
        ++myCounter;
    return 0;
}

int main(int argc, char *agrv[])
{

    int myCounter = 0;
    DWORD myThreadID;
    SECURITY_ATTRIBUTES SAT;

    //Initialize SECURITY_ATTRIBUTES structure
    SAT.bInheritHandle = TRUE;
    SAT.nLength = sizeof(SECURITY_ATTRIBUTES);

    SAT.lpSecurityDescriptor = NULL;
    HANDLE myHandle = CreateThread(&SAT, 0, myThread, &myCounter, 0, &myThreadID);
    char myChar = ' ';
    while (myChar != 'q')
    {

        cout << myCounter << "\n";
        myChar = getchar();
    }
    CloseHandle(myHandle);

    return 0;
}
