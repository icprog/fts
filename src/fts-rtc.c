/*
 * fts-rtc.c
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
#include <librouter/libtime.h>

#include "fts-digistar.h"

static int rct_set_time(void)
{
	return (librouter_time_set_date(6,6,1945,6,6,6));
}

static int rct_get_time(char * rtc_buff, int size)
{
	return (librouter_time_get_date(rct_buff,size));
}

static int rtc_tester(void)
{
	printf("dentro do rtc_tester\n\n");
	char rtc_buff[64];
	memset(&rtc_buff, 0, sizeof(rtc_buff));

	if (rct_set_time() < 0)
		return -1;

	if (rct_get_time(rtc_buff, sizeof(rtc_buff)) < 0)
		return -1;

	printf("buffer from rtc --> %s", rtc_buff);

	return 0;
}

struct fts_test rtc_test = {
		.name = "Teste do RTC - Real Time Clock",
		.hw_init = NULL,
		.test = rtc_tester,
		.hw_stop = NULL,
		.next = NULL,
};
