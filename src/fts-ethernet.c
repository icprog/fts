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

static int ethernet_test(char *dev, char *ipaddr, char *mask, char *ipdest)
{
	int ret = -1;
	int n = 500;

	if (set_ipaddr(dev, ipaddr, mask) < 0)
		return -1;

	 while (((ret = ping(ipdest, dev, 64 + n)) == 0) && --n) ;

	 if (ret < 0)
		 printf("ERRO com ping tamanho %d\n", n + 64);
	 return ret;

}

static int ethernet_wan_test(void)
{
	return ethernet_test(WAN_DEV, WAN_IP, WAN_MASK, HOST_0_PING);
}

static int ethernet_lan_test(void)
{
	return ethernet_test(LAN_DEV, LAN_IP, LAN_MASK, HOST_0_PING);
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
