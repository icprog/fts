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
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

#include <linux/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <librouter/options.h>
#include <librouter/usb.h>

#include "fts-digistar.h"
#include "fts-usb.h"


static int usb_dongle_compare(librouter_usb_dev * usb, int enable)
{
	if (!enable)
		return 0;

	switch (usb->port) {
	case USB_PORT_1:
		if ((usb->product_id == USB_P1_IDPRODUCT) && (usb->vendor_id == USB_P1_IDVENDOR))
			return 0;
		break;

	case USB_PORT_2:
		if ((usb->product_id == USB_P2_IDPRODUCT) && (usb->vendor_id == USB_P2_IDVENDOR))
			return 0;
		break;

	default:
		return -1;
		break;
	}

	return -1;
}
static int usb_test_port(int usb_port)
{
	librouter_usb_dev usb;
	memset(&usb, 0, sizeof(librouter_usb_dev));
	usb.port = usb_port;

	print_test_info("Verificando porta USB [%d] ", usb.port);
	if (librouter_usb_device_is_connected(usb.port)) {
		if (librouter_usb_get_descriptor(&usb) < 0)
			goto usb_end;

		if (usb_dongle_compare(&usb, USB_DONGLE_COMPARE_ENABLE) < 0)
			goto usb_end;

		print_ok_msg(1);
		print_test_info("Dispositivo USB na porta %d\n", usb.port);
		print_test_info("Product_ID [%04x] - %s / Vendor_ID [%04x] - %s\n\n", usb.product_id,
		                usb.product_str, usb.vendor_id, usb.manufacture_str);
		return 0;
	}

usb_end:
	print_ok_msg(0);

	return -1;
}

static int usb_tester(void)
{
	int ret = 0;

	ret |= usb_test_port(USB_PORT_1);

	ret |= usb_test_port(USB_PORT_2);

	if (ret < 0)
		return -1;

	return 0;
}

struct fts_test usb_test = {
		.name = "Teste das portas USBs",
		.hw_init = NULL,
		.test = usb_tester,
		.hw_stop = NULL,
		.next = NULL,
};
