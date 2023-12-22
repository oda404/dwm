
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
	float fbytes = bytes / 1024.f;
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
			strcpy(unit, "KiB");
			break;
		case 1:
			strcpy(unit, "MiB");
			break;
		case 2:
			strcpy(unit, "GiB");
			break;
		}
	}
	return fbytes;
}

static struct ifaddrs *get_main_ifa(struct ifaddrs *ifaddr)
{
	struct ifaddrs *main_ifa = NULL;
	for (struct ifaddrs *ifa = ifaddr; ifa; ifa = ifa->ifa_next)
	{
		/* Not connected to a host. */
		if (!ifa->ifa_addr)
			continue;

		if (ifa->ifa_flags & IFF_LOOPBACK)
			continue;

		/* We only care about non-loopback, IPv4 interfaces. */
		switch (ifa->ifa_addr->sa_family)
		{
		case AF_INET:
			main_ifa = ifa;
			break;

		default:
			break;
		}
	}
	return main_ifa;
}

static struct ifaddrs *get_packet_ifa_for_inet(struct ifaddrs *ifaddr, struct ifaddrs *inet_ifa)
{
	for (struct ifaddrs *ifa = ifaddr; ifa; ifa = ifa->ifa_next)
	{
		/* Not connected to a host. */
		if (!ifa->ifa_addr)
			continue;

		/* We only care about non-loopback, IPv4 interfaces. */
		switch (ifa->ifa_addr->sa_family)
		{
		case AF_PACKET:
			if (!ifa->ifa_data)
				break;

			if (strcmp(ifa->ifa_name, inet_ifa->ifa_name) != 0)
				break;

			return ifa;

		default:
			break;
		}
	}

	return NULL;
}

static u32 rx, tx, prev_rx, prev_tx;

int widget_network_init(Widget *w)
{
	widget_network_update(w);
	return 0;
}

bool widget_network_update(Widget *w)
{
	prev_rx = rx;
	prev_tx = tx;

	struct ifaddrs *ifaddr = NULL;
	if (getifaddrs(&ifaddr) < 0)
	{
		widget_copy_text(w, "No interfaces!");
		return true;
	}

	struct ifaddrs *avail_ifa = get_main_ifa(ifaddr);
	if (!avail_ifa)
	{
		
		widget_copy_text(w, "All interfaces down!");
		goto quit;
	}

	struct ifaddrs *packet_ifa = get_packet_ifa_for_inet(ifaddr, avail_ifa);
	if (packet_ifa)
	{
		struct rtnl_link_stats *stats = packet_ifa->ifa_data;
		rx = stats->rx_bytes;
		tx = stats->tx_bytes;
	}

	char host[NI_MAXHOST] = {0};
	int st = getnameinfo(
		avail_ifa->ifa_addr,
		sizeof(struct sockaddr_in),
		host,
		NI_MAXHOST,
		NULL,
		0,
		NI_NUMERICHOST);

	if (st != 0)
	{
		widget_copy_text(w, "Internall error!");
		goto quit;
	}

	char rx_units[4];
	char tx_units[4];
	float delta_rx = bytes_to_human_readable(rx - prev_rx, rx_units);
	float delta_tx = bytes_to_human_readable(tx - prev_tx, tx_units);
	widget_snprintf_text(w, "%s (%s) %5.1f %s  %5.1f %s  ", avail_ifa->ifa_name, host, delta_rx, rx_units, delta_tx, tx_units);

quit:
	freeifaddrs(ifaddr);
	return true;
}

void widget_network_destroy(struct S_Widget *w)
{
}
