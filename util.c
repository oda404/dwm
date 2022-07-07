/* See LICENSE file for copyright and license details. */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "util.h"

void *
ecalloc(size_t nmemb, size_t size)
{
	void *p;

	if (!(p = calloc(nmemb, size)))
		die("calloc:");
	return p;
}

void
die(const char *fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	if (fmt[0] && fmt[strlen(fmt)-1] == ':') {
		fputc(' ', stderr);
		perror(NULL);
	} else {
		fputc('\n', stderr);
	}

	exit(1);
}

bool exec_cmd(const char *cmd, char *buf, size_t n)
{
	FILE *fp = popen(cmd, "r");
	if(!fp)
		return false;
	
	if(buf)
	{
		fgets(buf, n, fp);
		size_t buflen = strlen(buf);
		for(size_t i = 0; i < buflen; ++i)
		{
			if(iscntrl(buf[i]))
			{
				buf[i] = '\0';
				break;
			}
		}
	}
		
	pclose(fp);

	return true;
}
