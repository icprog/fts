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

#include <librouter/options.h>

#include "fts-digistar.h"
#include "fts-modem3g.h"

#if defined (OPTION_MODEM3G)


static int modem3g_tester(void)
{
	printf("[OK] - P1 - Not yet Implemented\n");
	return 0;
}

static int modem3g_test_init(void)
{
	printf("[OK] - P2 - Not yet Implemented\n");
	return 0;
}

static int modem3g_test_exit(void)
{
	printf("[OK] - P3 - Not yet Implemented\n");
	return 0;
}

struct fts_test modem3g_test = {
		.name = "Teste do Modem 3G Interno",
		.hw_init = modem3g_test_init,
		.test = modem3g_tester,
		.hw_stop = modem3g_test_exit,
		.next = NULL,
};

#endif
