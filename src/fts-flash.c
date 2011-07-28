/*
 * fts-flash.c
 *
 *  Created on: Jul 26, 2011
 *      Author: ipinotti
 */
#include <stdio.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <stdlib.h>

#include <librouter/options.h>
#include <librouter/nv.h>

#include "fts-digistar.h"
#include "fts-flash.h"

static int flash_generate_ram_config_file(char * filename)
{
	FILE *fd;

	if ((fd = fopen(filename, "w+")) == NULL) {
		syslog(LOG_ERR, "Could not create flash saved conf. file\n");
		return -1;
	}
	fclose(fd);

	return 0;
}

static int flash_save_residual_data_to_file(char * filename)
{
	if (flash_generate_ram_config_file(filename) < 0)
		return -1;

	if (librouter_nv_load_configuration(filename) < 0)
		return -1;

	return 0;
}

static int flash_compare_data(char * filename)
{
	FILE *fd_current, *fd_orig;
	char line_cur[128] = { (int) NULL };
	char line_orig[128] = { (int) NULL };

	if (flash_save_residual_data_to_file(FLASH_FILE_TEST_DATA) < 0)
		return -1;

	if ((fd_current = fopen(FLASH_FILE_TEST_DATA, "r")) == NULL) {
		syslog(LOG_ERR, "Could not read flash saved conf. file\n");
		return -1;
	}

	if ((fd_orig = fopen(filename, "r")) == NULL) {
		syslog(LOG_ERR, "Could not read file saved conf. file\n");
		return -1;
	}

	while ( (fgets(line_cur, sizeof(line_cur), fd_current) != NULL) && (fgets(line_orig, sizeof(line_orig), fd_orig) != NULL) ) {
		if (strncmp(line_cur, line_orig, strlen(line_orig)))
			return -1;
	}

	fclose(fd_current);
	fclose(fd_orig);
	return 0;
}

static int flash_load_test_file(void)
{
	printf("Carregando configuracao teste [default] para flash\n");
	if (librouter_nv_save_configuration(DEV_DEFAULT_CONFIG) < 0)
		return -1;

	return 0;
}

static int flash_restore_residual_data(void)
{
	printf("Carregando configuracao salva para flash\n");
	if (librouter_nv_save_configuration(FLASH_FILE_SAVED_DATA) < 0)
		return -1;

	return 0;
}

static int flash_restore(void)
{
	if (flash_restore_residual_data() < 0)
		return -1;

	if (flash_compare_data(FLASH_FILE_SAVED_DATA) < 0)
		return -1;

	return 0;
}

static int flash_testing(void)
{
	if (flash_load_test_file() < 0)
		return -1;

	if (flash_compare_data(DEV_DEFAULT_CONFIG) < 0)
		return -1;

	return 0;
}

static int flash_backup(void)
{
	printf("Salvando configuracao existente em flash\n");
	if (flash_save_residual_data_to_file(FLASH_FILE_SAVED_DATA) < 0)
		return -1;

	return 0;
}
static int flash_tester(void)
{
	if (flash_backup() < 0)
		return -1;

	if (flash_testing() < 0)
		return -1;

	if (flash_restore() < 0)
		return -1;

	return 0;
}

struct fts_test flash_test = {
		.name = "Teste da Flash",
		.hw_init = NULL,
		.test = flash_tester,
		.hw_stop = NULL,
		.next = NULL,
};
