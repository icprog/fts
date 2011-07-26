/*
 * terminal.c
 *
 * Based on CISH terminal_echo.c
 *
 *  Created on: Jul 26, 2011
 *      Author: tgrande
 */

#include <stdlib.h>
#include <stdio.h>

#include <termios.h>
#include <string.h>

static struct termios stored_settings, startup_settings;

static void echo_off(void)
{
	struct termios new_settings;
	tcgetattr(0, &stored_settings);
	new_settings = stored_settings;
	new_settings.c_lflag &= (~ECHO);
	tcsetattr(0, TCSANOW, &new_settings);
	return;
}

static void echo_on(void)
{
	tcsetattr(0, TCSANOW, &stored_settings);
	return;
}

static void canon_off(void)
{
	struct termios new_settings;
	tcgetattr(0, &stored_settings);
	new_settings = stored_settings;
	new_settings.c_lflag &= (~ICANON);
	tcsetattr(0, TCSANOW, &new_settings);
	return;
}

static void canon_on(void)
{
	echo_on();
}

void save_termios(void)
{
	tcgetattr(0, &startup_settings);
	return;
}

void reload_termios(void)
{
	tcsetattr(0, TCSANOW, &startup_settings);
	return;
}

int fts_get_answer(void)
{
	int in;
	struct termios initial_settings, new_settings;

	fflush(stdout);

	/* Flushes stdin */
	new_settings.c_cc[VMIN] = 0; /* Minimum number of bytes is stdin to allow a read() */
	new_settings.c_cc[VTIME] = 0; /* Maximum time to wait for input in a read() */
	tcgetattr(0, &initial_settings);
	new_settings = initial_settings;
	new_settings.c_lflag &= ~ICANON;
	new_settings.c_cc[VMIN] = 0;
	new_settings.c_cc[VTIME] = 0;
	tcsetattr(0, TCSANOW, &new_settings);
	while (fgetc(stdin) != EOF)
		; /* Empty stdin */
	tcsetattr(0, TCSANOW, &initial_settings);

	/* Wait for input in non-canonical mode */
	canon_off();
	echo_off();

	in = fgetc(stdin);

	canon_on();

	if ((in == 'y') || (in == 'Y') || (in == 's') || (in == 'S'))
		return 1;

	return 0;
}

