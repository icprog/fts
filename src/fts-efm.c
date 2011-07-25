/*
 * fts-efm.c
 *
 *  Created on: Jul 25, 2011
 *      Author: tgrande
 */
#include <stdio.h>
#include <unistd.h>

#include <librouter/options.h>
#include <librouter/efm.h>

#include "fts-efm.h"

#ifdef OPTION_EFM

/**
 * do_dsp_connection
 *
 * Configure DSP and establish EFM connection with a CO device
 *
 */
int do_dsp_connection(void)
{
	/* Assure that the DSP is disabled */
	if (librouter_efm_enable(0) < 0)
		return -1;

	/* Set CPE mode */
	if (librouter_efm_set_mode(GTI_CPE) < 0)
		return -1;

	/* Enable DSP connection at last */
	if (librouter_efm_enable(1) < 0)
		return -1;

	return 0;
}


int check_dsp_connection(void)
{
	int i = EFM_MAX_CONNECTION_TIME;
	struct orionplus_stat stat[4];

	printf("Esperando conexao dos canais SHDSL\n");

	while(--i) {
		char msk = 0x0;
		int k;

		if (librouter_efm_get_status(stat) < 0)
			return -1;

		for (k=0; k < 4; k++) {
			msk = (stat[k].channel_st == CHANNEL_STATE_CONNECTED) ? (0x1 << k) : 0;
		}

		if (msk == 0xf) /* All 4 channels connected! */
			break;

		sleep(1);
	}

	if (i == 0) {
		printf("Timeout de conexao!\n");
		return -1;
	}

	return 0;
}

int check_dsp_parameters(void)
{
	struct orionplus_counters cnt;
	struct orionplus_stat stat[4];
	int i;

	for (i=0; i<4; i++) {
		do {
		if (librouter_efm_get_counters(&cnt) < 0)
			return -1;
		} while (cnt.xcvr_cnt[i].uptime < 16);
	}

	return 0;
}

#endif /* OPTION_EFM */
