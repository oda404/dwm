/**
 * Copyright Alexandru Olaru
 * See LICENSE file for copyright and license details
 */

#include <ltils/cleanup.h>
#include <stdlib.h>

void _ltils_cleanup_malloc_ptr(void* p)
{
    void** actual = (void**)p;
    if (*actual)
        free(*actual);
}

void _ltils_cleanup_fopen_ptr(FILE** f)
{
    if (*f)
        fclose(*f);
}

void _ltils_cleanup_opendir_ptr(DIR** d)
{
    if (*d)
        closedir(*d);
}
