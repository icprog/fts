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

#include "fts-digistar.h"
#include "fts-flash.h"

static int flash_tester(void)
{
#if defined(CONFIG_DIGISTAR_3G)


#elif defined(CONFIG_DIGISTAR_EFM)

#else
#error "Board not suppoted"
#endif
}

struct fts_test flash_test = {
		.name = "Teste da Flash",
		.hw_init = NULL,
		.test = flash_tester,
		.hw_stop = NULL,
		.next = NULL,
};
