/* See LICENSE file for copyright and license details. */

#ifndef DWM_UTIL_H
#define DWM_UTIL_H

#include <stdbool.h>
#include <stddef.h>
#include <sys/time.h>

#define MAX(A, B) ((A) > (B) ? (A) : (B))
#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define BETWEEN(X, A, B) ((A) <= (X) && (X) <= (B))

[[noreturn]] void die(const char *fmt, ...);
void *ecalloc(size_t nmemb, size_t size);

int exec_cmd(const char *cmd, char *buf, size_t n);

double timeval_to_ms(const struct timeval *);
double timeval_to_sec(const struct timeval *);

char *path_normalize_alloc(const char *basepath);

#endif // !DWM_UTIL_H
