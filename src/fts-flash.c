/*
 * fts-flash.c
 *
 *  Created on: Jul 26, 2011
 *      Author: ipinotti
 */
#include <stdio.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <stdlib.h>

#include <librouter/options.h>
#include <librouter/nv.h>

#include "fts-digistar.h"
#include "fts-flash.h"

static int flash_tester(void)
{
	printf("Carregando configuracao [default] para flash\n");
	if (librouter_nv_save_configuration(DEV_DEFAULT_CONFIG) < 0)
		return -1;

	return 0;
}

struct fts_test flash_test = {
		.name = "Teste da Flash",
		.hw_init = NULL,
		.test = flash_tester,
		.hw_stop = NULL,
		.next = NULL,
};
