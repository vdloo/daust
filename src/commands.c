/* commands.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"

void print_kill_daemon()
{
	if (config->verbosity) {
		printf("Received stop command. Goodbye.\n");
	}
}

void kill_daemon()
{
	print_kill_daemon();
	terminate_config();
	exit(0);
}

void print_pong()
{
	if (config->verbosity) {
		printf("pong\n");
	}
}

char *pong()
{
	print_pong();
	return strdup("pong");
}
