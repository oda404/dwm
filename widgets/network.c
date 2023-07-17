
#include "network.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <net/if.h>

#define NETWORK_NAME_SIZE 32
#define NETWORK_IPV4_SIZE 16

int widget_network_init(struct S_Widget *w)
{
	w->active = true;
	widget_network_update(w);

	return 0;
}

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

	for (struct ifaddrs *ifa = ifaddr; ifa; ifa = ifa->ifa_next)
	{
		/* Not connected to a host. */
		if (!ifa->ifa_addr)
			continue;

		/* We only care about non-loopback, IPv4 interfaces. */
		if (ifa->ifa_addr->sa_family != AF_INET || ifa->ifa_flags & IFF_LOOPBACK)
			continue;

		/* Just use the first one we hit :) */
		avail_ifa = ifa;
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

	snprintf(w->text, WIDGET_TEXT_MAXLEN, "%s (%s)", avail_ifa->ifa_name, host);

quit:
	freeifaddrs(ifaddr);
	return true;
}

void widget_network_destroy(struct S_Widget *w)
{
}
