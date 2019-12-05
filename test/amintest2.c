
#include "syscall.h"

int
main()
{
    int a;
    char* x="abcdefg";
    a=Open("aaaaa",0);
    Read(x,4,a);
    Remove("aaaaa");

}
