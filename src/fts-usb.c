/*
 * fts-usb.c
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
#include "fts-usb.h"

#if defined (OPTION_USB)

static int usb_tester(void)
{
	printf("[OK] - P1 - Not yet Implemented\n");
	return 0;
}


struct fts_test usb_test = {
		.name = "Teste das portas USBs",
		.hw_init = NULL,
		.test = usb_tester,
		.hw_stop = NULL,
		.next = NULL,
};

#endif
