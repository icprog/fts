/*
 * fts-efm.c
 *
 *  Created on: Jul 25, 2011
 *      Author: tgrande
 */
#include <stdio.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <stdlib.h>

#include <librouter/options.h>
#ifdef OPTION_EFM
#include <librouter/efm.h>
#endif

#include "fts-digistar.h"
#include "fts-efm.h"

#ifdef OPTION_EFM

/**
 * do_dsp_connection
 *
 * Configure DSP and establish EFM connection with a CO device
 *
 */
static int do_dsp_connection(void)
{
	struct orionplus_conf conf = {
		.mode = GTI_CPE,
	};

	printf("Inicializando DSP... ");
	/* Assure that the DSP is disabled */
	if (librouter_efm_enable(0) < 0)
		return -1;

	/* Set CPE mode */
	if (librouter_efm_set_mode(&conf) < 0)
		return -1;

	/* Enable DSP connection at last */
	if (librouter_efm_enable(1) < 0)
		return -1;

	printf("[OK]\n");

	return 0;
}


static int check_dsp_connection(void)
{
	int i = EFM_MAX_CONNECTION_TIME;
	struct orionplus_stat stat[4];

	printf("Esperando conexao dos canais SHDSL ...");
	fflush(stdout);

	while(--i) {
		char msk = 0x0;
		int k;

		if (librouter_efm_get_status(stat) < 0)
			return -1;

		msk = 0;
		for (k=0; k < 4; k++) {
			msk |= (stat[k].channel_st == CHANNEL_STATE_CONNECTED) ? (0x1 << k) : 0;
		}

		if (msk == 0xf) /* All 4 channels connected! */
			break;

		sleep(1);
	}

	if (i == 0) {
		printf("[TIMEOUT]\n");
		return -1;
	}

	sleep(2); /* Wait link to settle */

	printf("[OK]\n");

	return 0;
}

static int check_dsp_parameters(void)
{
	struct orionplus_counters cnt;
	int i;

	for (i=0; i<4; i++) {
		do {
		if (librouter_efm_get_counters(&cnt) < 0)
			return -1;
		} while (cnt.xcvr_cnt[i].uptime < 16);
	}

	return 0;
}

static int efm_hw_init(void)
{
	if (do_dsp_connection() < 0) {
		return -1;
	}

	if (check_dsp_connection() < 0) {
		return -1;
	}

	return 0;
}

#define EFM_IPADDR "15.0.0.1"
#define EFM_IPMASK "255.0.0.0"
#define EFM_DEV "eth1"
#define EFM_IPDEST "15.0.0.2"

static int efm_do_test(void)
{
	int ret = -1;
	int n = 500;

	if (set_ipaddr(EFM_DEV, EFM_IPADDR, EFM_IPMASK) < 0)
		return -1;

	 while (((ret = ping(EFM_IPDEST, EFM_DEV, 64 + n)) == 0) && --n)
		 ;

	 if (ret < 0)
		 printf("ERRO com ping tamanho %d\n", n + 64);

	 return ret;
}

static int efm_hw_stop(void)
{
	/* Disable DSP */
	printf("Desativando DSP ...");
	if (librouter_efm_enable(0) < 0) {
		printf("[FALHA]\n");
		return -1;
	}

	printf("[OK]\n");

	return 0;
}

struct fts_test efm_test = {
		.name = "Teste da interface EFM",
		.hw_init = efm_hw_init,
		//.hw_init = NULL,
		.test = efm_do_test,
		.hw_stop = efm_hw_stop,
};

#endif /* OPTION_EFM */
