#include <windows.h>
#include<iostream>

int main(int argc,LPTSTR argv[])
{
TCHAR *array[argc];

	if(argc < 2)
	{
		 printf("Usage error, Usage: %s file name ...\n",argv[0]);
		return 0;
	}
	for(int i = 1; i < argc;i++)
	{
		array[i]=argv[i];

        printf("%i : %s \n",i,array[i]);
	}
	return 0;
}
