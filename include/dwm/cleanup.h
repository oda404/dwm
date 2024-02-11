
#ifndef _DXGMX_CLEANUP_H
#define _DXGMX_CLEANUP_H

#include <stdio.h>

#define _CLEANUP(_f) __attribute__((cleanup(_f)))

/* p is in fact void**, but we can't declare it as such because the compiler
 * doesn't like automatically converting for example: int** to void** when
 * calling this function (rightfully so). In this case it's ok since we
 * know it's a pointer to some other pointer that's been allocated with
 * kmalloc, that is if you haven't called this function on bogus */
void _cleanup_malloc_ptr(void *p);
void _cleanup_fopen_ptr(FILE **f);

#define _CLEANUP_FREE _CLEANUP(_cleanup_malloc_ptr)
#define _CLEANUP_FCLOSE _CLEANUP(_cleanup_fopen_ptr)

#endif // !_DXGMX_CLEANUP_H