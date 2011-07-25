/*
 ============================================================================
 Name        : fts-digistar-EFM.c
 Author      : Igor Kramer Pinotti
 Version     : 0.1
 Copyright   : Your copyright notice
 Description : Factory Test System for Digistar Products (RCG and ETL series)
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <syslog.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#include <net/if.h>
#include <netinet/ip_icmp.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <librouter/options.h>

#include "fts-digistar.h"

#ifdef OPTION_MODEM3G
#include <librouter/dev.h> /* get_dev_link */
#include <librouter/usb.h>
#include <librouter/device.h>
#include <librouter/modem3G.h>
#include <librouter/quagga.h>
#include <librouter/str.h>
#endif

/* Temporary*/
#define OPTION_FTS_DIGISTAR


#if defined (OPTION_FTS_DIGISTAR)

static void fts_digistar_enable_log(int enable)
{
	freopen("/var/log/fts-digistar_log", "a+", stdin);
	freopen("/var/log/fts-digistar_log", "a+", stdout);
	freopen("/var/log/fts-digistar_log", "a+", stderr);
}

static void fts_digistar_welcome_string_show()
{
	printf("\n\tWelcome to Digistar %s\n", SERIES);
	printf("\tPress Y within 3 seconds to enter in test...\n\n");
}

static int fts_digistar_test_startup()
{
	char cmd=0;
	int ret=-1;
	pid_t pID, pID2,p;

	fts_digistar_welcome_string_show();

	switch (pID = fork()) {
		case -1:
			syslog(LOG_ERR, "Could not startup FTS-Digistar\n");
			break;

		case 0: /* Child */
			sleep(3);
			exit(EXIT_SUCCESS);
			break;

		default: /* Parent */
			pID2 = fork();
			if (pID2 == 0) {// child
				scanf("%c", &cmd);
				if (cmd == 'Y' || cmd == 'y')
					ret = 1;
				else
					ret = 0;
				kill( pID, SIGKILL );
				break;
			} else if (pID2 < 0) {          // failed to fork
				syslog(LOG_ERR, "Could not startup FTS-Digistar\n");
				break;
			} else {
				waitpid(pID,NULL,0);// parent
				kill(pID2,SIGKILL);
				ret = 0;
				break;
			}
	}
	waitpid(pID2,NULL,0);
	return ret;
}

static int main_fts()
{
	int i = 0,test = 0;
	test = fts_digistar_test_startup();
	printf("|TEST____ %d\n",test);
	if (!test){
		printf("DEU MERDA - pid %d\n",getpid());
		return 0;
	}

	printf("TESTANDO - pid %d\n",getpid());
	int b=5;
	while (b){
		b--;
		printf("b\n");
		sleep(1);
	}

	return 0;
}

int main(int argc, char **argv) {
	syslog(LOG_INFO, "FTS-Digistar WORKING ...\n");

	main_fts();

	printf("fechou");

	syslog(LOG_INFO, "FTS-Digistar exiting ...\n");
	return EXIT_SUCCESS;
}

#else /* OPTION_FTS_DIGISTAR */
int main(int argc, char **argv)
{
	syslog(LOG_INFO, "FTS-Digistar not supported, exiting ...\n");
	return 0;
}
#endif /* OPTION_FTS_DIGISTAR */
