/* add.c
 *	Simple program to test whether the systemcall interface works.
 *	
 *	Just do a add syscall that adds two values and returns the result.
 *
 */

#include "syscall.h"

int
main() {
    int result;
    int a;
    char* x="abcd";

    result = Add(42, 23);

    a = Create("salam", 111);
    Write(x, 4, a);


    //Halt();
    /* not reached */
}
