#include <windows.h>
#include <tchar.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>

using namespace std;
#define BUFSIZE MAX_PATH

struct HEX_STRING {
    string hex_line;
    string str_line;
};
template<typename iter_t>
std::string bytes_to_hex(iter_t begin, iter_t const& end)
{
    std::ostringstream hex;
    hex << std::hex;
    while (begin != end){
       hex<< std::hex<< std::setw(2) << std::setfill('0')<<static_cast<unsigned>(*begin++)<<' ';     
        }
    return hex.str();
}

template<typename iter_t>
std::string bytes_to_string(iter_t begin, iter_t const& end)
{
    std::ostringstream hex;
    hex << std::hex;
    while (begin != end){
       hex<<static_cast<unsigned char>(*begin++)<<' ';     
        }
    return hex.str();
}

std::string find_all_compares(std::string ch, std::string find_hex_infile)
{
     std::ostringstream chout;
    int i = 0;
    size_t pos =ch.find(find_hex_infile,0) ;
    while(pos!= string::npos) {
    i++;
    chout<<i<<":  "<<find_hex_infile.c_str()<<"\n";
    pos=ch.find(find_hex_infile,pos+1);
    }
    return chout.str();
}

HEX_STRING file_append_log(const char* result)
{
    HEX_STRING hstr;
    char buff[BUFSIZE]={0};
    std::string ch;
    HANDLE hAppend=NULL;
    DWORD  dwBytesRead, dwBytesWritten, dwPos;
     // create a new file.
    DWORD dwOldProt, bytes;
	// open it and get the size
	HANDLE hFile = CreateFile(result, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if (!hFile)
    {
		printf("Unable to open file\n");
	}
	DWORD dwSize = GetFileSize(hFile, 0);
	// load in memory
	LPBYTE fileB = (LPBYTE) malloc(dwSize);
	while(bytes!= dwSize)
    {
        ReadFile(hFile, fileB, dwSize, &bytes, 0);
        if(fileB==NULL || dwSize==0)
        {
            printf("buffer NULL");
        }
        std::vector<BYTE> buffer(fileB, fileB+dwSize);
        hstr.hex_line=bytes_to_hex(buffer.begin(),buffer.end());
        hstr.str_line=bytes_to_string(buffer.begin(),buffer.end());
    }

    CloseHandle(hFile);
    return hstr;
}

int main(int argc, TCHAR *argv[])
{
    HEX_STRING hstr;
const int MAX_FILEPATH = 255;
char fileName[MAX_FILEPATH] = {0};
std::string find1;
std::string file;
std::ostringstream hex_c;

  for(int i = 1; i < argc;i++)
	{
        if( lstrcmpi( argv[i], TEXT("File:")) == 0 )
        {
	        memcpy_s(&fileName, MAX_FILEPATH, argv[i+1], MAX_FILEPATH);
        }
        if( lstrcmpi( argv[i], TEXT("-h")) == 0 )
        {
                for(int j = i; j < argc;j++)
	            {
                      hex_c<<argv[j]<<" ";      
                }

                find1=hex_c.str();
                unsigned first = find1.find("[");
                unsigned last = find1.find("]");
                
                hstr=file_append_log(fileName);
                file=hstr.hex_line;
                std::cout<<find_all_compares(file,find1.substr(first+1, last-first-1));
        }
        if( lstrcmpi( argv[i], TEXT("HEX_all")) == 0 )
        {
            
            hstr=file_append_log(fileName);
            file=hstr.hex_line;
            std::cout<<file;
        } 


    }
   


    return 0;
}
