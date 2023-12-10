
#include "network.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <net/if.h>
#include <linux/if_ppp.h>
#include <linux/if_link.h>
#include <dwm/types.h>

#define NETWORK_NAME_SIZE 32
#define NETWORK_IPV4_SIZE 16

float bytes_to_human_readable(u32 bytes, char unit[4])
{
	float fbytes = bytes;
	u8 i = 0;
	while (fbytes >= 1024)
	{
		++i;
		fbytes /= 1024.f;
	}

	if (unit)
	{
		switch (i)
		{
		case 0:
			strcpy(unit, "B");
			break;
		case 1:
			strcpy(unit, "KiB");
			break;
		case 2:
			strcpy(unit, "MiB");
			break;
		case 3:
			strcpy(unit, "GiB");
			break;
		}
	}
	return fbytes;
}

int widget_network_init(struct S_Widget *w)
{
	w->active = true;
	widget_network_update(w);

	return 0;
}

static u32 prev_rx, prev_tx;

bool widget_network_update(struct S_Widget *w)
{
	struct ifaddrs *ifaddr = NULL;
	struct ifaddrs *avail_ifa = NULL;

	char host[NI_MAXHOST] = {0};

	if (getifaddrs(&ifaddr) < 0)
	{
		strncpy(w->text, "No interfaces!", WIDGET_TEXT_MAXLEN);
		return true;
	}

	static u32 tx = 0, rx = 0;
	prev_rx = rx;
	prev_tx = tx;

	for (struct ifaddrs *ifa = ifaddr; ifa; ifa = ifa->ifa_next)
	{
		/* Not connected to a host. */
		if (!ifa->ifa_addr)
			continue;

		/* We only care about non-loopback, IPv4 interfaces. */
		switch (ifa->ifa_addr->sa_family)
		{
		case AF_PACKET:
		{
			if (!ifa->ifa_data)
				break;

			struct rtnl_link_stats *stats = ifa->ifa_data;
			tx = stats->tx_bytes;
			rx = stats->rx_bytes;
			break;
		}
		case AF_INET:
			avail_ifa = ifa;
			break;

		default:
			break;
		}
	}

	if (!avail_ifa)
	{
		strncpy(w->text, "All interfaces down!", WIDGET_TEXT_MAXLEN);
		goto quit;
	}

	size_t structsize = sizeof(struct sockaddr_in);
	int st = getnameinfo(avail_ifa->ifa_addr, structsize, host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);

	if (st != 0)
	{
		strncpy(w->text, "Internal error!", WIDGET_TEXT_MAXLEN);
		goto quit;
	}

	char rx_units[4];
	char tx_units[4];

	float delta_rx = bytes_to_human_readable(rx - prev_rx, rx_units);
	float delta_tx = bytes_to_human_readable(tx - prev_tx, tx_units);

	snprintf(w->text, WIDGET_TEXT_MAXLEN, "%s (%s) | %05.1f %3s  | %05.1f %3s  |", avail_ifa->ifa_name, host, delta_rx, rx_units, delta_tx, tx_units);

quit:
	freeifaddrs(ifaddr);
	return true;
}

void widget_network_destroy(struct S_Widget *w)
{
}
