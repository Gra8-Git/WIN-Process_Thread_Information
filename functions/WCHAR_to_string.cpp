//just template for working function not tested ;-)
//

#include<iostream>
#include<string>
#include<vector>
#define slength 2000
std::string WCHAR_to_String(WCHAR *wch)
{
    std::string str;
    //convert from WCHAR
    char  ch[slength];
    memset(ch, '\0', slength);
    wcstombs(ch, wch, 2000);
    str=ch;
    //return line without wchar memory junk
    return str;
}

int main (void)
{
WCHAR hello[MAX_PATH];
std::string hello{0};
hello=WCHAR_to_String(hello);


return 0;
}
