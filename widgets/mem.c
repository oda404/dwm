
#include "mem.h"
#include <unistd.h>
#include <stdio.h>
#include <dwm/types.h>
#include <dwm/colors.h>

int widget_mem_init(struct S_Widget *w)
{
	if (access("/proc/meminfo", F_OK) != 0)
		return -1;

	widget_mem_update(w);
	return 0;
}

bool widget_mem_update(struct S_Widget *w)
{
	FILE *f = fopen("/proc/meminfo", "r");
	if (!f)
	{
		widget_crashed_and_burned(w);
		return false;
	}

	char buf[128];
	size_t entries_read = 0;
	u64 mem_total = 0;
	u64 mem_free = 0;
	u64 buffers = 0;
	u64 cached = 0;
	u64 sreclaimable = 0;
	u64 shmem = 0;

	/* Read 64 lines in which we should find the 6 we need */
	while (entries_read < 6)
	{
		char *rc = fgets(buf, 128, f);
		if (!rc)
		{
			fclose(f);
			widget_crashed_and_burned(w);
			return false;
		}

		int st;
		if (strstr(buf, "MemTotal:") == buf)
		{
			st = sscanf(buf, "MemTotal: %lu kB\n", &mem_total);
			++entries_read;
		}
		else if (strstr(buf, "MemFree:") == buf)
		{
			st = sscanf(buf, "MemFree: %lu kB\n", &mem_free);
			++entries_read;
		}
		else if (strstr(buf, "Buffers:") == buf)
		{
			st = sscanf(buf, "Buffers: %lu kB\n", &buffers);
			++entries_read;
		}
		else if (strstr(buf, "Cached:") == buf)
		{
			st = sscanf(buf, "Cached: %lu kB\n", &cached);
			++entries_read;
		}
		else if (strstr(buf, "SReclaimable:") == buf)
		{
			st = sscanf(buf, "SReclaimable: %lu kB\n", &sreclaimable);
			++entries_read;
		}
		else if (strstr(buf, "Shmem:") == buf)
		{
			st = sscanf(buf, "Shmem: %lu kB\n", &shmem);
			++entries_read;
		}
		else
		{
			continue;
		}

		/* We got a good entry but did we read it ok? */
		if (st != 1)
		{
			fclose(f);
			widget_crashed_and_burned(w);
			return false;
		}
	}

	fclose(f);

	const float mem_total_gb = (float)mem_total / 1024.f / 1024.f;
	const float mem_used_gb = (float)(mem_total - mem_free - (buffers + (cached + sreclaimable - shmem))) / 1024.f / 1024.f;

	const float mem_total_third = mem_total_gb / 3;
	if (mem_used_gb >= mem_total_third * 2)
	{
		w->bgcolor = col_red;
		w->fgcolor = col_gray3;
	}
	else if (mem_used_gb >= mem_total_third * 1 /* duh */)
	{
		w->bgcolor = col_yellow;
		w->fgcolor = col_purple1;
	}
	else
	{
		w->bgcolor = col_green;
		w->fgcolor = col_purple1;
	}

	widget_snprintf_text(w, "%.1f/%.1f GiB", mem_used_gb, mem_total_gb);
	return true;
}

void widget_mem_destroy(struct S_Widget *w)
{
}
