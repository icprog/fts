/*
 * fts-digistar.h
 *
 *  Created on: Jul 20, 2011
 *      Author: ipinotti
 */

#ifndef FTS_DIGISTAR_H_
#define FTS_DIGISTAR_H_

#include <librouter/options.h>

#if defined(CONFIG_DIGISTAR_3G)
#define SERIES "RCG-SERIES ROUTER"

#if defined(CONFIG_DIGISTAR_RCG1000)
#define MODEL "RCG-1000"
#elif defined(CONFIG_DIGISTAR_RCG800)
#define MODEL "RCG-800"
#elif defined(CONFIG_DIGISTAR_RCG700)
#define MODEL "RCG-700"
#endif

#elif defined (CONFIG_DIGISTAR_MRG)
#define SERIES "MRG-SERIES ROUTER/BRIDGE"
#define MODEL  "MRG-1234" /* FIXME Make a librouter function */

#elif defined(CONFIG_DIGISTAR_EFM)
#define SERIES "ETL-SERIES ROUTER/BRIDGE"
#define MODEL	"ETL-8110" /* FIXME Make a librouter function */
#elif defined(CONFIG_DIGISTAR_EFM4ETH)
#define SERIES "ETL-SERIES ROUTER/BRIDGE"
#define MODEL	"ETL-8110" /* FIXME Make a librouter function */

#else
#error "Board not supported"
#endif

int pinger(char *ipaddr, char *device);
int ping(char *ipaddr, char *device, int size);
int set_ipaddr(char *dev, char *addr, char *mask);
void print_ok_msg(int ok);
void print_test_info(char *fmt, ...);


struct fts_test {
	char name[256];
	int (*hw_init)(void);
	int (*test)(void);
	int (*hw_stop)(void);
	struct fts_test *next;
};

#endif /* FTS_DIGISTAR_H_ */
