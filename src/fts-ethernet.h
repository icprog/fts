/*
 * fts-ethernet.h
 *
 *  Created on: Jul 26, 2011
 *      Author: tgrande
 */

#ifndef FTS_ETHERNET_H_
#define FTS_ETHERNET_H_

/* General Defines*/
/* WAN */
#define WAN_IP "10.1.1.99"
#define WAN_MASK "255.255.0.0"
#define WAN_DEV "eth1"

/* WAN HOST PING */
#define HOST_0_PING "192.168.100.130"


/* LAN */
#define LAN_IP "192.168.100.131"
#define LAN_MASK "255.255.255.0"
#define LAN_DEV "eth0"

/* LAN HOST PING */
#define HOST_1_PING "10.1.1.1"
#define HOST_2_PING "10.1.1.2"
#define HOST_3_PING "10.1.1.3"
#define HOST_4_PING "10.1.1.4"

struct fts_test ethwan_test;
struct fts_test ethlan_test;

#endif /* FTS_ETHERNET_H_ */
