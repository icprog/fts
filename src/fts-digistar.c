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
#ifdef OPTION_MODEM3G
#include <librouter/dev.h> /* get_dev_link */
#include <librouter/usb.h>
#include <librouter/device.h>
#include <librouter/modem3G.h>
#include <librouter/quagga.h>
#include <librouter/str.h>
#endif

#include "terminal.h"
#include "fts-digistar.h"
#include "fts-efm.h"
#include "fts-ethernet.h"

/* Temporary*/
#define OPTION_FTS_DIGISTAR

#if defined (OPTION_FTS_DIGISTAR)

struct fts_test head_test = { .name = "Head test (should not be used)", .hw_init = NULL, .test = NULL,
                .next = NULL, };

#if 0
static void fts_digistar_enable_log(int enable)
{
	freopen("/var/log/fts-digistar_log", "a+", stdin);
	freopen("/var/log/fts-digistar_log", "a+", stdout);
	freopen("/var/log/fts-digistar_log", "a+", stderr);
}
#endif

static void show_welcome_string()
{
	printf("\n\tWelcome to Digistar %s\n", SERIES);
	printf("\tPress Y within 3 seconds to enter in test...\n\n");
}

static void show_product_id(void)
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
	while ((c = fgetc(stdin)) != EOF && c != '\n');
}

void sig_handler(int signal)
{
	/* Just exit for now */
	reload_termios();
	exit(0);
}

int ping(char *host, char *dev)
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

int set_ipaddr(char *dev, char *addr, char *mask)
{
	char cmd_sys[256];

	syslog(LOG_CRIT, "Configurando [%s] com %s/%s", dev, addr, mask);

	memset(&cmd_sys, 0, sizeof(cmd_sys));
	snprintf(cmd_sys, sizeof(cmd_sys), "/sbin/ifconfig %s up %s netmask %s", dev, addr, mask);

	if (system(cmd_sys) != 0)
		return -1;

	return 0;
}


static int startup_test(void)
{
	show_welcome_string();

	return fts_get_answer();
}



static int do_tests(void)
{
	char cmd = 'S';
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

			if (t->hw_stop != NULL)
				t->hw_stop();

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
	signal(SIGALRM, sig_handler);
	alarm(3); /* SIGALRM in 3 seconds */

	if (!startup_test())
		return -1;

	alarm(0); /* Remove schedule of SIGALRM */

	/*Show product model*/
	show_product_id();

	/*Test Functions...*/
	do_tests();

	/*End string*/
	fts_digistar_test_end();
	return 0;
}

int main(int argc, char **argv)
{
	syslog(LOG_INFO, "FTS-Digistar Starting ...\n");
	save_termios();

#ifdef OPTION_EFM
	fts_register_test(&efm_test);
#endif
	fts_register_test(&ethlan_test);

	main_fts();

	syslog(LOG_INFO, "FTS-Digistar Exiting ...\n");
	reload_termios();
	return EXIT_SUCCESS;
}

#else /* OPTION_FTS_DIGISTAR */
int main(int argc, char **argv)
{
	syslog(LOG_INFO, "FTS-Digistar not supported, exiting ...\n");
	return 0;
}
#endif /* OPTION_FTS_DIGISTAR */
