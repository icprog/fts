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
#include "fts-rtc.h"

static int rtc_get_time(char * rtc_read, int size)
{
	return (librouter_time_get_date(rtc_read,size));
}

static int rtc_set_time(char * rtc_loaded, int size)
{
	if (librouter_time_set_date(RTC_DAY,RTC_MONTH,RTC_YEAR,RTC_HOUR,RTC_MIN,RTC_SEC) < 0)
		return -1;

	if (rtc_get_time(rtc_loaded,size) < 0)
		return -1;

	return 0;
}

static int rtc_compare_time(char *rtc_loaded, char * rtc_read)
{
	if (!strcmp(rtc_loaded,rtc_read))
		return -1;
	return 0;
}

static int rtc_tester(void)
{
	char rtc_loaded[64];
	char rtc_read[64];
	memset(&rtc_loaded, 0, sizeof(rtc_loaded));
	memset(&rtc_read, 0, sizeof(rtc_read));

	printf("Configuracao RTC aplicada: ");
	if (rtc_set_time(rtc_loaded, sizeof(rtc_loaded)) < 0){
		return -1;
	}

	sleep(RTC_TIME_SLEEP);

	printf("Feedback RTC apos %d seg. : ", RTC_TIME_SLEEP);
	if (rtc_get_time(rtc_read, sizeof(rtc_read)) < 0){
		return -1;
	}

	if (rtc_compare_time(rtc_loaded,rtc_read) < 0){
		return -1;
	}

	return 0;
}

struct fts_test rtc_test = {
		.name = "Teste do RTC - Real Time Clock",
		.hw_init = NULL,
		.test = rtc_tester,
		.hw_stop = NULL,
		.next = NULL,
};
