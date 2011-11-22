/*
 * fts-ethernet.h
 *
 *  Created on: Jul 26, 2011
 *      Author: tgrande
 */

#ifndef FTS_ETHERNET_H_
#define FTS_ETHERNET_H_

#include <librouter/options.h>

#ifdef CONFIG_DEVELOPMENT
#define PD3_LOCAL_TEST
#endif

/* General Defines*/
/* WAN */
#ifdef PD3_LOCAL_TEST
#define WAN_IP 		"10.1.1.96"
#else
#define WAN_IP 		"10.0.0.90"
#endif

#ifdef PD3_LOCAL_TEST
#define WAN_MASK 	"255.255.0.0"
#else
#define WAN_MASK 	"255.255.255.0"
#endif

#define WAN_DEV 	"eth1"

/* WAN HOST PING */
#ifdef PD3_LOCAL_TEST
#define HOST_0_PING 	"10.1.1.1"
#else
#define HOST_0_PING 	"10.0.0.1"
#endif

/* LAN */
#ifdef PD3_LOCAL_TEST
#define LAN_IP 		"192.168.1.9"
#else
#define LAN_IP 		"10.1.1.90"
#endif

#define LAN_MASK 	"255.255.255.0"
#define LAN_DEV 	"eth0"
#define LAN_N_PORTS	4

/* LAN HOST PING */
#ifdef PD3_LOCAL_TEST
#define HOST_1_PING 	"192.168.1.1"
#else
#define HOST_1_PING 	"10.1.1.1"
#endif

#define HOST_2_PING 	"10.1.1.2"
#define HOST_3_PING 	"10.1.1.3"
#define HOST_4_PING 	"10.1.1.4"


struct fts_test ethwan_test;
struct fts_test ethlan_test;

#endif /* FTS_ETHERNET_H_ */
