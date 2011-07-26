/*
 * fts-ethernet.c
 *
 *  Created on: Jul 26, 2011
 *      Author: tgrande
 */
#include <stdio.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <stdlib.h>

#include <librouter/options.h>

#include "fts-digistar.h"
#include "fts-ethernet.h"

static int ethernet_wan_test(void)
{
	if (set_ipaddr(WAN_DEV, WAN_IP, WAN_MASK) < 0)
		return -1;

	return ping(HOST_0_PING, WAN_DEV);

}

static int ethernet_lan_test(void)
{
	if (set_ipaddr(LAN_DEV, LAN_IP, LAN_MASK) < 0)
		return -1;

	return ping(HOST_0_PING, LAN_DEV);

}


struct fts_test ethwan_test = {
		.name = "Teste da interface Ethernet WAN",
		.hw_init = NULL,
		.test = ethernet_wan_test,
		.hw_stop = NULL,
		.next = NULL,
};

struct fts_test ethlan_test = {
		.name = "Teste da interface Ethernet LAN",
		.hw_init = NULL,
		.test = ethernet_lan_test,
		.hw_stop = NULL,
		.next = NULL,
};
