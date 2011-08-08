/*
 * fts-usb.h
 *
 *  Created on: Aug 3, 2011
 *      Author: ipinotti
 */

#ifndef FTS_USB_H_
#define FTS_USB_H_

//#if defined (OPTION_USB)

/*######################################################################################
 * Verificação OPCIONAL..
 * pode ser atribuido valor 0 a fim de evitar dependencia com um modelo determinado de PENDRIVE
 */
#define USB_DONGLE_COMPARE_ENABLE 1
/*#####################################################################################*/

#define USB_PORT_1		 1
#define USB_PORT_2		 2

#define USB_P1_IDPRODUCT 5699
#define USB_P1_IDVENDOR  2385

#define USB_P2_IDPRODUCT 5699
#define USB_P2_IDVENDOR  2385

struct fts_test usb_test;

//#endif

#endif /* FTS_USB_H_ */
