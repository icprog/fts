/*
 * fts-efm.h
 *
 *  Created on: Jul 25, 2011
 *      Author: tgrande
 */

#ifndef FTS_EFM_H_
#define FTS_EFM_H_

#define EFM_MAX_CONNECTION_TIME		260 /* valor aumentado de 180 para 260, devido a falha na primeira tentativa de conex do EFM*/
#define EFM_MAX_ACCEPTED_ERRORS		5

#define EFM_DEV 	"eth1"
#define EFM_IPADDR 	"10.0.0.90"
#define EFM_IPDEST 	"10.0.0.1"
#define EFM_IPMASK 	"255.255.255.0"


struct fts_test efm_test;

#endif /* FTS_EFM_H_ */
