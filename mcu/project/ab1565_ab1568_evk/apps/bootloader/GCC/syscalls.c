#include <sys/stat.h>
#include "bl_common.h"

int printf (__const char *__restrict __format, ...)
{
     bl_print(LOG_DEBUG, (char *)__format);
     return 0;
}

void __assert_func(const char *file, int line, const char *func, const char *failedexpr)
{
    for (;;);
}

void platform_assert(const char *expr, const char *file, int line)
{
    for (;;);
}

