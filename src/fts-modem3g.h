/*
 * fts-modem3g.h
 *
 *  Created on: Aug 3, 2011
 *      Author: ipinotti
 */

#ifndef FTS_MODEM3G_H_
#define FTS_MODEM3G_H_

#if defined (OPTION_MODEM3G)

#define SIM_CARD_P1 1
#define SIM_CARD_P2 0
#define MAX_WAIT_TIME_CONNECTION_3G 120
#define MODEM3G_USB_PORT 3
#define MODEM3G_BUILTIN "ppp0"
#define PPPD_BIN_FILE 	"/sbin/pppd"
#define IP_DEST_TEST_3G "8.8.8.8"
#define MODEM3G_PPPD_PID "/var/run/ppp0.pid"

struct fts_test modem3g_test_sc1;
struct fts_test modem3g_test_sc2;

#endif

#endif /* FTS_MODEM3G_H_ */
