#include "syscall.h"

int
main()
{
  int a;
  char* h="abcdefg";
  
  Create("aaaaa",101,100);
  a=Open("aaaaa",1);
  Write("ABCD",4,a);
  Seek(2,a);

}
