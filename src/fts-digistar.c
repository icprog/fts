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

#include <librouter/dev.h> /* get_dev_link */
#include <librouter/usb.h>
#include <librouter/device.h>
#include <librouter/modem3G.h>
#include <librouter/quagga.h>
#include <librouter/str.h>

#include "terminal.h"
#include "fts-digistar.h"
#include "fts-efm.h"
#include "fts-ethernet.h"
#include "fts-rtc.h"
#include "fts-flash.h"

#if defined (OPTION_FTS_DIGISTAR)

struct fts_test head_test = {
		.name = "Head test (should not be used)",
		.hw_init = NULL,
		.test = NULL,
		.hw_stop = NULL,
                .next = NULL,
};

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
	syslog(LOG_CRIT,"$I%s\n",MODEL);
}

static void fts_digistar_test_end(void)
{
	printf("\n$Ffim do teste de fabrica\n\n");
	syslog(LOG_CRIT,"$Ffim do teste de fabrica\n");
}

#if 0
static void fts_digistar_fflushstdin(void)
{
	int c;
	while ((c = fgetc(stdin)) != EOF && c != '\n');
}
#endif

void sig_handler(int signal)
{
	/* Just exit for now */
	reload_termios();
	exit(0);
}

static int in_cksum(unsigned short *buf, int sz)
{

	int nleft = sz;
	int sum = 0;
	unsigned short *w = buf;
	unsigned short ans = 0;

	while (nleft > 1) {
		sum += *w++;
		nleft -= 2;
	}

	if (nleft == 1) {
		*(unsigned char *) (&ans) = *(unsigned char *) w;
		sum += ans;
	}

	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	ans = ~sum;
	return ans;
}

int ping(char *ipaddr, char *device, int size)
{

	struct sockaddr_in pingaddr;
	struct icmp *pkt;
	int pingsock, c, i, ret = -1;
	long arg;
	char packet[size + ICMP_MINLEN];
	struct ifreq ifr;

	if (librouter_dev_get_link_running(device) <= 0)
		return -1;

	pingsock = socket(AF_INET, SOCK_RAW, 1); /* 1 == ICMP */
	pingaddr.sin_family = AF_INET;
	pingaddr.sin_addr.s_addr = inet_addr(ipaddr);

	/* Force source address to be of the interface we want */
	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name, device, IFNAMSIZ - 1);
	ioctl(pingsock, SIOCGIFADDR, &ifr);

	if (bind(pingsock, (struct sockaddr*) &ifr.ifr_addr, sizeof(struct sockaddr_in)) == -1) {
		perror("bind");
		exit(2);
	}

	pkt = (struct icmp *) packet;
	memset(pkt, 0, sizeof(packet));

	pkt->icmp_type = ICMP_ECHO;
	pkt->icmp_cksum = in_cksum((unsigned short *) pkt, sizeof(packet));

	c = sendto(pingsock, packet, size + ICMP_MINLEN, 0, (struct sockaddr *) &pingaddr,
	                sizeof(pingaddr));

	/* Set non-blocking */
	if ((arg = fcntl(pingsock, F_GETFL, NULL)) < 0) {
		fprintf(stderr, "Error fcntl(..., F_GETFL) (%s)\n", strerror(errno));
		return -1;
	}

	arg |= O_NONBLOCK;

	if (fcntl(pingsock, F_SETFL, arg) < 0) {
		fprintf(stderr, "Error fcntl(..., F_SETFL) (%s)\n", strerror(errno));
		return -1;
	}

	/* listen for replies */
	i = 30; /* Number of attempts */
	while (i--) {
		struct sockaddr_in from;
		socklen_t fromlen = sizeof(from);

		c = recvfrom(pingsock, packet, sizeof(packet), 0, (struct sockaddr *) &from,
		                &fromlen);

		if (c < 0) {
			usleep(1000);
			continue;
		}

		if (c >= 64) { /* ip + icmp */
			struct iphdr *iphdr = (struct iphdr *) packet;

			pkt = (struct icmp *) (packet + (iphdr->ihl << 2)); /* skip ip hdr */
			if (pkt->icmp_type == ICMP_ECHOREPLY) {
				ret = 0;
				break;
			}
		}
	}

	close(pingsock);
	return ret;
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
	int ret = -1;
	struct fts_test *t = head_test.next;

	if (t == NULL) {
		printf("ERROR: No tests registered\n");
		return -1;
	}

	for (; t != NULL; t = t->next) {

		printf("\n$QIniciar %s?\n", t->name);

		while (1) {
			if (fts_get_answer() == 0) {
				printf("$RSKIP\n");
				break;
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
		}
	}

	return 0;
}

static void fts_register_test(struct fts_test *t)
{
	struct fts_test *n;

	for (n = &head_test; n->next != NULL; n = n->next);

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

#if defined(CONFIG_DIGISTAR_3G)
	fts_register_test(&ethlan_test);
	fts_register_test(&ethwan_test);
#elif defined(CONFIG_DIGISTAR_EFM)
	fts_register_test(&ethlan_test);
	fts_register_test(&efm_test);
#else
#error "Board not suppoted"
#endif

	fts_register_test(&rtc_test);
	fts_register_test(&flash_test);

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
