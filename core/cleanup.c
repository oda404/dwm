
#include <dwm/cleanup.h>
#include <stdlib.h>
#include <stdlib.h>

void _cleanup_malloc_ptr(void *p)
{
    void **actual = (void **)p;
    if (*actual)
        free(*actual);
}

void _cleanup_fopen_ptr(FILE **f)
{
    if (*f)
        fclose(*f);
}