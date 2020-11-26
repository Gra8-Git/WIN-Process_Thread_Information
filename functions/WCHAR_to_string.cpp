//just template for working function not tested ;-)
//

#include<iostream>
#include<string>
#include<vector>

std::string WCHAR_to_String(WCHAR *wch)
{
    std::string str;
    //convert from WCHAR
    char  ch[SLENGTH];
    memset(ch, '\0', SLENGTH);
    wcstombs(ch, wch, 2000);
    str=ch;
    return str;
}

int main (void)
{
WCHAR hello[MAX_PATH];
std::string hello{0};
hello=WCHAR_to_String(hello);


return 0;
}
