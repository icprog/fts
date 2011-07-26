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

#include <net/if.h>
#include <netinet/ip_icmp.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <librouter/options.h>

#include "fts-digistar.h"
#include "fts-efm.h"

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

struct fts_test head_test = { .name = "Head test (should not be used)", .hw_init = NULL, .test = NULL,
                .next = NULL, };

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

static int fts_digistar_test_startup_question()
{
	char cmd = 0;
	int ret = -1;
	pid_t pID, pID2;

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
			if (toupper((int) cmd) == 'Y')
				ret = 1;
			else
				ret = 0;
			kill(pID, SIGKILL);
			break;
		} else if (pID2 < 0) { // failed to fork
			syslog(LOG_ERR, "Could not startup FTS-Digistar\n");
			break;
		} else {
			waitpid(pID, NULL, 0);// parent
			kill(pID2, SIGKILL);
			ret = 0;
			break;
		}
	}
	waitpid(pID2, NULL, 0);
	return ret;
}

static void fts_digistar_fflushstdin(void)
{
	int c;
	while ((c = fgetc(stdin)) != EOF && c != '\n')
		;
}

int fts_digistar_ping(char *host, char *dev)
{
	int ret;
	char cmd[64];
	memset(&cmd, 0, sizeof(cmd));

	snprintf(cmd, sizeof(cmd), "/bin/ping -I %s -c 4 %s", dev, host);

	ret = system(cmd);
	syslog(LOG_CRIT, "Executando ping: [ %s ]", cmd);

	if (WIFEXITED(ret))
		return WEXITSTATUS(ret);
	else
		return -1;
}

static void fts_digistar_product_id_show(void)
{
	printf("$I%s\n", MODEL);
	syslog(LOG_CRIT, "$Ffim do teste de fabrica\n");

}

static int fts_digistar_ethernet_wan_test(void)
{
	char cmd = 'S';
	char cmd_sys[256];
	int ret = -1;

	printf("\n$QIniciar teste Ethernet Wan?\n");
	syslog(LOG_CRIT, "$QIniciar teste Ethernet Wan?\n");

	while (cmd == 'S') {
		scanf("%c", &cmd);

		if ((cmd = (char) toupper((int) cmd)) != 'S') {
			printf("$RSKIP\n");
			return -1;
		}

		syslog(LOG_CRIT, "Configurando [%s] com 10.1.1.99/255.255.0.0", DEV_WAN);
		memset(&cmd_sys, 0, sizeof(cmd_sys));
		sprintf(cmd_sys, "/sbin/ifconfig eth0 up 10.1.1.99 netmask 255.255.0.0"); /* flush */
		if (system(cmd_sys) != 0)
			return -1;

		if (fts_digistar_ping(HOST_PING, DEV_LAN) == 0)
			ret = 0;
		else
			ret = -1;

		if (ret == 0) {
			printf("\n$ROK\n\n");
			syslog(LOG_CRIT, "$ROK\n");
		} else {
			printf("\n$RERROR\n\n");
			syslog(LOG_CRIT, "$RERROR\n");
		}

		printf("$QRepetir teste Ethernet Wan?\n");
		syslog(LOG_CRIT, "$QRepetir teste Ethernet Wan?\n");
		fts_digistar_fflushstdin();
	}

	return 0;

}

static void fts_digistar_test_end(void)
{
	printf("$Ffim do teste de fabrica\n");
	syslog(LOG_CRIT, "$Ffim do teste de fabrica\n");
}

static int fts_do_tests(void)
{
	char cmd = 'S';
	char cmd_sys[256];
	int ret = -1;
	struct fts_test *t = head_test.next;

	if (t == NULL) {
		printf("ERROR: No tests registered\n");
		return -1;
	}

	for (; t != NULL; t = t->next) {

		printf("\n$QIniciar %s?\n", t->name);

		while (cmd == 'S') {
			scanf("%c", &cmd);

			if ((cmd = (char) toupper((int) cmd)) != 'S') {
				printf("$RSKIP\n");
				return -1;
			}

			if (t->hw_init != NULL)
				t->hw_init();

			ret = t->test();

			if (ret == 0) {
				printf("\n$ROK\n\n");
				syslog(LOG_CRIT, "$ROK\n");
			} else {
				printf("\n$RERROR\n\n");
				syslog(LOG_CRIT, "$RERROR\n");
			}

			printf("$QRepetir %s?\n", t->name);
			syslog(LOG_CRIT, "$QRepetir %s?\n", t->name);
			fts_digistar_fflushstdin();
		}
	}

	return 0;
}

static void fts_register_test(struct fts_test *t)
{
	struct fts_test *n;

	for (n = &head_test; n->next != NULL; n = n->next)
		;

	n->next = t;
}

static int main_fts()
{
	int i = 0, test = 0;
	test = fts_digistar_test_startup();
	printf("|TEST____ %d\n", test);
	if (!test) {
		printf("DEU MERDA - pid %d\n", getpid());
		return 0;
	}

	printf("TESTANDO - pid %d\n", getpid());
	int b = 5;
	while (b) {
		b--;
		printf("b\n");
		sleep(1);
	}

	return 0;
}

int main(int argc, char **argv)
{
	syslog(LOG_INFO, "FTS-Digistar Starting ...\n");

	//main_fts();
	//fts_digistar_ethernet_wan_test();
#ifdef OPTION_EFM
	fts_register_test(&efm_test);
#endif

	fts_do_tests();

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
