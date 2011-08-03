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

	sleep(1);

	while (n){
		 if ( (ret = ping(ipdest, dev, 64 + n)) == 0)
			 break;
		 n--;
	}

	if (ret < 0){
		printf(" - [FAIL]\n");
		printf("%% ERRO com ping tamanho %d\n", n + 64);
	}

	printf(" - [OK]\n");

	return ret;
}

static int ethernet_wan_SFP_test(void)
{
	printf("[OK] - Not yet working\n");
	return 0;
}

static int ethernet_wan_test(void)
{
	printf("WAN: Executando Ping para host %s",HOST_0_PING);
	if (ethernet_test(WAN_DEV, WAN_IP, WAN_MASK, HOST_0_PING) < 0)
		return -1;

	return 0;
}

static int ethernet_lan_test(void)
{
#if defined(CONFIG_DIGISTAR_3G)
	printf("LAN: Executando Ping para host %s",HOST_1_PING);
	if (ethernet_test(LAN_DEV, LAN_IP, LAN_MASK, HOST_1_PING) < 0)
		return -1;

	printf("LAN: Executando Ping para host %s",HOST_2_PING);
	if (ethernet_test(LAN_DEV, LAN_IP, LAN_MASK, HOST_2_PING) < 0)
		return -1;

	printf("LAN: Executando Ping para host %s",HOST_3_PING);
	if (ethernet_test(LAN_DEV, LAN_IP, LAN_MASK, HOST_3_PING) < 0)
		return -1;

	printf("LAN: Executando Ping para host %s",HOST_4_PING);
	if (ethernet_test(LAN_DEV, LAN_IP, LAN_MASK, HOST_4_PING) < 0)
		return -1;

	return 0;
#elif defined(CONFIG_DIGISTAR_EFM)
	printf("LAN: Executando Ping para host %s",HOST_1_PING);
	if (ethernet_test(LAN_DEV, LAN_IP, LAN_MASK, HOST_1_PING) < 0)
		return -1;

	return 0;
#else
#error "Board not suppoted"
#endif
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

struct fts_test ethwan_SFP_test = {
		.name = "Teste da interface Ethernet SFP WAN",
		.hw_init = NULL,
		.test = ethernet_wan_SFP_test,
		.hw_stop = NULL,
		.next = NULL,
};
