/* commands.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "utils.h"
#include "init.h"
#include "node_data.h"

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

char *trace()
{
	int m_siz = 0;
	int *mp = &m_siz;
	char *r;
	r = asdtobfp(r, mp, "_ ", head->info->hostname);
	r = astobfp(r, mp, NULL);
	return r;
}
