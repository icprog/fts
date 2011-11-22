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
#include <librouter/bcm53115s.h>
#include <librouter/lan.h>

#include "fts-digistar.h"
#include "fts-ethernet.h"

static int ethernet_test(char *dev, char *ipaddr, char *mask, char *ipdest)
{
	int ret = -1;
	int n = 500;
#if (defined CONFIG_DIGISTAR_3G) || (defined CONFIG_DIGISTAR_MRG)
	struct lan_status st;
	memset(&st, 0, sizeof(struct lan_status));
#endif

	if (set_ipaddr(dev, ipaddr, mask) < 0)
		return -1;

	sleep(5);

#if (defined CONFIG_DIGISTAR_3G) || (defined CONFIG_DIGISTAR_MRG)
	if (librouter_lan_get_status(dev, &st) < 0)
		return -1;
	printf (" - Link [%d]", st.speed);
#endif

	while (n) {
		if ((ret = ping(ipdest, dev, 64 + n)) == 0)
			break;
		n--;
	}

	if (ret < 0) {
		print_test_info("Erro com ping tamanho %d\n", n + 64);
		print_ok_msg(0);
	} else
		print_ok_msg(1);

	return ret;
}

static int ethernet_test_lan(char *dev, char *ipaddr, char *mask, char *ipdest, int port)
{
	int ret = -1;
	int n = 500;

	if (set_ipaddr(dev, ipaddr, mask) < 0)
		return -1;

	sleep(5);

#if (defined CONFIG_DIGISTAR_3G) || (defined CONFIG_DIGISTAR_MRG)
	printf (" - Link [%d]", librouter_bcm53115s_get_port_speed(port));
#endif

	while (n) {
		if ((ret = ping(ipdest, dev, 64 + n)) == 0)
			break;
		n--;
	}

	if (ret < 0) {
		print_test_info("Erro com ping tamanho %d\n", n + 64);
		print_ok_msg(0);
	} else
		print_ok_msg(1);

	return ret;
}

static int ethernet_wan_test(void)
{
	print_test_info("WAN: Executando Ping para host %s", HOST_0_PING);
	if (ethernet_test(WAN_DEV, WAN_IP, WAN_MASK, HOST_0_PING) < 0)
		return -1;

	return 0;
}

static int set_switch_port_enable(int enable, int port)
{
	if (librouter_bcm53115s_set_MII_port_enable(enable, port) < 0)
		return -1;
	sleep(2);
	return 0;
}

static int set_switch_test_port_mode(int enable)
{
	int i = 0;

	enable = !enable;

	for (i = 0; i < LAN_N_PORTS; ++i) {
		if (librouter_bcm53115s_set_MII_port_enable(enable, i) < 0)
			return -1;
	}

	sleep(3);
	return 0;
}

#if 0
/*Função utilizada para verificar estado das portas do switch*/
static int port_state_check()
{
	int i;
	printf("\n_____________\n");
	for (i=0; i < LAN_N_PORTS; i++)
		printf("Port [%d] state = %x\n", i+1, librouter_bcm53115s_get_MII_port_data(i));
	printf("_____________\n");
	return 0;
}
#endif

static int port_switch_test(int port)
{
	int enable = 1, ret = 0;

	print_test_info("LAN: Testando porta [%d]\n", port + 1);
	if (set_switch_port_enable(enable, port) < 0)
		return -1;

	print_test_info("LAN - P%d: Executando Ping para host %s", port + 1, HOST_1_PING);
	ret |= ethernet_test_lan(LAN_DEV, LAN_IP, LAN_MASK, HOST_1_PING, port);
	ret |= set_switch_port_enable(!enable, port);

	if (ret < 0)
		return -1;

	return 0;
}

static int ethernet_lan_test(void)
{
#if (defined CONFIG_DIGISTAR_3G) || (defined CONFIG_DIGISTAR_MRG)
	int enable = 1, ret = 0, i = 0;

	print_test_info("LAN: Configurando modo de teste da LAN. Desativando todas as portas.\n");
	if (set_switch_test_port_mode(enable) < 0)
		return -1;

	for (i = 0; i < LAN_N_PORTS; i++) {
		ret |= port_switch_test(i);
	}

	print_test_info("LAN: Restaurando modo normal de funcionamento da LAN. Ativando todas as portas.\n");
	ret |= set_switch_test_port_mode(!enable);

	if (ret < 0)
		return -1;

	return 0;
#elif defined(CONFIG_DIGISTAR_EFM)
	print_test_info("LAN: Executando Ping para host %s",HOST_1_PING);
	if (ethernet_test(LAN_DEV, LAN_IP, LAN_MASK, HOST_1_PING) < 0)
		return -1;

	return 0;
#elif defined(CONFIG_DIGISTAR_EFM4ETH)
	print_test_info("LAN: Executando Ping para host %s",HOST_1_PING);
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
