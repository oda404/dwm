
#include "network.h"
#include "../util.h"
#include <string.h>
#include <stdlib.h>

#define NETWORK_BUF_SIZE 64
#define NETWORK_NAME_SIZE 32
#define NETWORK_IPV4_SIZE 16

static const char *network_get_name_cmd = "iwctl station wlan0 show | grep 'Connected network' | sed 's/Connected network//' | sed -r 's/\\s+//g'";
static const char *network_get_ipv4_cmd = "iwctl station wlan0 show | grep 'IPv4 address' | sed 's/IPv4 address//' | sed -r 's/\\s+//g'";

bool widget_network_init(struct S_Widget *w)
{
	if(!(w->text = malloc(NETWORK_BUF_SIZE)))
	{
		w->active = false;
		return false;
	}

	w->active = true;
	widget_network_update(w);

	return true;
}

bool widget_network_update(struct S_Widget *w)
{
	char network[NETWORK_BUF_SIZE];
	char ipv4[NETWORK_IPV4_SIZE];

	exec_cmd(network_get_name_cmd, network, NETWORK_NAME_SIZE);
	network[strnlen(network, NETWORK_NAME_SIZE) - 1] = '\0';

	if(!strlen(network))
	{
		strcpy(w->text, "disconnected");
		w->should_redraw = true;
		return true;
	}
	
	exec_cmd(network_get_ipv4_cmd, ipv4, NETWORK_IPV4_SIZE);
	ipv4[strnlen(ipv4, NETWORK_IPV4_SIZE) - 1] = '\0';

	strcpy(w->text, network);
	strcat(w->text, " - ");
	strcat(w->text, ipv4);

	w->should_redraw = true;

	return true;
}

void widget_network_destroy(struct S_Widget *w)
{
	if(w->text)
		free(w->text);
}
