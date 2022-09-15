
#include "network.h"
#include "../util.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define NETWORK_NAME_SIZE 32
#define NETWORK_IPV4_SIZE 16
#define NETWORK_BUF_SIZE (NETWORK_NAME_SIZE + NETWORK_IPV4_SIZE + 8)

static const char *network_get_info_cmd = "ip route get 8.8.8.8";

int widget_network_init(struct S_Widget *w)
{
	w->active = true;
	widget_network_update(w);

	return true;
}

bool widget_network_update(struct S_Widget *w)
{
	char info[NETWORK_BUF_SIZE];

	if (exec_cmd(network_get_info_cmd, info, NETWORK_BUF_SIZE) != 0)
	{
		strcpy(w->text, "disconnected");
		return true;
	}

	char *interfaceloc = strstr(info, "dev ");
	if (!interfaceloc)
	{
		strcpy(w->text, "disconnected");
		return true;
	}

	interfaceloc += 4;

	char ip[NETWORK_IPV4_SIZE] = {0};

	{
		char *iploc = strstr(info, "src ");
		if (iploc)
		{
			iploc += 4;
			for (size_t i = 0; i < NETWORK_IPV4_SIZE; ++i)
			{
				if (iploc[i] == ' ')
				{
					ip[i] = '\0';
					break;
				}

				ip[i] = iploc[i];
			}
		}
		else
		{
			strncpy(ip, "???", NETWORK_IPV4_SIZE);
		}
	}

	/* Wireless interfaces start with a 'w'. */
	const bool wired = (interfaceloc[0] != 'w');

	if (wired)
	{
		w->icon = " ";
		strncpy(w->text, ip, NETWORK_BUF_SIZE);
	}
	else
	{
		w->icon = " ";

		char interface[NETWORK_NAME_SIZE] = {0};
		for (size_t i = 0; i < NETWORK_NAME_SIZE; ++i)
		{
			if (interfaceloc[i] == ' ')
			{
				interface[i] = '\0';
				break;
			}

			interface[i] = interfaceloc[i];
		}

		char network_name_cmd[128] = {0};
		snprintf(
			network_name_cmd,
			128,
			"iwctl station %s show | grep 'Connected network' | sed 's/Connected network//' | sed -r 's/\\s+//g'",
			interface);

		char name[NETWORK_NAME_SIZE];
		if (exec_cmd(network_name_cmd, name, NETWORK_NAME_SIZE) != 0)
			strncpy(name, "???", NETWORK_NAME_SIZE);

		snprintf(w->text, NETWORK_BUF_SIZE, "%s (%s)", name, ip);
	}

	return true;
}

void widget_network_destroy(struct S_Widget *w)
{
}
