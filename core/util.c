/* See LICENSE file for copyright and license details. */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <dwm/log.h>
#include <dwm/util.h>

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

	/* Doesn't work because dwm installs a SIGCHLD >:( */
	return pclose(fp);
}

double timeval_to_ms(const struct timeval *tv)
{
	return (double)tv->tv_sec * 1000.0 + (double)tv->tv_usec / 1000.0;
}

double timeval_to_sec(const struct timeval *tv)
{
	return (double)tv->tv_sec + (double)tv->tv_usec / 1000000.0;
}

char *path_normalize_alloc(const char *basepath)
{
	size_t len = strlen(basepath);
	if (!len)
		return calloc(1, 1);

	u8 usernorm = 0;
	if (len == 1 && basepath[0] == '~')
		usernorm = 1;
	else if (len >= 2 && basepath[0] == '~' && basepath[1] == '/')
		usernorm = 2;

	if (usernorm)
	{
		char *uname = getlogin();
		if (!uname)
		{
			log_print(LOG_ERR, "path_to_normalize_alloc: Failed to get user!");
			return NULL;
		}

		size_t normpath_len = 7 + strlen(uname) + 1 + strlen(basepath) - usernorm + 1;
		char *normpath = malloc(normpath_len);
		if (!normpath)
		{
			log_print(LOG_ERR, "path_to_normalize_alloc: Failed to allocate buffer!");
			return NULL;
		}

		snprintf(normpath, normpath_len, "/home/%s/%s", uname, basepath + usernorm);
		return normpath;
	}

	return strdup(basepath);
}
