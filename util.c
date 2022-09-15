/* See LICENSE file for copyright and license details. */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "util.h"

void *
ecalloc(size_t nmemb, size_t size)
{
	void *p;

	if (!(p = calloc(nmemb, size)))
		die("calloc:");
	return p;
}

void die(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	if (fmt[0] && fmt[strlen(fmt) - 1] == ':')
	{
		fputc(' ', stderr);
		perror(NULL);
	}
	else
	{
		fputc('\n', stderr);
	}

	exit(1);
}

int exec_cmd(const char *cmd, char *buf, size_t n)
{
	FILE *fp = popen(cmd, "r");
	if (!fp)
		return errno;

	if (buf)
	{
		fgets(buf, n, fp);
		size_t buflen = strlen(buf);
		for (size_t i = 0; i < buflen; ++i)
		{
			if (iscntrl(buf[i]))
			{
				buf[i] = '\0';
				break;
			}
		}
	}

	return pclose(fp);
}

double timeval_to_ms(const struct timeval *tv)
{
	return tv->tv_sec * 1000 + tv->tv_usec / 1000.0;
}

double timeval_to_sec(const struct timeval *tv)
{
	return tv->tv_sec + tv->tv_usec / 1000000.0;
}
