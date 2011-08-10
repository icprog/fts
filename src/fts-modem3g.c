/*
 * fts-modem3g.c
 *
 *  Created on: Aug 3, 2011
 *      Author: ipinotti
 */

#include <stdio.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

#include <linux/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <librouter/options.h>
#include <librouter/modem3G.h>
#include <librouter/dev.h>
#include <librouter/usb.h>

#include "fts-digistar.h"
#include "fts-modem3g.h"

#if defined (OPTION_MODEM3G)

static int modem3g_verify_usb()
{
	if (!librouter_usb_device_is_connected(MODEM3G_USB_PORT)){
		printf(" - [FAIL]\n");
		return -1;
	}

	return 0;
}

static int modem3g_test_ping(char *dev, char *ipdest)
{
	int ret = -1;
	int n = 500;

	while (n){
		 if ( (ret = ping(ipdest, dev, 64 + n)) == 0)
			 break;
		 n--;
	}

	if (ret < 0){
		printf(" - [FAIL]\n");
		printf("%% ERRO com ping tamanho %d\n", n + 64);
	}
	else
		printf(" - [OK]\n");

	return ret;
}

static int modem3g_verify_link(void)
{
	int time = MAX_WAIT_TIME_CONNECTION_3G;

	while (time){
		if (librouter_dev_exists(MODEM3G_BUILTIN) && (librouter_dev_get_link(MODEM3G_BUILTIN) == IFF_UP)){
			printf("\n");
			return 0;
		}
		time--;
		printf(".");
		sleep(1);
	}
	printf(" - [FAIL]\n");
	return -1;
}

static int modem3g_start_connection(int scard)
{
	char cmd[128];

	memset(cmd, 0, sizeof(cmd));

	sprintf(cmd, "/bin/pppd call FTS_sim%d_modem-3g-0",scard);
	if (system(cmd) != 0)
		return -1;

	return 0;
}

static int get_connection_pid(void)
{
	FILE *fd;
	char line[32] = { (int) NULL };

	if ((fd = fopen(MODEM3G_PPPD_PID, "r")) == NULL) {
			syslog(LOG_ERR, "Could not open configuration file\n");
			return -1;
	}

	while (fgets(line, sizeof(line), fd) != NULL);

	return (atoi(line));
}

static int modem3g_stop_connection(void)
{
	int pid = 0;
	char cmd[128];
	memset(cmd, 0, sizeof(cmd));

	if ((pid = get_connection_pid()) < 0)
		return -1;

	sprintf(cmd, "/bin/kill -15 %d",pid);
	if (system(cmd) != 0)
		return -1;

	return 0;
}

static int modem3g_tester(void)
{
	printf("Iniciando teste do Modem3G...");
	if (modem3g_test_ping(MODEM3G_BUILTIN, IP_DEST_TEST_3G) < 0){
		modem3g_stop_connection();
		return -1;
	}

	return 0;
}

static int modem3g_test_init_SC1(void)
{
	int scard = 1;

	printf("Verificando porta USB do Modem3G...");
	if (modem3g_verify_usb() < 0)
		return -1;

	printf(" - [OK]\n");

	printf("Inicializando HW Modem3G / Configuracao de SIM Card...");
	if (librouter_modem3g_sim_card_set(SIM_CARD_P1) < 0)
		return -1;

	printf(" - [OK]\n");

	printf("Inicializando conexao 3G, utilizando SIM Card da porta [%d]...",scard);
	if (modem3g_start_connection(scard) < 0)
		return -1;

	printf(" - [OK]\n");

	printf("Iniciando verificação de Link - ");
	if (modem3g_verify_link() < 0){
		modem3g_stop_connection();
		return -1;
	}
	printf("Link Online - SIM Card na porta [%d]... - [OK]\n", scard);

	return 0;
}

static int modem3g_test_init_SC2(void)
{
	int scard = 2;

	printf("Verificando porta USB do Modem3G...");
	if (modem3g_verify_usb() < 0)
		return -1;

	printf(" - [OK]\n");

	printf("Inicializando HW Modem3G / Configuracao de SIM Card...");
	if (librouter_modem3g_sim_card_set(SIM_CARD_P2) < 0)
		return -1;

	printf(" - [OK]\n");

	printf("Inicializando conexao 3G, utilizando SIM Card da porta [%d]...",scard);
	if (modem3g_start_connection(scard) < 0)
		return -1;

	printf(" - [OK]\n");

	printf("Iniciando verificacao de Link - ");
	if (modem3g_verify_link() < 0){
		modem3g_stop_connection();
		return -1;
	}
	printf("Link Online - SIM Card na porta [%d]... - [OK]\n", scard);

	return 0;
}

static int modem3g_test_exit(void)
{
	printf("Desativando HW Modem3G...");
	if (modem3g_stop_connection() < 0){
		printf(" - [FAIL]\n");
		return -1;
	}

	printf(" - [OK]\n");
	return 0;
}

struct fts_test modem3g_test_sc1 = {
		.name = "Teste do Modem 3G Interno - Sim Card P1",
		.hw_init = modem3g_test_init_SC1,
		.test = modem3g_tester,
		.hw_stop = modem3g_test_exit,
		.next = NULL,
};

struct fts_test modem3g_test_sc2 = {
		.name = "Teste do Modem 3G Interno - Sim Card P2",
		.hw_init = modem3g_test_init_SC2,
		.test = modem3g_tester,
		.hw_stop = modem3g_test_exit,
		.next = NULL,
};
#endif
