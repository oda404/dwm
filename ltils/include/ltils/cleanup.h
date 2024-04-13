/**
 * Copyright Alexandru Olaru
 * See LICENSE file for copyright and license details
 */

#ifndef LTILS_CLEANUP_CLEANUP_H
#define LTILS_CLEANUP_CLEANUP_H

#include <dirent.h>
#include <stdio.h>

#define _LTILS_CLEANUP(_f) __attribute__((cleanup(_f)))

/* p is in fact void**, but we can't declare it as such because the compiler
 * doesn't like automatically converting for example: int** to void** when
 * calling this function (rightfully so). In this case it's ok since we
 * know it's a pointer to some other pointer that's been allocated with
 * kmalloc, that is if you haven't called this function on bogus */
void _ltils_cleanup_malloc_ptr(void* p);
void _ltils_cleanup_fopen_ptr(FILE** f);
void _ltils_cleanup_opendir_ptr(DIR** d);

#define LTILS_CLEANUP_FREE _LTILS_CLEANUP(_ltils_cleanup_malloc_ptr)
#define LTILS_CLEANUP_FCLOSE _LTILS_CLEANUP(_ltils_cleanup_fopen_ptr)
#define LTILS_CLEANUP_CLOSEDIR _LTILS_CLEANUP(_ltils_cleanup_opendir_ptr)

#endif // !LTILS_CLEANUP_CLEANUP_H