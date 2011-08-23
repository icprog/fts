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

	printf("$TResetando DSP ...");
	if (librouter_efm_reset() < 0) {
		printf("[FALHA]\n");
		return -1;
	}
	printf("[OK]\n");

	printf("$TInicializando DSP... ");
	/* Assure that the DSP is disabled */
	if (librouter_efm_enable(0) < 0)
		return -1;

	sleep(2);

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

	printf("$TEsperando conexao dos canais SHDSL ...");
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

	printf("[OK]\n");
	printf("$TConexao concluida em %d segundos\n", EFM_MAX_CONNECTION_TIME - i);

	for (i = 0; i < 4; i++) {
		printf("$TTaxa da linha do canal %d: %dkbps ... ", i, stat[0].bitrate[0]);
		if (stat[0].bitrate[0] != EFM_SHDSL_LINERATE) {
			printf("[FALHA]\n");
			return -1;
		} else
			printf("[OK]\n");
	}

	printf("$TVerificando estabilidade do link ...");
	i = 5;
	while (--i) {
		char msk = 0x0;
		int k;

		if (librouter_efm_get_status(stat) < 0)
			return -1;

		msk = 0;
		for (k = 0; k < 4; k++) {
			msk |= (stat[k].channel_st == CHANNEL_STATE_CONNECTED) ? (0x1 << k) : 0;
		}

		if (msk == 0xf) /* All 4 channels connected! */
			sleep(1);
		else {
			printf("[FALHA]\n");
			return -1;
		}
	}

	printf("[OK]\n");

	return 0;
}

#if 0
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
#endif

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

static int efm_do_test(void)
{
	int ret = -1;
	int n = 1000;
	int i;
	int err = 0;

	if (set_ipaddr(EFM_DEV, EFM_IPADDR, EFM_IPMASK) < 0)
		return -1;

	for (i = 0; i < 60; i++) {
		if (ping(EFM_IPDEST, EFM_DEV, 100) == 0)
			break;
		sleep(1);
	}

	printf("$TTempo de espera até primeiro ping voltar: %d segundos\n", i);

	while (--n) {
		 ret = ping(EFM_IPDEST, EFM_DEV, 64 + n);
		 if (ret < 0) {
			 err++;
		 }

		 if (err > EFM_MAX_ACCEPTED_ERRORS)
			 break;

		 ret = 0;
	 }


	 if (ret < 0)
		 printf("ERRO com ping tamanho %d\n", n + 64);

	 return ret;
}

static int efm_hw_stop(void)
{
	/* Disable DSP */
	printf("$TDesativando DSP ...");
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
		.test = efm_do_test,
		.hw_stop = efm_hw_stop,
		.next = NULL,

};

#endif /* OPTION_EFM */
