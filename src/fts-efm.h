/*
 * fts-efm.h
 *
 *  Created on: Jul 25, 2011
 *      Author: tgrande
 */

#ifndef FTS_EFM_H_
#define FTS_EFM_H_

#define EFM_MAX_CONNECTION_TIME	120

int do_dsp_connection(void);
int check_dsp_connection(void);
int check_dsp_parameters(void);

#endif /* FTS_EFM_H_ */
