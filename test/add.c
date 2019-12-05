/* add.c
 *	Simple program to test whether the systemcall interface works.
 *	
 *	Just do a add syscall that adds two values and returns the result.
 *
 */

#include "syscall.h"

int
main()
{
  int a;
  char* h="abcdefg";
  
  Create("aaaaa",111,100);
  a=Open("aaaaa",1);
  Write("ABCD",4,a);
  Seek(2,a);
  //a=Open("salam",1);
  //Read(h,4,a);
  
  //result = Add(42, 23);

  //Halt();
  /* not reached */
}
