/*
 * fts-digistar.h
 *
 *  Created on: Jul 20, 2011
 *      Author: ipinotti
 */

#ifndef FTS_DIGISTAR_H_
#define FTS_DIGISTAR_H_

#include <librouter/options.h>

#ifdef CONFIG_DIGISTAR_3G
#define SERIES "RCG-SERIES ROUTER"

#ifdef CONFIG_DIGISTAR_RCG1000
#define MODEL "RCG-1000"
#endif

#ifdef CONFIG_DIGISTAR_RCG800
#define MODEL "RCG-800"
#endif

#ifdef CONFIG_DIGISTAR_RCG700
#define MODEL "RCG-700"
#endif

#elif CONFIG_DIGISTAR_EFM
#define SERIES "ETL-SERIES ROUTER/BRIDGE"

#ifdef CONFIG_DIGISTAR_ETL8110
#define MODEL "RCG-1000"
#endif

#ifdef CONFIG_DIGISTAR_ETL8210
#define MODEL "RCG-800"
#endif

#ifdef CONFIG_DIGISTAR_ETL8221
#define MODEL "RCG-700"
#endif

#ifdef CONFIG_DIGISTAR_ETL8421
#define MODEL "ETL-8421"
#endif

#endif

#endif /* FTS_DIGISTAR_H_ */
