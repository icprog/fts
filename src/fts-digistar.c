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
#include <ctype.h>
#include <fcntl.h>

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

static void fts_digistar_product_id_show(void)
{
	printf("$I%s\n",MODEL);
	syslog(LOG_CRIT,"$Ffim do teste de fabrica\n");
}

static void fts_digistar_test_end(void)
{
	printf("\n$Ffim do teste de fabrica\n\n");
	syslog(LOG_CRIT,"$Ffim do teste de fabrica\n");
}

static void fts_digistar_fflushstdin(void)
{
    int c;
    while( (c = fgetc( stdin )) != EOF && c != '\n' );
}

static int fts_digistar_ping(char * host, char * dev)
{
	int ret;
	char cmd[64];
	memset(&cmd, 0, sizeof(cmd));

	snprintf(cmd, sizeof(cmd), "/bin/ping -I %s -c 4 %s", dev, host);

	ret = system(cmd);
	syslog(LOG_CRIT,"Executando ping: [ %s ]",cmd);

	if(WIFEXITED(ret))
		return WEXITSTATUS(ret);
	else
		return -1;
}

static int fts_digistar_test_startup(void)
{
	time_t start, work ;
	char cmd[2];
	int flags = 0, flags_bkp = 0;
	memset(&cmd, 0, sizeof(cmd));

	fts_digistar_welcome_string_show();

	/*Tornar fgets non-blocking*/ //TODO
#if 0
	flags = fcntl(stdin, F_GETFL, 0);

	printf("flags is %d\n",flags);

	flags |= O_NONBLOCK;

	printf("flags after %d\n",flags);

	fcntl(stdin, F_SETFL, flags);
#endif

	time (&start) ;
	work = start ;
	while ( (work - start) < 3 ) time (&work);

	fgets(cmd, sizeof(cmd), stdin);
	fts_digistar_fflushstdin();

	if (toupper((int)cmd[0]) == 'Y')
		return 1;
	else
		return 0;
}

static int fts_digistar_ethernet_wan_test(void)
{
	char cmd = 'S';
	char cmd_sys[256];
	int ret = -1;

	printf("\n$QIniciar teste Ethernet Wan?\n");
	syslog(LOG_CRIT,"$QIniciar teste Ethernet Wan?\n");

	while (cmd == 'S'){
		scanf("%c", &cmd);

		if ( (cmd = (char)toupper((int)cmd)) != 'S'){
			printf("$RSKIP\n");
			return -1;
		}

		syslog(LOG_CRIT,"Configurando [%s] com %s/%s", WAN_DEV, WAN_IP, WAN_MASK);
		memset(&cmd_sys, 0, sizeof(cmd_sys));
		snprintf(cmd_sys, sizeof(cmd_sys), "/sbin/ifconfig %s up %s netmask %s", WAN_DEV, WAN_IP, WAN_MASK); /* flush */
		if (system(cmd_sys) != 0)
			return -1;

		if (fts_digistar_ping(HOST_0_PING, WAN_DEV) == 0)
			ret = 0;
		else
			ret = -1;

		if(ret == 0){
			printf("\n$ROK\n\n");
			syslog(LOG_CRIT,"$ROK\n");
		}
		else{
			printf("\n$RERROR\n\n");
			syslog(LOG_CRIT,"$RERROR\n");
		}

		printf("$QRepetir teste Ethernet Wan?\n");
		syslog(LOG_CRIT,"$QRepetir teste Ethernet Wan?\n");
		fts_digistar_fflushstdin();
	}

	return 0;

}

static int main_fts()
{
	if (!fts_digistar_test_startup()){
		printf("\nNo good..\n\n");
		return -1;
	}
	printf("Starting test\n\n");

	/*Show product model*/
	fts_digistar_product_id_show();

	/*Test Functions...*/
	fts_digistar_ethernet_wan_test();


	/*End string*/
	fts_digistar_test_end();
	return 0;
}

int main(int argc, char **argv) {
	syslog(LOG_INFO, "FTS-Digistar Starting ...\n");

	main_fts();

	syslog(LOG_INFO, "FTS-Digistar Exiting ...\n");
	return EXIT_SUCCESS;
}

#else /* OPTION_FTS_DIGISTAR */
int main(int argc, char **argv)
{
	syslog(LOG_INFO, "FTS-Digistar not supported, exiting ...\n");
	return 0;
}
#endif /* OPTION_FTS_DIGISTAR */
