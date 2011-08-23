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
#include <time.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <linux/rtc.h>

#include <librouter/options.h>
#include <librouter/libtime.h>

#include "fts-digistar.h"
#include "fts-rtc.h"

static struct tm ctm;

static int rtc_get_time(struct tm *tm_time)
{
	return librouter_time_get_rtc_date(tm_time);
}

static int rtc_set_time(struct tm *tm_time)
{
	if (librouter_time_set_date(RTC_DAY, RTC_MONTH, RTC_YEAR, RTC_HOUR, RTC_MIN, RTC_SEC) < 0)
		return -1;

	if (rtc_get_time(tm_time) < 0)
		return -1;

	return 0;
}

static int rtc_compare_time(struct tm *t1, struct tm *t2)
{
	if (t1->tm_min != t2->tm_min)
		return -1;

	if (t1->tm_mday != t2->tm_mday)
		return -1;

	if (t1->tm_mon != t2->tm_mon)
		return -1;

	if (t1->tm_year != t2->tm_year)
		return -1;

	if (t1->tm_hour != t2->tm_hour)
		return -1;

	if (t1->tm_sec != (t2->tm_sec - 1))
		return -1;

	return 0;
}

static int rtc_tester(void)
{
	struct tm time1, time2;
	int i = 0;

	if (rtc_set_time(&time1) < 0) {
		return -1;
	}

	printf("$TConfiguracao RTC aplicada!\n");

	usleep(5000);

	while(i++ < RTC_NUM_READS) {
		sleep(RTC_TIME_SLEEP);

		printf("$TLeitura do RTC apos %d seg.\n", i);

		if (rtc_get_time(&time2) < 0) {
			return -1;
		}

		if (rtc_compare_time(&time1, &time2) < 0) {
			return -1;
		}

		memcpy(&time1, &time2, sizeof(struct tm));
	}

	return 0;
}

static int rtc_test_init(void)
{
	char buf[128];
	printf("$TSalvando Hora do Sistema: ");
	librouter_time_get_rtc_date(&ctm);
	strftime(buf, sizeof(buf), "%a %b %e %H:%M:%S %Z %Y", &ctm);
	puts(buf);
	return 0;
}

static int rtc_test_exit(void)
{
	printf("$TRestaurando Hora do Sistema: ");
	if (librouter_time_set_date_from_tm(&ctm) < 0) {
		printf("[FAIL]\n");
		return -1;
	}

	printf("[OK]\n");
	return 0;
}

struct fts_test rtc_test = {
		.name = "Teste do RTC - Real Time Clock",
		.hw_init = rtc_test_init,
		.test = rtc_tester,
                .hw_stop = rtc_test_exit,
                .next = NULL,
};
