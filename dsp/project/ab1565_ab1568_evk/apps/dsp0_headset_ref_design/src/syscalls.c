#include "exception_handler.h"
#include <string.h>
#include <stdlib.h>


/* NOTE:
 * Because libc.a contains two functions, _Assert and _exit,
 * to avoid duplication of definitions, -wrap=_Assert and -wrap=_exit are added.
 */


void __wrap__Assert(const char *mesg, const char *fun)
{
    (void)(fun);
    char *substr = NULL, *saveptr = NULL;
    char *file = NULL, *line = NULL;

    substr = strtok_r((char*)mesg, ":", &saveptr);
    file = substr;

    substr = strtok_r(NULL, " ", &saveptr);
    line = substr;

    platform_assert("0", file, atoi(line));
    while(1){;}
}
/*
 * stub funs for xclib
 * only inbyte/outbyte/_exit are needed according to ./XtensaTools/xtensa-elf/src/libgloss/gloss.h
*/
void __wrap__exit(int status)
{
    (void)(status);

    platform_assert("exit", __FILE__, __LINE__);

    for (;;);
}

char inbyte(void)
{
    /*
     * not implement, please directly use hal_uart interface
    */
    platform_assert("not implement", __FILE__, __LINE__);
    return (char)0;
}

int outbyte(char c)
{
    (void)(c);
    /*
     * not implement, please directly use hal_uart interface
    */
    platform_assert("not implement", __FILE__, __LINE__);
    return 0;
}
